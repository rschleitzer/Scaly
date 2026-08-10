/* Evented-I/O backend for Win64 — IOCP (stage 7, brocken 2).
 *
 * A SEPARATE FILE from eio.c, unlike kqueue and epoll which share it. Those
 * two differ only in their backend half and share everything else verbatim;
 * Windows shares nothing — a different notification MODEL, different socket
 * calls, different error reporting. Folding it in as a third #ifdef arm would
 * have wrapped the whole working POSIX file in conditionals to add a target
 * that reuses none of it. tools/eio.sh selects per OS, exactly as
 * tools/fcontext.sh selects per ABI.
 *
 * ============================ THE MODEL GAP ============================
 *
 * The contract this file must satisfy is READINESS, oneshot: `arm(q, fd,
 * for_write, tag)` says "report `tag` once, when this fd can be read/written",
 * and `wait` returns the tags that fired. kqueue and epoll are readiness
 * mechanisms, so for them that is a direct translation.
 *
 * IOCP is not. It reports COMPLETION — "the operation you started has
 * finished" — so readiness has to be manufactured, and the manufacturing
 * differs by what the socket IS:
 *
 *   connected socket, read   -> a ZERO-BYTE WSARecv. It completes when data
 *                               arrives or the peer closes, without consuming
 *                               anything, so the caller's own recv still sees
 *                               the bytes. This is the standard technique.
 *   connected socket, write  -> a zero-byte WSASend. See the caveat below; it
 *                               is the weakest of the three.
 *   LISTENING socket         -> neither works: a listener cannot recv. It
 *                               needs AcceptEx, which inverts the accept path
 *                               (the accepted socket is created BEFORE the
 *                               connection arrives, not returned by accept()).
 *
 * So arm() has to know the socket's role, and it asks the kernel rather than
 * guessing: SO_ACCEPTCONN reports whether a socket is listening.
 *
 * ★ THE WRITE-READINESS CAVEAT, stated plainly because it is a real narrowing:
 * a zero-byte WSASend generally completes at once, since Winsock accepts it
 * into the socket buffer rather than waiting for room. So arming for write
 * reports ready almost immediately, and a caller whose real send then returns
 * WSAEWOULDBLOCK will re-arm and be told ready again. That is not incorrect —
 * the contract promises "you may try", not "this will succeed" — but it can
 * spin where kqueue/epoll would block. It is acceptable here because the only
 * write-arming caller re-arms after a partial write, which makes progress;
 * a future caller that arms for write and does nothing else would burn CPU.
 *
 * ============================ HANDLES AS int ============================
 *
 * The API carries queues and sockets as `int`, and on Win64 a HANDLE and a
 * SOCKET are 64-bit. Truncating is nonetheless correct, and by documented
 * guarantee rather than by luck: 64-bit Windows defines handle values to have
 * only 32 significant bits precisely so they can cross 32-bit interfaces. Do
 * not "fix" this to size_t without a reason — the width lives in the Scaly
 * declarations, so widening it costs a seed refresh on every target.
 */

#ifdef _WIN32

#include <winsock2.h>   /* before windows.h — it owns the socket API */
#include <ws2tcpip.h>
#include <mswsock.h>    /* AcceptEx, SO_UPDATE_ACCEPT_CONTEXT */
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>      /* scaly_eio_errno's CRT half */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SCALY_EIO_MAX_EVENTS 64

/* ---- one-time Winsock startup ---------------------------------------- */

static INIT_ONCE scaly_ws_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK scaly_ws_init(PINIT_ONCE o, PVOID p, PVOID* c)
{
    WSADATA d;
    (void)o; (void)p; (void)c;
    return WSAStartup(MAKEWORD(2, 2), &d) == 0;
}

static void scaly_ws_start(void)
{
    InitOnceExecuteOnce(&scaly_ws_once, scaly_ws_init, NULL, NULL);
}

/* ---- per-arm state ----------------------------------------------------
 *
 * One of these per arm(), freed when wait() dequeues its completion. The
 * contract's "at most one armed waiter per fd" is what makes a per-arm
 * allocation sound: there is never a second outstanding operation to collide
 * with. OVERLAPPED must be FIRST — the completion hands back its address and
 * we cast straight back to the enclosing struct.
 */
#define SC_ARM_RECV   0
#define SC_ARM_SEND   1
#define SC_ARM_ACCEPT 2

typedef struct SC_ARM {
    OVERLAPPED ov;                 /* MUST be first */
    void*      tag;
    SOCKET     s;                  /* the armed socket */
    int        kind;
    SOCKET     accepted;           /* SC_ARM_ACCEPT: pre-created by AcceptEx */
    char       addrs[2 * (sizeof(struct sockaddr_in6) + 16)];
} SC_ARM;

/* AcceptEx completions leave their socket here, keyed by the LISTENER, for
 * scaly_eio_accept to pick up. One slot per listener is enough for the same
 * reason the per-arm allocation is: one armed waiter per fd. */
#define SC_ACCEPT_SLOTS 64
static SOCKET sc_acc_listener[SC_ACCEPT_SLOTS];
static SOCKET sc_acc_ready[SC_ACCEPT_SLOTS];
static SRWLOCK sc_acc_lock = SRWLOCK_INIT;

static void sc_acc_put(SOCKET listener, SOCKET accepted)
{
    int i, free_slot = -1;
    AcquireSRWLockExclusive(&sc_acc_lock);
    for (i = 0; i < SC_ACCEPT_SLOTS; i++) {
        if (sc_acc_listener[i] == listener) { sc_acc_ready[i] = accepted; goto done; }
        if (free_slot < 0 && sc_acc_listener[i] == 0) free_slot = i;
    }
    if (free_slot >= 0) {
        sc_acc_listener[free_slot] = listener;
        sc_acc_ready[free_slot] = accepted;
    } else {
        closesocket(accepted);   /* table full: drop it rather than leak */
    }
done:
    ReleaseSRWLockExclusive(&sc_acc_lock);
}

static SOCKET sc_acc_take(SOCKET listener)
{
    SOCKET r = INVALID_SOCKET;
    int i;
    AcquireSRWLockExclusive(&sc_acc_lock);
    for (i = 0; i < SC_ACCEPT_SLOTS; i++) {
        if (sc_acc_listener[i] == listener) {
            r = sc_acc_ready[i];
            sc_acc_ready[i] = INVALID_SOCKET;
            break;
        }
    }
    ReleaseSRWLockExclusive(&sc_acc_lock);
    return r;
}

/* AcceptEx is not in any import library under a fixed address; it must be
 * fetched per socket family through WSAIoctl. Cached after the first call. */
static LPFN_ACCEPTEX sc_acceptex;

static LPFN_ACCEPTEX sc_get_acceptex(SOCKET s)
{
    GUID guid = WSAID_ACCEPTEX;
    DWORD n = 0;
    LPFN_ACCEPTEX fn = sc_acceptex;
    if (fn != NULL)
        return fn;
    if (WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof guid,
                 &fn, sizeof fn, &n, NULL, NULL) == SOCKET_ERROR)
        return NULL;
    sc_acceptex = fn;
    return fn;
}

static int sc_is_listener(SOCKET s)
{
    int v = 0, len = (int)sizeof v;
    if (getsockopt(s, SOL_SOCKET, SO_ACCEPTCONN, (char*)&v, &len) == SOCKET_ERROR)
        return 0;
    return v != 0;
}

/* ---- queue ------------------------------------------------------------ */

int scaly_eio_create(void)
{
    HANDLE h;
    scaly_ws_start();
    h = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    return h == NULL ? -1 : (int)(intptr_t)h;
}

static int sc_associate(HANDLE port, SOCKET s)
{
    /* Re-associating an already-associated handle fails with
     * ERROR_INVALID_PARAMETER; that is the normal case on re-arm, not an
     * error. Anything else is real. */
    if (CreateIoCompletionPort((HANDLE)s, port, (ULONG_PTR)0, 0) != NULL)
        return 0;
    return GetLastError() == ERROR_INVALID_PARAMETER ? 0 : -1;
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    HANDLE port = (HANDLE)(intptr_t)q;
    SOCKET s = (SOCKET)(intptr_t)fd;
    SC_ARM* a;
    DWORD flags = 0, got = 0;
    WSABUF buf;
    int rc;

    if (sc_associate(port, s) < 0)
        return -1;

    a = (SC_ARM*)calloc(1, sizeof *a);
    if (a == NULL)
        return -1;
    a->tag = tag;
    a->s = s;
    a->accepted = INVALID_SOCKET;

    if (!for_write && sc_is_listener(s)) {
        LPFN_ACCEPTEX ax = sc_get_acceptex(s);
        DWORD recvd = 0;
        a->kind = SC_ARM_ACCEPT;
        if (ax == NULL) { free(a); return -1; }
        a->accepted = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (a->accepted == INVALID_SOCKET) { free(a); return -1; }
        /* Zero receive length: complete on connection, not on first byte —
         * that is what keeps this a READINESS report and not a read. */
        if (!ax(s, a->accepted, a->addrs, 0,
                sizeof(struct sockaddr_in6) + 16,
                sizeof(struct sockaddr_in6) + 16, &recvd, &a->ov)
            && WSAGetLastError() != WSA_IO_PENDING) {
            closesocket(a->accepted);
            free(a);
            return -1;
        }
        return 0;
    }

    buf.len = 0;
    buf.buf = NULL;
    if (for_write) {
        a->kind = SC_ARM_SEND;
        rc = WSASend(s, &buf, 1, &got, 0, &a->ov, NULL);
    } else {
        a->kind = SC_ARM_RECV;
        rc = WSARecv(s, &buf, 1, &got, &flags, &a->ov, NULL);
    }
    /* rc == 0 means it finished synchronously — the completion is STILL
     * queued to the port (we never set FILE_SKIP_COMPLETION_PORT_ON_SUCCESS),
     * so both outcomes are handled by wait() alone. */
    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        free(a);
        return -1;
    }
    return 0;
}

/* ms < 0 blocks; ms >= 0 returns 0 when the timeout elapses first. */
static int scaly_eio_wait_ms(int q, void** tags, int max, int ms)
{
    HANDLE port = (HANDLE)(intptr_t)q;
    OVERLAPPED_ENTRY ents[SCALY_EIO_MAX_EVENTS];
    ULONG n = 0;
    ULONG i;
    int out = 0;

    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    if (!GetQueuedCompletionStatusEx(port, ents, (ULONG)max, &n,
                                     ms < 0 ? INFINITE : (DWORD)ms, FALSE)) {
        return GetLastError() == WAIT_TIMEOUT ? 0 : -1;
    }
    for (i = 0; i < n; i++) {
        if (ents[i].lpOverlapped == NULL) {
            /* A wake: posted by scaly_eio_wake with the tag as the key. This
             * is the discriminator — a real completion always carries its
             * OVERLAPPED, a posted one never does. */
            tags[out++] = (void*)ents[i].lpCompletionKey;
            continue;
        }
        {
            SC_ARM* a = (SC_ARM*)ents[i].lpOverlapped;
            if (a->kind == SC_ARM_ACCEPT) {
                /* Without SO_UPDATE_ACCEPT_CONTEXT the accepted socket has no
                 * inherited state: getpeername and shutdown fail on it. */
                setsockopt(a->accepted, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                           (char*)&a->s, sizeof a->s);
                sc_acc_put(a->s, a->accepted);
            }
            tags[out++] = a->tag;
            free(a);
        }
    }
    return out;
}

int scaly_eio_wait(int q, void** tags, int max)
{
    return scaly_eio_wait_ms(q, tags, max, -1);
}

int scaly_eio_wait_timeout(int q, void** tags, int max, int ms)
{
    return scaly_eio_wait_ms(q, tags, max, ms);
}

/* ---- cross-thread wake ------------------------------------------------
 *
 * No kernel object at all: PostQueuedCompletionStatus IS the wake, and it is
 * callable from any thread by design. So wake_create only has to remember the
 * tag, and it hands back a small index rather than a pointer — the API's `int`
 * cannot carry one. Repeatable and never oneshot, matching the eventfd arm.
 */
#define SC_WAKE_SLOTS 64
static void* sc_wake_tag[SC_WAKE_SLOTS];
static SRWLOCK sc_wake_lock = SRWLOCK_INIT;

int scaly_eio_wake_create(int q, void* tag)
{
    int i;
    (void)q;
    AcquireSRWLockExclusive(&sc_wake_lock);
    for (i = 0; i < SC_WAKE_SLOTS; i++) {
        if (sc_wake_tag[i] == NULL) {
            sc_wake_tag[i] = tag;
            ReleaseSRWLockExclusive(&sc_wake_lock);
            return i + 1;            /* 0 would be indistinguishable from a fd */
        }
    }
    ReleaseSRWLockExclusive(&sc_wake_lock);
    return -1;
}

int scaly_eio_wake(int q, int w)
{
    HANDLE port = (HANDLE)(intptr_t)q;
    void* tag;
    if (w < 1 || w > SC_WAKE_SLOTS)
        return -1;
    AcquireSRWLockShared(&sc_wake_lock);
    tag = sc_wake_tag[w - 1];
    ReleaseSRWLockShared(&sc_wake_lock);
    return PostQueuedCompletionStatus(port, 0, (ULONG_PTR)tag, NULL) ? 0 : -1;
}

int scaly_eio_wake_close(int q, int w)
{
    (void)q;
    if (w < 1 || w > SC_WAKE_SLOTS)
        return -1;
    AcquireSRWLockExclusive(&sc_wake_lock);
    sc_wake_tag[w - 1] = NULL;
    ReleaseSRWLockExclusive(&sc_wake_lock);
    return 0;
}

/* ---- byte transfer ----------------------------------------------------
 *
 * recv/send, not _read/_write: a Windows SOCKET is not a CRT file descriptor
 * and the CRT calls fail on it. The fallback the other way round is real
 * though — the same API is used on pipes by the worker pool — so a
 * WSAENOTSOCK is retried through the CRT rather than reported.
 *
 * -2 for "would block" and -1 for a hard error, as the POSIX file does.
 */
static long long sc_map_rw(int r)
{
    if (r >= 0)
        return r;
    {
        int e = WSAGetLastError();
        if (e == WSAEWOULDBLOCK)
            return -2;
    }
    return -1;
}

long long scaly_eio_read(int fd, void* buf, size_t count)
{
    SOCKET s = (SOCKET)(intptr_t)fd;
    int r = recv(s, (char*)buf, (int)count, 0);
    if (r == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK)
        return _read(fd, buf, (unsigned int)count);
    return sc_map_rw(r);
}

long long scaly_eio_write(int fd, const void* buf, size_t count)
{
    SOCKET s = (SOCKET)(intptr_t)fd;
    int r = send(s, (const char*)buf, (int)count, 0);
    if (r == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK)
        return _write(fd, buf, (unsigned int)count);
    return sc_map_rw(r);
}

/* There is no SIGPIPE on Windows, so the darwin/linux split that exists in
 * eio.c (SO_NOSIGPIPE vs MSG_NOSIGNAL) has no counterpart: a send to a dead
 * peer simply returns an error. */
long long scaly_eio_tcp_write(int fd, const void* buf, size_t count)
{
    return scaly_eio_write(fd, buf, count);
}

int scaly_eio_errno(void)
{
    int e = WSAGetLastError();
    return e != 0 ? e : errno;
}

int scaly_eio_set_nonblocking(int fd)
{
    u_long on = 1;
    SOCKET s = (SOCKET)(intptr_t)fd;
    return ioctlsocket(s, FIONBIO, &on) == SOCKET_ERROR ? -1 : 0;
}

/* ---- TCP -------------------------------------------------------------- */

static int sc_listen_at(unsigned int ip_host_order, int port)
{
    struct sockaddr_in a;
    SOCKET s;
    BOOL yes = TRUE;
    scaly_ws_start();
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
        return -1;
    /* NOT SO_REUSEADDR: on Windows that permits two live sockets on one port
     * (it means what SO_REUSEPORT means elsewhere) and would silently steal a
     * port from another process. SO_EXCLUSIVEADDRUSE is the correct opposite. */
    setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&yes, sizeof yes);
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons((unsigned short)port);
    a.sin_addr.s_addr = htonl(ip_host_order);
    if (bind(s, (struct sockaddr*)&a, sizeof a) == SOCKET_ERROR
        || listen(s, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(s);
        return -1;
    }
    return (int)(intptr_t)s;
}

int scaly_eio_tcp_listen(int port)      { return sc_listen_at(INADDR_LOOPBACK, port); }
int scaly_eio_tcp_listen_any(int port)  { return sc_listen_at(INADDR_ANY, port); }

int scaly_eio_tcp_port(int fd)
{
    struct sockaddr_in a;
    int len = (int)sizeof a;
    if (getsockname((SOCKET)(intptr_t)fd, (struct sockaddr*)&a, &len) == SOCKET_ERROR)
        return -1;
    return ntohs(a.sin_port);
}

int scaly_eio_tcp_connect(int port)
{
    struct sockaddr_in a;
    SOCKET s;
    scaly_ws_start();
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
        return -1;
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons((unsigned short)port);
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(s, (struct sockaddr*)&a, sizeof a) == SOCKET_ERROR) {
        closesocket(s);
        return -1;
    }
    return (int)(intptr_t)s;
}

int scaly_eio_tcp_connect_host(const char* host, int port)
{
    struct addrinfo hints, *res = NULL, *p;
    char portstr[16];
    SOCKET s = INVALID_SOCKET;
    scaly_ws_start();
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    sprintf(portstr, "%d", port);
    if (getaddrinfo(host, portstr, &hints, &res) != 0)
        return -1;
    for (p = res; p != NULL; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == INVALID_SOCKET)
            continue;
        if (connect(s, p->ai_addr, (int)p->ai_addrlen) != SOCKET_ERROR)
            break;
        closesocket(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(res);
    return s == INVALID_SOCKET ? -1 : (int)(intptr_t)s;
}

/* Two paths, and which one applies depends on how the caller got here. After
 * an arm() the connection was already taken by AcceptEx and is waiting in the
 * table; without one this is a plain non-blocking accept. Checking the table
 * first is what makes both callers work. */
int scaly_eio_accept(int fd)
{
    SOCKET listener = (SOCKET)(intptr_t)fd;
    SOCKET r = sc_acc_take(listener);
    if (r != INVALID_SOCKET)
        return (int)(intptr_t)r;
    r = accept(listener, NULL, NULL);
    if (r == INVALID_SOCKET)
        return WSAGetLastError() == WSAEWOULDBLOCK ? -2 : -1;
    return (int)(intptr_t)r;
}

/* ---- guard page -------------------------------------------------------
 *
 * The POSIX file installs a SIGSEGV handler on an alternate stack, because an
 * overflowed fiber stack has no room for the handler. Windows has the
 * equivalent built in — the OS dispatches EXCEPTION_STACK_OVERFLOW on a
 * reserved guard region — so a vectored handler suffices and no alternate
 * stack has to be arranged. EXCEPTION_CONTINUE_SEARCH on anything the
 * classifier does not recognise reproduces the POSIX file's "reset to the
 * default action and return" exactly: the process dies as it would have.
 */
static int (*scaly_guard_classify)(void*);

static LONG CALLBACK scaly_guard_veh(EXCEPTION_POINTERS* ep)
{
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if ((code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_STACK_OVERFLOW)
        && scaly_guard_classify != NULL
        && ep->ExceptionRecord->NumberParameters >= 2) {
        void* addr = (void*)ep->ExceptionRecord->ExceptionInformation[1];
        if (scaly_guard_classify(addr))
            return EXCEPTION_CONTINUE_SEARCH;   /* classifier exits the process */
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int scaly_guard_install(int (*classify)(void*))
{
    scaly_guard_classify = classify;
    return AddVectoredExceptionHandler(1, scaly_guard_veh) == NULL ? -1 : 0;
}

/* ---- misc ------------------------------------------------------------- */

int scaly_eio_ncpu(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwNumberOfProcessors;
}

long long scaly_eio_now_ns(void)
{
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    /* Split rather than (c * 1e9) / f: the product overflows 64 bits after a
     * couple of hours at a 10 MHz frequency, and this clock is a calibration
     * source that must stay monotonic for the life of the process. */
    return (c.QuadPart / f.QuadPart) * 1000000000LL
         + ((c.QuadPart % f.QuadPart) * 1000000000LL) / f.QuadPart;
}

/* No st_blksize on Windows; the reference's SP_STAT_BLKSIZE default is what
 * the POSIX file falls back to for non-regular files anyway. */
long long scaly_eio_blksize_path(const char* path) { (void)path; return 8192; }
long long scaly_eio_blksize_fd(int fd)             { (void)fd;   return 8192; }

long long scaly_eio_tell(void* stream)
{
    return _ftelli64((FILE*)stream);
}

long long scaly_eio_seek(void* stream, long long offset, long long whence)
{
    return _fseeki64((FILE*)stream, offset, (int)whence);
}

#else
typedef int scaly_eio_win_not_needed_on_this_target;
#endif /* _WIN32 */
