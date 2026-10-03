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

/* getenv (scaly_eio_ncpu's SCALY_WORKERS) is standard C that MSVC deprecates;
 * the same define as posixcompat_windows.c and ctime.c, not the _s form, which would
 * make this target differ from the other three. Before any CRT header. */
#define _CRT_SECURE_NO_WARNINGS 1
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

/* SO_REUSEPORT has no Windows counterpart (SO_REUSEADDR there lets a second
 * socket take over a live port): one socket per port, so https.H3 serves
 * on one listener here. */
int scaly_eio_reuseport(int fd)
{
    (void)fd;
    return -1;
}

/* UDP with its peers' addresses, for https's h3 (ngtcp2): not served on
 * Windows yet -- h3 stays off there. */
long long scaly_eio_udp_recv(int fd, void* buf, size_t len, void* addr, size_t addrcap,
                             size_t* addrlen)
{
    (void)fd; (void)buf; (void)len; (void)addr; (void)addrcap; (void)addrlen;
    return -1;
}

long long scaly_eio_udp_send(int fd, const void* buf, size_t len, const void* addr, size_t addrlen)
{
    (void)fd; (void)buf; (void)len; (void)addr; (void)addrlen;
    return -1;
}

long long scaly_eio_udp_send_train(int fd, const void* buf, size_t len, size_t segment,
                                   const void* addr, size_t addrlen)
{
    (void)fd; (void)buf; (void)len; (void)segment; (void)addr; (void)addrlen;
    return -1;
}

int scaly_eio_loopback6(int port, void* addr, size_t addrcap, size_t* len)
{
    (void)port; (void)addr; (void)addrcap; (void)len;
    return -1;
}

int scaly_eio_sockname(int fd, void* addr, size_t addrcap, size_t* len)
{
    (void)fd; (void)addr; (void)addrcap; (void)len;
    return -1;
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

/* Edge-triggered watching (eio.c) has no counterpart in this readiness
 * emulation — a zero-byte WSARecv/WSASend is by nature one completion per
 * arm. -2 says "not here", and the Scaly side stays on scaly_eio_arm. */
int scaly_eio_watch(int q, int fd, void* tag)
{
    (void)q;
    (void)fd;
    (void)tag;
    return -2;
}

/* The completion path (io_uring, linux): not in this emulation either —
 * IOCP is completion-based, but the runtime above speaks readiness here. */
int scaly_eio_submit_recv(int q, int fd, void* buf, size_t count, void* tag)
{
    (void)q; (void)fd; (void)buf; (void)count; (void)tag;
    return -2;
}

int scaly_eio_submit_send(int q, int fd, void* buf, size_t count, void* tag)
{
    (void)q; (void)fd; (void)buf; (void)count; (void)tag;
    return -2;
}

int scaly_eio_completions(int q)
{
    (void)q;
    return 0;
}

/* Which backend the poller is (see eio.c): the IOCP readiness emulation. */
int scaly_eio_backend(int q)
{
    (void)q;
    return 4;
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
        /* AcceptEx takes the connection into a socket made in ADVANCE, and
         * that socket must be of the LISTENER's family — an AF_INET one
         * under an IPv6 listener fails. The address buffers below are sized
         * for sockaddr_in6 already. */
        {
            struct sockaddr_storage ls;
            int llen = (int)sizeof ls;
            int fam = AF_INET;
            if (getsockname(s, (struct sockaddr*)&ls, &llen) != SOCKET_ERROR)
                fam = ls.ss_family;
            a->accepted = socket(fam, SOCK_STREAM, IPPROTO_TCP);
        }
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

/* The nanosecond wait of eio.c: IOCP waits in whole milliseconds, so the
 * bound is rounded up (never early). */
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns)
{
    long long ms = ns <= 0 ? 0 : (ns + 999999LL) / 1000000LL;
    if (ms > 2000000000LL)
        ms = 2000000000LL;
    return scaly_eio_wait_timeout(q, tags, max, (int)ms);
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

/* ★Close the poller QUEUE. This is the half the wake close had and the queue
 * did not: `Io.close_poller` used to call POSIX `close` on it directly, which
 * is right for a kqueue/epoll fd and wrong here twice over — an IOCP HANDLE is
 * not a CRT descriptor, so the CRT either rejects the number through its
 * invalid-parameter handler (which TERMINATES the process, and that is what
 * ate the last line of a cross-thread test: the reclaim runs at thread exit,
 * after the work is done and before main's final print) or, if the number
 * happens to match a live descriptor, closes an UNRELATED file. */
int scaly_eio_close(int q)
{
    return CloseHandle((HANDLE)(intptr_t)q) ? 0 : -1;
}

/* ---- byte transfer ----------------------------------------------------
 *
 * ★★★ TWO NAMESPACES, and this is the trap that cost rung 4 a red run.
 * On POSIX a socket IS a file descriptor and read/write serve both. On
 * Windows they are unrelated kinds of value: fd 1 is a CRT descriptor,
 * a SOCKET is a kernel handle, and the API here carries either in one `int`.
 * Both really do arrive — Console.print writes to fd 1 through this very
 * function, while the TCP helpers hand back sockets — so the kind has to be
 * DECIDED, not guessed.
 *
 * The first version guessed: it called send() and fell back to _write() when
 * the error was WSAENOTSOCK. That is wrong twice over. Winsock may not be
 * started yet, in which case the error is WSANOTINITIALISED and the fallback
 * never fires — which is exactly what happened: every Console.print returned
 * -1, write_all gave up quietly, and the program exited 0 having printed
 * nothing. And even started, calling send() on a CRT descriptor whose number
 * happens to match a live socket handle would send to the wrong place.
 *
 * SO_TYPE is the decision: it succeeds for a socket and fails for anything
 * else, and it moves nothing. Winsock is started first so the answer means
 * what it says.
 *
 * -2 for "would block" and -1 for a hard error, as the POSIX file does.
 */
static int sc_is_socket(SOCKET s)
{
    int t = 0, len = (int)sizeof t;
    scaly_ws_start();
    return getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&t, &len) != SOCKET_ERROR;
}

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
    if (!sc_is_socket(s))
        return _read(fd, buf, (unsigned int)count);
    return sc_map_rw(recv(s, (char*)buf, (int)count, 0));
}

long long scaly_eio_write(int fd, const void* buf, size_t count)
{
    SOCKET s = (SOCKET)(intptr_t)fd;
    if (!sc_is_socket(s))
        return _write(fd, buf, (unsigned int)count);
    return sc_map_rw(send(s, (const char*)buf, (int)count, 0));
}

/* There is no SIGPIPE on Windows, so the darwin/linux split that exists in
 * eio.c (SO_NOSIGPIPE vs MSG_NOSIGNAL) has no counterpart: a send to a dead
 * peer simply returns an error. */
long long scaly_eio_tcp_write(int fd, const void* buf, size_t count)
{
    return scaly_eio_write(fd, buf, count);
}

/* TCP_NODELAY on or off (1/0) — the eio.c twin; the SOCKET is a kernel
 * handle and the option value a char pointer here. 0 on success, -1 on an
 * error (a descriptor that is not a socket). */
int scaly_eio_tcp_nodelay(int fd, int on)
{
    SOCKET s = (SOCKET)(intptr_t)fd;
    int v = on ? 1 : 0;
    scaly_ws_start();
    return setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&v, (int)sizeof v) == 0 ? 0 : -1;
}

/* ★A WSA ERROR AND A C errno ARE UNRELATED NUMBERINGS, and preferring the
 * former mislabelled every file diagnostic in the product. This used to answer
 * `WSAGetLastError()` when that was non-zero — but on Windows WSAGetLastError
 * IS GetLastError (they share the thread's last-error slot), so after a failed
 * `_open` it holds a WIN32 code, not an errno: ERROR_PATH_NOT_FOUND is 3, and
 * every caller feeds this straight to `strerror`, which reads 3 as ESRCH.
 * Measured on the corpus: `cannot open output file "no_such_dir/out.txt"
 * (No such process)` where POSIX says `(No such file or directory)`.
 *
 * Every consumer in this tree is a FILE error going to `strerror` — the whole
 * list is opensp's Storage/Parser/EntityCatalog/onsgmls and dazzle's four FOT
 * builders plus the CLI — so the C errno is what this accessor owes them, and
 * eio.c answers exactly that on POSIX. Same shape as `close` and `creat`: the
 * Windows name resolves and its ANSWER belongs to a different namespace.
 *
 * If a socket path ever needs the Winsock code, it must ask for it separately
 * and MAP it — WSAECONNRESET is 10054 and there is no errno that means it. */
int scaly_eio_errno(void)
{
    return errno;
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
    /* ★NON-BLOCKING, like the POSIX file's listen_at — and it is the whole
     * accept path, not a detail. Io.accept calls scaly_eio_accept FIRST and
     * only parks when it answers -2 ("nothing pending"); a blocking listener
     * never answers that, so the plain accept() below sits in the kernel and
     * takes the ENTIRE OS THREAD with it. In a cooperative scheduler that is
     * fatal: the client task that would create the connection can no longer
     * be scheduled, so the wait is for something that can never arrive. The
     * symptom is a test that prints nothing and hits the harness timeout. */
    if (bind(s, (struct sockaddr*)&a, sizeof a) == SOCKET_ERROR
        || listen(s, SOMAXCONN) == SOCKET_ERROR
        || scaly_eio_set_nonblocking((int)(intptr_t)s) < 0) {
        closesocket(s);
        return -1;
    }
    return (int)(intptr_t)s;
}

int scaly_eio_tcp_listen(int port)      { return sc_listen_at(INADDR_LOOPBACK, port); }

/* Is this the unspecified IPv6 address "::"? Byte by byte, as in eio.c. */
static int sc_in6_any(const struct sockaddr* sa)
{
    const unsigned char* b = (const unsigned char*)&((const struct sockaddr_in6*)sa)->sin6_addr;
    int i;
    for (i = 0; i < 16; i++)
        if (b[i] != 0)
            return 0;
    return 1;
}

/* The eio.c twin: listen on a NUMERIC host of either family; "::" is
 * dual-stack. IPV6_V6ONLY defaults to ON here (unlike Linux), so switching
 * it off explicitly is what makes "::" take IPv4 connections at all. */
int scaly_eio_tcp_listen_host(const char* host, int port)
{
    struct addrinfo hints, *res = NULL, *p;
    char portstr[16];
    SOCKET s = INVALID_SOCKET;
    BOOL yes = TRUE;
    scaly_ws_start();
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
    snprintf(portstr, sizeof portstr, "%d", port);
    if (getaddrinfo(host, portstr, &hints, &res) != 0)
        return -1;
    for (p = res; p != NULL; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == INVALID_SOCKET)
            continue;
        setsockopt(s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&yes, sizeof yes);
        if (p->ai_family == AF_INET6) {
            DWORD v6only = sc_in6_any(p->ai_addr) ? 0 : 1;
            setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&v6only, sizeof v6only);
        }
        if (bind(s, p->ai_addr, (int)p->ai_addrlen) != SOCKET_ERROR
            && listen(s, SOMAXCONN) != SOCKET_ERROR
            && scaly_eio_set_nonblocking((int)(intptr_t)s) == 0)
            break;
        closesocket(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(res);
    return s == INVALID_SOCKET ? -1 : (int)(intptr_t)s;
}

/* Every interface of both families (the dual-stack "::"), IPv4's
 * INADDR_ANY where the host has no IPv6 — as eio.c. */
int scaly_eio_tcp_listen_any(int port)
{
    int fd = scaly_eio_tcp_listen_host("::", port);
    if (fd >= 0)
        return fd;
    return sc_listen_at(INADDR_ANY, port);
}

int scaly_eio_tcp_port(int fd)
{
    struct sockaddr_storage a;
    int len = (int)sizeof a;
    if (getsockname((SOCKET)(intptr_t)fd, (struct sockaddr*)&a, &len) == SOCKET_ERROR)
        return -1;
    if (a.ss_family == AF_INET6)
        return ntohs(((struct sockaddr_in6*)&a)->sin6_port);
    return ntohs(((struct sockaddr_in*)&a)->sin_port);
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
    /* snprintf, not sprintf: bounded, and it sidesteps the same MSVC
     * deprecation that ctime.c has to silence with a macro (there the
     * functions are the reference's own and cannot be swapped). */
    snprintf(portstr, sizeof portstr, "%d", port);
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
 *
 * ★THE CLASSIFIER DOES NOT EXIT — the handler does. `fiber_guard_hit` is a
 * pure predicate (it reads two globals and answers 1/0, so that it can be
 * async-signal-safe on the POSIX side); the reporting half belongs to the
 * shim, where `scaly_guard_handler` writes the message and `_exit(108)`s.
 * The first version of this function returned CONTINUE_SEARCH in BOTH arms
 * with a comment claiming the classifier exits, so the answer was computed
 * and thrown away and a fiber overflow died anonymously. When a port splits
 * a decision from its consequence, both halves have to cross.
 *
 * WriteFile + TerminateProcess rather than fprintf + exit, for the same
 * reason the POSIX file uses write(2) + _exit(2): this runs on the stack that
 * just overflowed, so anything that buffers, locks or runs atexit handlers is
 * a second fault waiting to happen.
 */
#ifndef STATUS_GUARD_PAGE_VIOLATION
#define STATUS_GUARD_PAGE_VIOLATION ((DWORD)0x80000001L)
#endif

/* The guard region itself, and the mechanism is the whole point. POSIX makes
 * it PROT_NONE; doing the same here (PAGE_NOACCESS) would be a faithful
 * translation of the SPELLING and a broken translation of the CONTRACT: when
 * the overflow hits, the kernel builds the exception record on the very stack
 * that just ran out, the push lands in the inaccessible page, and the process
 * dies with STATUS_STACK_OVERFLOW before ntdll ever calls a handler — no
 * message, no exit 108, nothing to read.
 *
 * PAGE_GUARD is what Windows uses for its own thread stacks: the first touch
 * raises STATUS_GUARD_PAGE_VIOLATION and CLEARS the attribute, so the page is
 * ordinary memory by the time the dispatcher needs room. One fault, then
 * space to report it in.
 *
 * ★And "space" is 4096 bytes MINUS what the dispatch spends, i.e. a few hundred
 * — measured, not assumed; whether it is enough depends on where in the page the
 * faulting access landed. The consequences of that number are at
 * scaly_guard_die and in the handler.
 *
 * ★★A second correction from the same measurement, and it is load-bearing for
 * the handler below: on a FIBER stack the exception does not arrive as
 * STATUS_GUARD_PAGE_VIOLATION at all but as **STATUS_STACK_OVERFLOW**, because
 * scaly_make_context sets the TIB's DeallocationStack equal to its StackLimit —
 * there is nothing below the mapping to grow into, so the kernel reports an
 * exhausted stack rather than a guard page it could move. The handler's
 * three-code test looks redundant and is not: EXCEPTION_STACK_OVERFLOW is the
 * arm that actually fires. Do not prune it to the "obvious" one. */
/* ★★★DO NOT MOVE THIS GUARD. The room the handler needs is BELOW it, and it is
 * already there — bought by the mmap shim in win32/posixcompat_windows.c, which reserves
 * a band under every mapping it returns (sized from this machine's CONTEXT record
 * since 2026-09-27). Read that note with this one.
 *
 * ★Why the band is not arranged here, where it would seem to belong: this file
 * owns the QUESTION "where is this stack's overflow guard", so it can place the
 * guard one page up, keep the lowest page as the band, and translate fault
 * addresses back into the classifier's coordinates on the way out. That was
 * built (2026-08-10). It works — guard_probe's sweep goes to 64/64 — and it
 * BREAKS FIFTEEN fiber tests, because `fiber.scaly` reserves the two words
 * immediately above the guard page for the stack pool's free list ("the entry is
 * two size_t slots [next, size] at stack_base + guard page"). Moving the guard
 * onto that node turns every fiber DISPOSAL into a guard-page write.
 *
 * ★So the guard's position is an implicit contract between two files, and the
 * lesson generalises past this page: **a shim that owns a QUESTION does not
 * thereby own the ADDRESS SPACE around its answer.** Buying the band in the
 * allocator instead leaves every address the Scaly side computes exactly where
 * it was — and costs the other three targets nothing at all, where moving the
 * guard would have cost them an emission change and a page of every stack. */
int scaly_stack_guard(void* base, size_t len)
{
    DWORD old = 0;
    return VirtualProtect(base, len, PAGE_READWRITE | PAGE_GUARD, &old) ? 0 : -1;
}

static int (*scaly_guard_classify)(void*);

/* ★THE TERMINATOR HAS TO BE A SYSCALL STUB, AND THAT IS THE WHOLE FIX (measured
 * on a Windows box 2026-08-10; the reproduction and its numbers are in
 * tests/win32/WINDOWS-BOX.md section 5).
 *
 * The handler is entered with only a few HUNDRED bytes of stack: the guard page
 * is 4096 bytes, the exception dispatch spends ~2600 of them on the CONTEXT
 * record and ntdll's own frames, and what is left is everything the handler and
 * everything it calls may use before it runs off the bottom of the mapping into
 * unmapped memory. Measured across a sweep of recursion frame sizes: 240 to
 * 1520 bytes.
 *
 * WriteFile fits in that (it is a thin path down to NtWriteFile). kernel32's
 * TerminateProcess does NOT: it faults, the access violation becomes the
 * process's exit status, and the whole failure looked like "nothing after the
 * first WriteFile runs" — which was true, and whose cause was misattributed to
 * WriteFile for three rounds. NtTerminateProcess, ntdll's syscall stub for the
 * same operation, fits with room to spare and ends the process with the code we
 * asked for.
 *
 * Resolved once at install time, never in the handler: a fault is no place to
 * be walking export tables. GetProcAddress rather than a link against
 * ntdll.lib, so no target's link line changes. If it ever fails, the fallback
 * is today's behaviour — a correct message and the wrong exit code — which is
 * the same best-effort posture as a failed handler install. */
typedef LONG(NTAPI* scaly_nt_terminate_t)(HANDLE, LONG);
static scaly_nt_terminate_t scaly_nt_terminate;

static void scaly_guard_die(void)
{
    if (scaly_nt_terminate != NULL)
        scaly_nt_terminate((HANDLE)(LONG_PTR)-1, 108); /* NtCurrentProcess */
    else
        TerminateProcess(GetCurrentProcess(), 108);
    /* Unreachable in practice, and it must not RESUME: a guard-page violation
     * inside the range the TIB declares as this thread's stack is a resumable
     * event, and resuming re-enters the recursion that overflowed. */
    Sleep(INFINITE);
}

static LONG CALLBACK scaly_guard_veh(EXCEPTION_POINTERS* ep)
{
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if ((code == STATUS_GUARD_PAGE_VIOLATION
         || code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_STACK_OVERFLOW)
        && scaly_guard_classify != NULL
        && ep->ExceptionRecord->NumberParameters >= 2) {
        void* addr = (void*)ep->ExceptionRecord->ExceptionInformation[1];
        if (scaly_guard_classify(addr)) {
            static const char msg[] = "fiber stack overflow (guard page hit)\n";
            DWORD written = 0;
            WriteFile(GetStdHandle(STD_ERROR_HANDLE), msg,
                      (DWORD)(sizeof msg - 1), &written, NULL);
            /* ★The message was never the hard part — it has come out on the
             * first try every time. Ending the process with 108 was, and the
             * reason is stack, not the choice of API: see scaly_guard_die.
             * Everything the handler calls has a few hundred bytes to work in.
             *
             * ★★★WHAT IS STILL NOT SOLVED, because it is one level below this
             * function: if the recursion's first touch inside the guard page
             * lands lower than the ~2600 bytes the exception dispatch needs,
             * there is no room to build the CONTEXT record and the process dies
             * with NO handler at all — no message, and the raw fault as the
             * exit code. Measured over 64 recursion frame sizes: 20 of them.
             * The POSIX side is immune because sigaltstack puts the signal
             * frame on a different stack, and a VEH cannot switch stacks. What
             * closes it is one WRITABLE page below the guard page for the
             * dispatch to spill into (64 of 64 with it, measured the same way),
             * and that is a stack-LAYOUT change in fiber.scaly, not a shim
             * change. tests/win32/WINDOWS-BOX.md section 5 has the numbers and
             * the shape. */
            scaly_guard_die();
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int scaly_guard_install(int (*classify)(void*))
{
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    scaly_guard_classify = classify;
    if (ntdll != NULL)
        /* Through void*, not directly: a function-pointer-to-function-pointer
         * cast is what -Wcast-function-type objects to. */
        scaly_nt_terminate = (scaly_nt_terminate_t)(void*)
            GetProcAddress(ntdll, "NtTerminateProcess");
    return AddVectoredExceptionHandler(1, scaly_guard_veh) == NULL ? -1 : 0;
}

/* ---- misc ------------------------------------------------------------- */

int scaly_eio_ncpu(void)
{
    /* SCALY_WORKERS=<n> caps the default pool, as in eio.c */
    const char *w = getenv("SCALY_WORKERS");
    if (w != NULL && *w != '\0') {
        long k = strtol(w, NULL, 10);
        if (k >= 1)
            return (int)k;
    }
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwNumberOfProcessors;
}

/* Sleep for us microseconds — the fork-join ticker's pace, as in eio.c.
 * Sleep() rounds to the system tick (1-15.6 ms), so a high-resolution
 * waitable timer (Windows 10 1803+) carries it, one per thread; Sleep(1)
 * stays the fallback where the flag is refused. */
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
void scaly_eio_sleep_us(unsigned int us)
{
    static __declspec(thread) HANDLE timer = NULL;
    if (timer == NULL)
        timer = CreateWaitableTimerExW(NULL, NULL, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (timer == NULL) {
        Sleep(1);
        return;
    }
    LARGE_INTEGER due;
    due.QuadPart = -(LONGLONG)us * 10;   /* relative, 100 ns units */
    if (!SetWaitableTimer(timer, &due, 0, NULL, NULL, FALSE)) {
        Sleep(1);
        return;
    }
    WaitForSingleObject(timer, INFINITE);
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

/* scaly_eio_stat (see eio.c): size, mtime in nanoseconds, inode, block
 * size. There is no inode (0) and no st_blksize -- the reference's
 * SP_STAT_BLKSIZE default is what the POSIX file falls back to for
 * non-regular files anyway.
 *
 * ★A PATH is asked through GetFileAttributesEx, not the CRT's _stat64: the
 * CRT hands the mtime out in WHOLE SECONDS, and with no inode either a file
 * replaced by one of the same size within the second looked unchanged -- the
 * http static cache served the old content (tests/http/static_reload, found
 * by the win-tool balloon, 2026-10-02). FILETIME counts 100 ns since 1601;
 * taken to the Unix epoch BEFORE the scaling, because 425 years of
 * nanoseconds do not fit 63 bits. A descriptor keeps _fstat64: it may be a
 * pipe or the console, which have no file time. */
int scaly_eio_stat(const char* path, int fd, long long* out)
{
    if (path != 0) {
        WIN32_FILE_ATTRIBUTE_DATA d;
        long long t;
        if (!GetFileAttributesExA(path, GetFileExInfoStandard, &d))
            return -1;
        t = ((long long)d.ftLastWriteTime.dwHighDateTime << 32) | (long long)d.ftLastWriteTime.dwLowDateTime;
        out[0] = ((long long)d.nFileSizeHigh << 32) | (long long)d.nFileSizeLow;
        out[1] = (t - 116444736000000000LL) * 100LL;
        out[2] = 0;
        out[3] = 8192;
        return 0;
    } else {
        struct _stat64 sb;
        if (_fstat64(fd, &sb) < 0)
            return -1;
        out[0] = (long long)sb.st_size;
        out[1] = (long long)sb.st_mtime * 1000000000LL;
        out[2] = 0;
        out[3] = 8192;
        return 0;
    }
}

/* scaly_eio_wall_ns (see eio.c): the system time as a FILETIME, taken to the
 * Unix epoch as scaly_eio_stat takes a file's -- the same clock the file
 * system stamps a write with. */
long long scaly_eio_wall_ns(void)
{
    FILETIME ft;
    long long t;
    GetSystemTimeAsFileTime(&ft);
    t = ((long long)ft.dwHighDateTime << 32) | (long long)ft.dwLowDateTime;
    return (t - 116444736000000000LL) * 100LL;
}

long long scaly_eio_tell(void* stream)
{
    return _ftelli64((FILE*)stream);
}

long long scaly_eio_seek(void* stream, long long offset, long long whence)
{
    return _fseeki64((FILE*)stream, offset, (int)whence);
}

/* ---- OS facts the runtime cannot ask for directly ----------------------
 *
 * The Windows halves of the pair eio.c documents at the same place.
 *
 * _aligned_malloc takes its arguments in the OPPOSITE order to aligned_alloc
 * (size first, then alignment) — a silent swap here would ask for a 4096-byte
 * alignment of 16 bytes on one call and the reverse on another, and both
 * succeed often enough to look fine.
 *
 * GetCurrentThreadStackLimits answers for the CALLING THREAD, which is the
 * better fit for what the caller wants than POSIX's process-wide limit — but
 * it means the answer legitimately differs between threads, so it must not be
 * cached across them. The caller derives its budget once per compilation on
 * the thread that plans, which holds.
 */

long long scaly_stack_limit(void)
{
    ULONG_PTR low = 0, high = 0;
    GetCurrentThreadStackLimits(&low, &high);
    if (high <= low)
        return 0;
    return (long long)(high - low);
}

/* The sized thread spawn, the Windows half of eio.c's (see the account
 * there). CreateThread's size argument is the initial COMMIT unless
 * STACK_SIZE_PARAM_IS_A_RESERVATION says otherwise -- without the flag a
 * 64 MB request commits 64 MB up front and reserves no more than the PE
 * header's default, which is the opposite of what a deep recursion needs.
 * The entry-signature cast is the one posixcompat_windows.c's pthread_create
 * documents: safe because nobody reads the thread's result. */
int scaly_thread_spawn_sized(size_t* thread, void* start, void* arg, size_t stack_size)
{
    HANDLE h;
    DWORD flags = 0;
    if (stack_size > 0)
        flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
    h = CreateThread(NULL, stack_size, (LPTHREAD_START_ROUTINE)start, arg, flags, NULL);
    if (h == NULL)
        return -1;
    *thread = (size_t)h;
    return 0;
}

/* The fault injector, same contract as eio.c's (see the account there): it
 * has to exist on BOTH sides or the out-of-memory gates would be a
 * POSIX-only measurement, and win-undef.sh would report the symbol missing. */
static long long scaly_alloc_fail_left = -1;

void scaly_alloc_fail_after(long long n)
{
    scaly_alloc_fail_left = n;
}

/* ---- Large blocks are kept, not handed back --------------------------------
 *
 * Rule (a): this is about what THIS system's allocator does. The CRT passes a
 * large block straight to the OS and returns it on free, so every new one is
 * made of fresh pages the kernel zeroes at first touch. A program that builds
 * and drops a region per unit of work pays that for all of its memory, every
 * time: measured 2026-10-02 on the arm64 VM, one tscaly_dump over 400 small
 * units asked for 17 GB in blocks of 64 KB to 16 MB (peak working set 54 MB)
 * and spent 6.7 s in the kernel beside 4.7 s of its own, 3.3 million page
 * faults; a 256 KB heap bucket alone cost 0.6 ms per round trip. POSIX
 * allocators keep such blocks; here the shim does.
 *
 * A freed block of SC_BIG_MIN bytes or more goes into a cache instead of to
 * the CRT, and the next request of the same rounded size and alignment takes
 * it. scaly_aligned_free is not told a size, so the live big blocks stand in
 * a table by address; a pointer that is not in it is a small block and goes
 * to the CRT as before. Sizes are rounded up to a quarter of their power of
 * two so that requests which differ by a page meet in one class.
 *
 * The cache holds at most SC_CACHE_CAP bytes; past that the blocks that have
 * waited longest are really freed, and when the CRT cannot serve a request the cache is emptied and the
 * request tried once more -- cached memory must never be the reason an
 * allocation fails. A reused block holds what its last owner left, as on
 * every other system. */
#define SC_BIG_MIN   ((size_t)64 * 1024)
#define SC_CACHE_CAP ((size_t)256 * 1024 * 1024)
#define SC_TOMB      ((void*)(size_t)1)

typedef struct sc_big { void* p; size_t size; size_t align; } sc_big;

static SRWLOCK sc_big_lock = SRWLOCK_INIT;
static sc_big* sc_live;                 /* open addressing by address */
static size_t  sc_live_cap, sc_live_used, sc_live_tombs;
static sc_big* sc_cache;                /* freed blocks, last in first out */
static size_t  sc_cache_n, sc_cache_cap, sc_cache_bytes;

static size_t sc_big_round(size_t size)
{
    size_t top = SC_BIG_MIN, step;
    while (top <= size / 2)
        top *= 2;
    step = top / 4;
    return (size + step - 1) / step * step;
}

static size_t sc_live_slot(size_t cap, void* p)
{
    size_t h = ((size_t)p >> 12) * (size_t)0x9E3779B97F4A7C15ull;
    return (h >> 20) & (cap - 1);
}

/* Under the lock. Returns 0 when the table could not grow. */
static int sc_live_insert(void* p, size_t size, size_t align)
{
    size_t i;
    if ((sc_live_used + sc_live_tombs + 1) * 10 > sc_live_cap * 7) {
        size_t ncap = sc_live_cap ? sc_live_cap : 1024, k;
        sc_big* nt;
        if ((sc_live_used + 1) * 10 > ncap * 5)
            ncap *= 2;
        nt = (sc_big*)calloc(ncap, sizeof(sc_big));
        if (nt == NULL)
            return 0;
        for (k = 0; k < sc_live_cap; k++) {
            if (sc_live[k].p != NULL && sc_live[k].p != SC_TOMB) {
                size_t j = sc_live_slot(ncap, sc_live[k].p);
                while (nt[j].p != NULL)
                    j = (j + 1) & (ncap - 1);
                nt[j] = sc_live[k];
            }
        }
        free(sc_live);
        sc_live = nt;
        sc_live_cap = ncap;
        sc_live_tombs = 0;
    }
    i = sc_live_slot(sc_live_cap, p);
    while (sc_live[i].p != NULL && sc_live[i].p != SC_TOMB)
        i = (i + 1) & (sc_live_cap - 1);
    if (sc_live[i].p == SC_TOMB)
        sc_live_tombs--;
    sc_live[i].p = p;
    sc_live[i].size = size;
    sc_live[i].align = align;
    sc_live_used++;
    return 1;
}

/* Under the lock. Takes p out of the live table; 0 when it is not there. */
static int sc_live_remove(void* p, sc_big* out)
{
    size_t i;
    if (sc_live_cap == 0)
        return 0;
    i = sc_live_slot(sc_live_cap, p);
    while (sc_live[i].p != NULL) {
        if (sc_live[i].p == p) {
            *out = sc_live[i];
            sc_live[i].p = SC_TOMB;
            sc_live_used--;
            sc_live_tombs++;
            return 1;
        }
        i = (i + 1) & (sc_live_cap - 1);
    }
    return 0;
}

/* Hand every cached block back to the CRT. Called without the lock. */
static void sc_cache_flush(void)
{
    for (;;) {
        void* p = NULL;
        AcquireSRWLockExclusive(&sc_big_lock);
        if (sc_cache_n > 0) {
            sc_cache_n--;
            p = sc_cache[sc_cache_n].p;
            sc_cache_bytes -= sc_cache[sc_cache_n].size;
        }
        ReleaseSRWLockExclusive(&sc_big_lock);
        if (p == NULL)
            return;
        _aligned_free(p);
    }
}

static void* sc_big_alloc(size_t alignment, size_t size)
{
    size_t rounded = sc_big_round(size), k;
    void* p = NULL;
    int tracked;

    AcquireSRWLockExclusive(&sc_big_lock);
    for (k = sc_cache_n; k > 0; k--) {
        if (sc_cache[k - 1].size == rounded && sc_cache[k - 1].align == alignment) {
            p = sc_cache[k - 1].p;
            /* closing the gap keeps the cache in the order it was filled,
             * which is what the eviction in sc_big_free goes by */
            memmove(&sc_cache[k - 1], &sc_cache[k], (sc_cache_n - k) * sizeof(sc_big));
            sc_cache_n--;
            sc_cache_bytes -= rounded;
            break;
        }
    }
    if (p == NULL) {
        ReleaseSRWLockExclusive(&sc_big_lock);
        p = _aligned_malloc(rounded, alignment);
        if (p == NULL) {
            sc_cache_flush();
            p = _aligned_malloc(rounded, alignment);
            if (p == NULL)
                return NULL;
        }
        AcquireSRWLockExclusive(&sc_big_lock);
    }
    /* A block the table has no room for (it could not grow) stays untracked:
     * its free finds nothing here and goes to the CRT, which is right. */
    tracked = sc_live_insert(p, rounded, alignment);
    ReleaseSRWLockExclusive(&sc_big_lock);
    (void)tracked;
    return p;
}

/* 1 when p was a live big block and has been dealt with. */
static int sc_big_free(void* p)
{
    sc_big b;
    int keep = 0;

    AcquireSRWLockExclusive(&sc_big_lock);
    if (!sc_live_remove(p, &b)) {
        ReleaseSRWLockExclusive(&sc_big_lock);
        return 0;
    }
    /* Make room by handing back the blocks that have waited LONGEST. Refusing
     * the newcomer instead filled the cache with sizes nobody asked for again
     * and then turned every later free away: over 800 corpus cases a full
     * cache reused nothing more whether it held 256 MB or 1 GB (measured
     * 2026-10-02, 8.3 and 8.2 s of kernel time). The lock is dropped around
     * the CRT call, so the condition is asked again each time round. */
    while (b.size <= SC_CACHE_CAP && sc_cache_n > 0 && sc_cache_bytes + b.size > SC_CACHE_CAP) {
        void* oldest = sc_cache[0].p;
        sc_cache_bytes -= sc_cache[0].size;
        memmove(&sc_cache[0], &sc_cache[1], (sc_cache_n - 1) * sizeof(sc_big));
        sc_cache_n--;
        ReleaseSRWLockExclusive(&sc_big_lock);
        _aligned_free(oldest);
        AcquireSRWLockExclusive(&sc_big_lock);
    }
    if (sc_cache_bytes + b.size <= SC_CACHE_CAP) {
        if (sc_cache_n == sc_cache_cap) {
            size_t ncap = sc_cache_cap ? sc_cache_cap * 2 : 256;
            sc_big* nc = (sc_big*)realloc(sc_cache, ncap * sizeof(sc_big));
            if (nc != NULL) {
                sc_cache = nc;
                sc_cache_cap = ncap;
            }
        }
        if (sc_cache_n < sc_cache_cap) {
            sc_cache[sc_cache_n++] = b;
            sc_cache_bytes += b.size;
            keep = 1;
        }
    }
    ReleaseSRWLockExclusive(&sc_big_lock);
    if (!keep)
        _aligned_free(p);
    return 1;
}

void* scaly_aligned_alloc(size_t alignment, size_t size)
{
    if (scaly_alloc_fail_left >= 0) {
        if (scaly_alloc_fail_left == 0) {
            scaly_alloc_fail_left = -1;      /* one shot */
            return 0;
        }
        scaly_alloc_fail_left--;
    }
    if (size >= SC_BIG_MIN)
        return sc_big_alloc(alignment, size);
    return _aligned_malloc(size, alignment);   /* note the argument order */
}

void scaly_aligned_free(void* p)
{
    if (p != NULL && sc_big_free(p))
        return;
    _aligned_free(p);
}

/* ---- DIRECTORY ENTRIES ----------------------------------------------------
 *
 * The Windows half of the directory walk eio.c documents at the same place:
 * FindFirstFileA/FindNextFileA over "<path>\\*". The first entry arrives with
 * the handle, so it is held back until the first next.
 */
typedef struct {
    HANDLE h;
    WIN32_FIND_DATAA fd;
    int pending;
} ScalyDir;

void* scaly_eio_dir_open(const char* path)
{
    size_t n = strlen(path);
    char* pattern = (char*)malloc(n + 3);
    ScalyDir* d;
    if (pattern == NULL)
        return NULL;
    memcpy(pattern, path, n);
    if (n > 0 && path[n - 1] != '/' && path[n - 1] != '\\')
        pattern[n++] = '\\';
    pattern[n++] = '*';
    pattern[n] = 0;
    d = (ScalyDir*)malloc(sizeof(ScalyDir));
    if (d == NULL) {
        free(pattern);
        return NULL;
    }
    d->h = FindFirstFileA(pattern, &d->fd);
    free(pattern);
    if (d->h == INVALID_HANDLE_VALUE) {
        free(d);
        return NULL;
    }
    d->pending = 1;
    return d;
}

long long scaly_eio_dir_next(void* dir, char* buf, size_t cap, int* is_dir)
{
    ScalyDir* d = (ScalyDir*)dir;
    for (;;) {
        const char* name;
        size_t n;
        if (d->pending)
            d->pending = 0;
        else if (!FindNextFileA(d->h, &d->fd))
            return -1;
        name = d->fd.cFileName;
        n = strlen(name);
        if (name[0] == '.' && (n == 1 || (n == 2 && name[1] == '.')))
            continue;
        if (n + 1 > cap)
            continue;
        memcpy(buf, name, n + 1);
        *is_dir = (d->fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 ? 1 : 0;
        return (long long)n;
    }
}

int scaly_eio_dir_close(void* dir)
{
    ScalyDir* d = (ScalyDir*)dir;
    int rc = FindClose(d->h) ? 0 : -1;
    free(d);
    return rc;
}

/* ---- LOADING A C LIBRARY INTO THE PROCESS --------------------------------
 *
 * The Windows half of eio.c's scaly_eio_load_library: a DLL stays loaded and
 * the JIT's process lookup finds its exports.
 */
int scaly_eio_load_library(const char* path)
{
    return LoadLibraryA(path) != NULL ? 0 : -1;
}

/* ---- A DOUBLE AS TEXT (the Windows twin of eio.c's), shim category (b) -----------------------------------
 *
 * snprintf is variadic, and a fixed-prototype extern drops variadic arguments
 * on arm64. The REPL shows a floating-point value the way a reader expects
 * it: the SHORTEST %g form that reads back to the same double (0.1, not
 * 0.10000000000000001). Answers the length written, at most cap - 1.
 */
int scaly_eio_format_double(char* buf, size_t cap, double value)
{
    int precision;
    int n = 0;
    for (precision = 1; precision <= 17; precision++) {
        n = snprintf(buf, cap, "%.*g", precision, value);
        if (n < 0 || (size_t)n >= cap)
            return 0;
        if (strtod(buf, NULL) == value)
            break;
    }
    return n;
}

/* ---- THE TERMINAL'S RAW MODE ---------------------------------------------
 *
 * The Windows half of eio.c's raw mode: the console reads key by key, without
 * echo, and arrows arrive as the same VT sequences a POSIX terminal sends
 * (ENABLE_VIRTUAL_TERMINAL_INPUT); the output side understands them too.
 */
static DWORD scaly_term_saved_in;
static DWORD scaly_term_saved_out;
static int scaly_term_raw_on = 0;

int scaly_eio_term_isatty(int fd)
{
    return _isatty(fd) ? 1 : 0;
}

int scaly_eio_term_raw(int fd)
{
    HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    (void)fd;
    if (!GetConsoleMode(in, &scaly_term_saved_in) || !GetConsoleMode(out, &scaly_term_saved_out))
        return -1;
    mode = scaly_term_saved_in;
    mode &= ~(DWORD)(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    if (!SetConsoleMode(in, mode))
        return -1;
    SetConsoleMode(out, scaly_term_saved_out | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    scaly_term_raw_on = 1;
    return 0;
}

int scaly_eio_term_restore(int fd)
{
    (void)fd;
    if (!scaly_term_raw_on)
        return 0;
    scaly_term_raw_on = 0;
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), scaly_term_saved_in);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), scaly_term_saved_out);
    return 0;
}

/* ---- A CHILD THAT RUNS THIS PROGRAM AGAIN ---------------------------------
 *
 * The Windows half of the block eio.c documents at the same place:
 * CreateProcess on this executable, the mode word and the child's two pipe
 * HANDLEs (hexadecimal) on its command line.
 *
 * ★The child inherits EXACTLY four handles (PROC_THREAD_ATTRIBUTE_HANDLE_LIST),
 * not every inheritable one: a child that held the parent's own stdout — the
 * editor's pipe — would keep it open after the parent is gone, and the client
 * would wait for an end of file that never comes.
 * ★CREATE_NO_WINDOW: an editor starts the server without a console, and a
 * console program started from there would otherwise open one of its own.
 * ★The "pid" is the process HANDLE: it is what TerminateProcess and the wait
 * take, and it stays valid until scaly_proc_reap closes it.
 */
int scaly_proc_spawn_self(const char* mode, long long* out_pid, int* out_write_fd, int* out_read_fd)
{
    enum { SELF_CAP = 2048, CMD_CAP = SELF_CAP + 128 };
    wchar_t* self = NULL;
    wchar_t* cmd = NULL;
    SECURITY_ATTRIBUTES sa;
    HANDLE c_in_r = NULL, p_in_w = NULL, p_out_r = NULL, c_out_w = NULL;
    HANDLE nul = INVALID_HANDLE_VALUE, err = NULL;
    HANDLE list[4];
    DWORD nlist = 0;
    SIZE_T attr_size = 0;
    LPPROC_THREAD_ATTRIBUTE_LIST attrs = NULL;
    STARTUPINFOEXW si;
    PROCESS_INFORMATION pi;
    DWORD n;
    int wfd, rfd;
    int ok = 0;

    sa.nLength = sizeof sa;
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    self = (wchar_t*)malloc(SELF_CAP * sizeof(wchar_t));
    cmd = (wchar_t*)malloc(CMD_CAP * sizeof(wchar_t));
    if (self == NULL || cmd == NULL)
        goto done;
    n = GetModuleFileNameW(NULL, self, SELF_CAP);
    if (n == 0 || n >= SELF_CAP)
        goto done;

    if (!CreatePipe(&c_in_r, &p_in_w, &sa, 65536))
        goto done;
    if (!CreatePipe(&p_out_r, &c_out_w, &sa, 65536))
        goto done;
    SetHandleInformation(p_in_w, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(p_out_r, HANDLE_FLAG_INHERIT, 0);
    nul = CreateFileW(L"NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                      &sa, OPEN_EXISTING, 0, NULL);
    if (nul == INVALID_HANDLE_VALUE)
        goto done;
    /* stderr as an inheritable duplicate; a process without one gets NUL */
    if (!DuplicateHandle(GetCurrentProcess(), GetStdHandle(STD_ERROR_HANDLE), GetCurrentProcess(),
                         &err, 0, TRUE, DUPLICATE_SAME_ACCESS))
        err = NULL;

    list[nlist++] = c_in_r;
    list[nlist++] = c_out_w;
    list[nlist++] = nul;
    if (err != NULL)
        list[nlist++] = err;
    InitializeProcThreadAttributeList(NULL, 1, 0, &attr_size);
    attrs = (LPPROC_THREAD_ATTRIBUTE_LIST)malloc(attr_size);
    if (attrs == NULL || !InitializeProcThreadAttributeList(attrs, 1, 0, &attr_size)) {
        free(attrs);
        attrs = NULL;
        goto done;
    }
    if (!UpdateProcThreadAttribute(attrs, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, list,
                                   nlist * sizeof(HANDLE), NULL, NULL))
        goto done;

    _snwprintf(cmd, CMD_CAP, L"\"%ls\" %hs %llx %llx", self, mode,
               (unsigned long long)(uintptr_t)c_in_r, (unsigned long long)(uintptr_t)c_out_w);
    cmd[CMD_CAP - 1] = 0;

    memset(&si, 0, sizeof si);
    si.StartupInfo.cb = sizeof si;
    si.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
    si.StartupInfo.hStdInput = nul;
    si.StartupInfo.hStdOutput = nul;
    si.StartupInfo.hStdError = err != NULL ? err : nul;
    si.lpAttributeList = attrs;
    if (!CreateProcessW(self, cmd, NULL, NULL, TRUE, EXTENDED_STARTUPINFO_PRESENT | CREATE_NO_WINDOW,
                        NULL, NULL, &si.StartupInfo, &pi))
        goto done;
    CloseHandle(pi.hThread);

    /* the descriptors own the parent's ends from here on */
    wfd = _open_osfhandle((intptr_t)p_in_w, _O_BINARY);
    rfd = _open_osfhandle((intptr_t)p_out_r, _O_BINARY | _O_RDONLY);
    if (wfd < 0 || rfd < 0) {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        if (wfd >= 0) { _close(wfd); p_in_w = NULL; }
        if (rfd >= 0) { _close(rfd); p_out_r = NULL; }
        goto done;
    }
    p_in_w = NULL;
    p_out_r = NULL;
    *out_pid = (long long)(intptr_t)pi.hProcess;
    *out_write_fd = wfd;
    *out_read_fd = rfd;
    ok = 1;

done:
    if (attrs != NULL) {
        DeleteProcThreadAttributeList(attrs);
        free(attrs);
    }
    if (c_in_r != NULL) CloseHandle(c_in_r);
    if (c_out_w != NULL) CloseHandle(c_out_w);
    if (p_in_w != NULL) CloseHandle(p_in_w);
    if (p_out_r != NULL) CloseHandle(p_out_r);
    if (nul != INVALID_HANDLE_VALUE) CloseHandle(nul);
    if (err != NULL) CloseHandle(err);
    free(self);
    free(cmd);
    return ok ? 0 : -1;
}

int scaly_proc_worker_fds(long long argc, char** argv, const char* mode, int* in_fd, int* out_fd)
{
    HANDLE hin, hout;
    if (argc < 4 || strcmp(argv[1], mode) != 0)
        return 0;
    hin = (HANDLE)(uintptr_t)strtoull(argv[2], NULL, 16);
    hout = (HANDLE)(uintptr_t)strtoull(argv[3], NULL, 16);
    *in_fd = _open_osfhandle((intptr_t)hin, _O_BINARY | _O_RDONLY);
    *out_fd = _open_osfhandle((intptr_t)hout, _O_BINARY);
    return 1;
}

int scaly_proc_reap(long long pid)
{
    HANDLE h = (HANDLE)(intptr_t)pid;
    TerminateProcess(h, 1);
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
    return 0;
}

/* An anonymous pipe cannot be waited on, only asked (PeekNamedPipe), so a
 * positive timeout is a loop of short naps. A broken pipe counts as readable,
 * as POLLHUP does: the read that follows answers end of file. Anything that
 * is not a pipe (a console, a file) is -1 — "cannot tell", which the callers
 * read as "nothing waiting". */
int scaly_proc_wait_readable(int fd, int timeout_ms)
{
    intptr_t h = _get_osfhandle(fd);
    ULONGLONG start = GetTickCount64();
    DWORD nap = 1;
    if (h == -1 || h == -2)
        return -1;
    for (;;) {
        DWORD avail = 0;
        if (!PeekNamedPipe((HANDLE)h, NULL, 0, NULL, &avail, NULL)) {
            DWORD e = GetLastError();
            return e == ERROR_BROKEN_PIPE || e == ERROR_HANDLE_EOF ? 1 : -1;
        }
        if (avail > 0)
            return 1;
        if (timeout_ms == 0)
            return 0;
        if (timeout_ms > 0 && GetTickCount64() - start >= (ULONGLONG)timeout_ms)
            return 0;
        Sleep(nap);
        if (nap < 8)
            nap++;
    }
}

/* The CRT opens stdin, stdout and stderr in TEXT mode: "\n" goes out as
 * "\r\n", a "\r\n" comes in as "\n" and a Ctrl-Z byte ends the input. A Scaly
 * program's standard streams carry BYTES on every target (decided 2026-10-03,
 * tests/win32/WINDOWS-BOX.md §8): what the program writes is what leaves it, as
 * on POSIX, and a protocol framed by byte counts (the language server, a JSON
 * pipe) or a byte-compared golden needs no filter. So every program sets them
 * binary BEFORE main, through the CRT's own initializer table (.CRT$XCU, the
 * section C++ static constructors run from). A program the compiler emitted
 * for COFF carries its own constructor for it (Emitter.emit_stdio_binary_ctor#,
 * so a program needs no runtime archive for it); this one serves the
 * executables built from the target-neutral seed -- the compiler and the tool
 * -- and every link that pulls this object (the page allocator calls
 * scaly_aligned_alloc). A
 * console shows a lone "\n" as a new line; what a console READS keeps its "\r",
 * which the line readers strip. scaly_proc_stdio_binary stays for its callers
 * (scalyls' main) and is now a second, harmless call. */
/* SCALY_CRASH_REPORT=1: a hard fault (access violation, illegal instruction)
 * is reported as it happens, before the process dies -- code, PC, the faulting address, the return address and the
 * module the PC lies in ("none": memory no module maps, e.g. the JIT's). A
 * crash witness for the cases a debugger cannot reach: under lldb every
 * guard-page exception of the runtime stops the debugger, and a JIT run then
 * crawls (2026-10-03). Off by default; the report is the only effect. */
static void scaly_crash_put(const char* s)
{
    DWORD w = 0;
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), s, (DWORD)strlen(s), &w, NULL);
}

static LONG WINAPI scaly_crash_report(EXCEPTION_POINTERS* ep)
{
    char buf[512];
    char mod[MAX_PATH] = "none";
    HMODULE h = NULL;
    void* pc = ep->ExceptionRecord->ExceptionAddress;
    unsigned long long fault = ep->ExceptionRecord->NumberParameters >= 2
        ? (unsigned long long)ep->ExceptionRecord->ExceptionInformation[1] : 0;
    unsigned long long ret = 0;
#if defined(_M_ARM64) || defined(__aarch64__)
    ret = ep->ContextRecord->Lr;
#else
    ret = *(unsigned long long*)ep->ContextRecord->Rsp;
#endif
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                           | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)pc, &h) && h != NULL)
        GetModuleFileNameA(h, mod, sizeof mod);
    MEMORY_BASIC_INFORMATION mbi;
    memset(&mbi, 0, sizeof mbi);
    VirtualQuery(pc, &mbi, sizeof mbi);
    snprintf(buf, sizeof buf,
             "scaly crash: code 0x%08lx at pc %p (module %s, +0x%llx), fault address 0x%llx, return 0x%llx; "
             "pc page: state 0x%lx protect 0x%lx, allocation %p\n",
             (unsigned long)ep->ExceptionRecord->ExceptionCode, pc, mod,
             h ? (unsigned long long)((char*)pc - (char*)h) : 0ull, fault, ret,
             (unsigned long)mbi.State, (unsigned long)mbi.Protect, mbi.AllocationBase);
    scaly_crash_put(buf);
    /* The frames the OS can walk: through the exception dispatcher back into
     * the faulting code, as far as unwind data reaches (the JIT's has none). */
    void* frames[16];
    USHORT n = RtlCaptureStackBackTrace(0, 16, frames, NULL);
    for (USHORT i = 0; i < n; i++) {
        char fmod[MAX_PATH] = "none";
        HMODULE fh = NULL;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                               | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               (LPCSTR)frames[i], &fh) && fh != NULL)
            GetModuleFileNameA(fh, fmod, sizeof fmod);
        snprintf(buf, sizeof buf, "  #%u %p %s +0x%llx\n", (unsigned)i, frames[i], fmod,
                 fh ? (unsigned long long)((char*)frames[i] - (char*)fh) : 0ull);
        scaly_crash_put(buf);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

/* Vectored and registered LAST, because a fault in code without unwind data
 * (the JIT's) ends the frame-based search before an unhandled-exception
 * filter is ever asked; first-chance, so only the hard faults are named. */
static LONG CALLBACK scaly_crash_vectored(EXCEPTION_POINTERS* ep)
{
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_ILLEGAL_INSTRUCTION
        || code == EXCEPTION_DATATYPE_MISALIGNMENT || code == EXCEPTION_PRIV_INSTRUCTION)
        scaly_crash_report(ep);
    return EXCEPTION_CONTINUE_SEARCH;
}

static void scaly_stdio_binary_at_start(void)
{
    _setmode(0, _O_BINARY);
    _setmode(1, _O_BINARY);
    _setmode(2, _O_BINARY);
    if (getenv("SCALY_CRASH_REPORT") != NULL)
        AddVectoredExceptionHandler(0, scaly_crash_vectored);
}
#pragma section(".CRT$XCU", read)
__declspec(allocate(".CRT$XCU")) void (*const scaly_stdio_binary_hook)(void) = scaly_stdio_binary_at_start;

void scaly_proc_stdio_binary(void)
{
    scaly_stdio_binary_at_start();
}

/* The file of the running program, for a handed-out program that finds its
 * packages beside itself (cli.scaly, adopt_home). Rule (a): every system asks
 * this differently. The length, or -1 when it is not known or does not fit. */
int scaly_eio_self_path(char* buffer, size_t capacity)
{
    DWORD n = GetModuleFileNameA(NULL, buffer, (DWORD)capacity);
    return n == 0 || n >= capacity ? -1 : (int)n;
}

int scaly_eio_is_symlink(const char* path)
{
    DWORD a = GetFileAttributesA(path);
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_REPARSE_POINT) != 0 ? 1 : 0;
}

/* ---- the JIT's memory: one arena for every section --------------------
 *
 * ★★★WHY (2026-10-03, tests/win32/WINDOWS-BOX.md §8). ORC's default memory
 * manager allocates each SECTION of a JIT'd object on its own, and on COFF
 * every linkonce function is a section of its own (a comdat). VirtualAlloc
 * places those allocations anywhere in the address space, sometimes more than
 * 4 GB apart -- and a PC-relative reference between two of them (ADRP on
 * arm64, +-4 GB; a REL32 on x64, +-2 GB) was then silently TRUNCATED: the
 * jump landed in someone's read-write data with the caller's high bits
 * (SCALY_CRASH_REPORT: pc = fault address, the page MEM_COMMIT and
 * PAGE_READWRITE). How far apart the sections landed varied with the layout,
 * so the opensp tests under the JIT crashed in a different test each run and
 * the small programs never. Here every section of the JIT session comes out
 * of ONE reservation: code from its base upward (direct branches reach
 * +-128 MB on arm64), data from 256 MB above it, so every reference stays in
 * range.
 *
 * ★The LLVM C API is reached through GetProcAddress on LLVM-C.dll, because
 * this object is in every Scaly program and an ordinary program must not
 * depend on LLVM; only a JIT host has the DLL loaded when this runs.
 * ★A JIT host with LLVM linked INTO it has no such DLL (the experiment of
 * tests/win32/WINDOWS-BOX.md §10): there the three functions are looked up in
 * the program itself, which exports them (tools/win-link.sh, the static
 * branch). Without that the arena was silently off and the truncation above
 * was back -- `scaly test` on opensp, 9 of 24 runs on x64.
 *
 * Allocation is a bump in pages, never freed: a JIT session lives as long as
 * the process. Sections are committed read-write, and at finalize the code
 * turns execute-read and read-only data read-only. */
typedef void* (*scaly_mm_ctx_fn)(void*);
typedef void (*scaly_mm_term_fn)(void*);
typedef unsigned char* (*scaly_mm_code_fn)(void*, uintptr_t, unsigned, unsigned, const char*);
typedef unsigned char* (*scaly_mm_data_fn)(void*, uintptr_t, unsigned, unsigned, const char*, int);
typedef int (*scaly_mm_final_fn)(void*, char**);
typedef void (*scaly_mm_destroy_fn)(void*);
typedef void* (*scaly_rtdyld_cb_fn)(void*, void*, scaly_mm_ctx_fn, scaly_mm_term_fn, scaly_mm_code_fn,
                                    scaly_mm_data_fn, scaly_mm_final_fn, scaly_mm_destroy_fn);
typedef void* (*scaly_rtdyld_default_fn)(void*);
typedef void* (*scaly_layer_creator_fn)(void*, void*, const char*);
typedef void (*scaly_set_creator_fn)(void*, scaly_layer_creator_fn, void*);

#define SCALY_JIT_ARENA (1024ull << 20)     /* reserved once, committed as used */
#define SCALY_JIT_DATA_AT (256ull << 20)    /* code below, data above */

static HMODULE scaly_jit_llvm(void)
{
    HMODULE llvm = GetModuleHandleA("LLVM-C.dll");
    return llvm != NULL ? llvm : GetModuleHandleA(NULL);
}

static SRWLOCK scaly_jit_lock = SRWLOCK_INIT;
static unsigned char* scaly_jit_base;
static size_t scaly_jit_code_top, scaly_jit_data_top;

typedef struct { unsigned char* at; size_t size; int kind; } scaly_jit_section; /* kind 0 code, 1 ro, 2 rw */
typedef struct { scaly_jit_section* s; size_t n, cap; } scaly_jit_ctx;

static unsigned char* scaly_jit_take(scaly_jit_ctx* c, uintptr_t size, unsigned align, int kind)
{
    size_t page = 4096, a = align > page ? align : page;
    size_t rounded = (size + page - 1) & ~(page - 1);
    unsigned char* at = NULL;
    if (rounded == 0)
        rounded = page;
    AcquireSRWLockExclusive(&scaly_jit_lock);
    if (kind == 0) {
        size_t off = (scaly_jit_code_top + a - 1) & ~(a - 1);
        if (off + rounded <= SCALY_JIT_DATA_AT) { at = scaly_jit_base + off; scaly_jit_code_top = off + rounded; }
    } else {
        size_t off = (scaly_jit_data_top + a - 1) & ~(a - 1);
        if (off + rounded <= SCALY_JIT_ARENA) { at = scaly_jit_base + off; scaly_jit_data_top = off + rounded; }
    }
    ReleaseSRWLockExclusive(&scaly_jit_lock);
    if (at == NULL || VirtualAlloc(at, rounded, MEM_COMMIT, PAGE_READWRITE) == NULL)
        return NULL;
    if (c->n == c->cap) {
        size_t cap = c->cap ? c->cap * 2 : 16;
        scaly_jit_section* s = (scaly_jit_section*)realloc(c->s, cap * sizeof *s);
        if (s == NULL)
            return NULL;
        c->s = s; c->cap = cap;
    }
    c->s[c->n].at = at; c->s[c->n].size = rounded; c->s[c->n].kind = kind; c->n++;
    return at;
}

static void* scaly_jit_ctx_new(void* unused) { (void)unused; return calloc(1, sizeof(scaly_jit_ctx)); }
static void scaly_jit_terminating(void* unused) { (void)unused; }

static unsigned char* scaly_jit_code(void* c, uintptr_t size, unsigned align, unsigned id, const char* name)
{
    (void)id; (void)name;
    return scaly_jit_take((scaly_jit_ctx*)c, size, align, 0);
}

static unsigned char* scaly_jit_data(void* c, uintptr_t size, unsigned align, unsigned id, const char* name, int ro)
{
    (void)id; (void)name;
    return scaly_jit_take((scaly_jit_ctx*)c, size, align, ro ? 1 : 2);
}

static int scaly_jit_finalize(void* cv, char** err)
{
    scaly_jit_ctx* c = (scaly_jit_ctx*)cv;
    (void)err;
    for (size_t i = 0; i < c->n; i++) {
        DWORD old = 0;
        if (c->s[i].kind == 0) {
            VirtualProtect(c->s[i].at, c->s[i].size, PAGE_EXECUTE_READ, &old);
            FlushInstructionCache(GetCurrentProcess(), c->s[i].at, c->s[i].size);
        } else if (c->s[i].kind == 1) {
            VirtualProtect(c->s[i].at, c->s[i].size, PAGE_READONLY, &old);
        }
    }
    c->n = 0;                               /* finalized; the memory stays */
    return 0;
}

static void scaly_jit_destroy(void* cv)
{
    scaly_jit_ctx* c = (scaly_jit_ctx*)cv;
    free(c->s);
    free(c);
}

static void* scaly_jit_layer(void* unused, void* es, const char* triple)
{
    HMODULE llvm = scaly_jit_llvm();
    (void)unused; (void)triple;
    if (scaly_jit_base == NULL)
        scaly_jit_base = (unsigned char*)VirtualAlloc(NULL, SCALY_JIT_ARENA, MEM_RESERVE, PAGE_NOACCESS);
    if (scaly_jit_base != NULL && scaly_jit_data_top == 0)
        scaly_jit_data_top = SCALY_JIT_DATA_AT;
    if (scaly_jit_base != NULL) {
        scaly_rtdyld_cb_fn make = (scaly_rtdyld_cb_fn)(void*)GetProcAddress(llvm,
            "LLVMOrcCreateRTDyldObjectLinkingLayerWithMCJITMemoryManagerLikeCallbacks");
        if (make != NULL)
            return make(es, NULL, scaly_jit_ctx_new, scaly_jit_terminating, scaly_jit_code,
                        scaly_jit_data, scaly_jit_finalize, scaly_jit_destroy);
    }
    scaly_rtdyld_default_fn plain = (scaly_rtdyld_default_fn)(void*)GetProcAddress(llvm,
        "LLVMOrcCreateRTDyldObjectLinkingLayerWithSectionMemoryManager");
    return plain != NULL ? plain(es) : NULL;
}

/* Called by the compiler's JIT set-up on a COFF host, with its LLJIT builder. */
void scaly_jit_use_arena(void* builder)
{
    HMODULE llvm = scaly_jit_llvm();
    if (llvm == NULL || builder == NULL)
        return;
    scaly_set_creator_fn set = (scaly_set_creator_fn)(void*)GetProcAddress(llvm,
        "LLVMOrcLLJITBuilderSetObjectLinkingLayerCreator");
    if (set != NULL)
        set(builder, scaly_jit_layer, NULL);
}

#else
typedef int scaly_eio_win_not_needed_on_this_target;
#endif /* _WIN32 */
