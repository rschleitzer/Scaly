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
/* ★★★DO NOT MOVE THIS GUARD, and the reason is a contract you cannot see from
 * here (tried and reverted 2026-08-10).
 *
 * The emergency band that section 5.2 of tests/win32/WINDOWS-BOX.md asks for —
 * one ordinary writable page BELOW the guard page, so the exception dispatch has
 * somewhere to spill — looks like it could live entirely in this file: this shim
 * already owns the QUESTION "where is this stack's overflow guard", so it could
 * place the guard one page up, keep the lowest page as the band, and translate
 * every fault address back into the classifier's coordinates on the way out.
 * That was built. It works — guard_probe's sweep goes from 41/64 to 64/64 — and
 * it BREAKS FIFTEEN fiber tests, because `fiber.scaly` reserves the two words
 * immediately above the guard page for the stack pool's free list ("the entry is
 * two size_t slots [next, size] at stack_base + guard page"). Moving the guard
 * onto that node turns every fiber DISPOSAL into a guard-page write.
 *
 * So the position of this guard is an implicit contract between two files, and
 * the band has to move BOTH halves — which means fiber.scaly, a re-converged
 * cycle and a seed refresh. It is not a shim-local fix, however much it looks
 * like one. ★The lesson generalises past this page: a shim that owns a QUESTION
 * does not thereby own the ADDRESS SPACE around its answer. */
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

void* scaly_aligned_alloc(size_t alignment, size_t size)
{
    return _aligned_malloc(size, alignment);   /* note the argument order */
}

void scaly_aligned_free(void* p)
{
    _aligned_free(p);
}

#else
typedef int scaly_eio_win_not_needed_on_this_target;
#endif /* _WIN32 */
