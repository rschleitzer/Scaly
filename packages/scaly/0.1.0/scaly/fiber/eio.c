/* Evented-I/O backend shim (self-scaling stage 1, milestone 1.5).
 *
 * One C file covers both OS backends — kqueue on darwin, epoll on linux —
 * selected by the preprocessor at build time (tools/eio.sh compiles it with
 * clang; every libscaly.a build archives eio.o beside fcontext.o, and the
 * seed/compiler links carry it too). The backend cannot live in Scaly code:
 * the committed seed ships ONE scaly.ll for all four LP64 targets, so
 * OS-specific syscalls in the runtime would poison the seed's
 * cross-targetness. The shim also owns the two calls Scaly externs cannot
 * make faithfully: fcntl is VARIADIC in libc (a fixed-prototype extern call
 * silently loses the third argument on arm64 ABIs — flags end up garbage),
 * and errno is a thread-local macro, so read/write wrappers map
 * EAGAIN/EWOULDBLOCK to -2 (a plain -1 stays a hard error).
 *
 * Readiness registration is ONESHOT on both backends (EV_ONESHOT /
 * EPOLLONESHOT): an armed fd fires once and must be re-armed, which keeps
 * the two event models semantically identical. epoll keeps a fired fd
 * registered but disabled, so arming retries EPOLL_CTL_MOD after EEXIST.
 * At most one armed waiter per fd at a time (epoll has one entry per fd;
 * kqueue could hold read+write separately, but the API contracts to the
 * intersection).
 *
 * The TCP helpers (listen/port/connect/accept) are shim-owned for the
 * same cross-targetness reason: struct sockaddr_in's layout is
 * OS-specific (darwin leads with a sin_len byte, linux with a 16-bit
 * sin_family), so Scaly code cannot fill one portably; and accept needs
 * the same errno mapping as read/write (EAGAIN/EWOULDBLOCK -> -2).
 *
 * NO BARE `long` IN AN EXPORTED SIGNATURE (LLP64 rule, 2026-08-09).
 * The same argument that puts this file in C at all applies to its widths:
 * ONE seed serves every target, so a Scaly extern declaration cannot be
 * target-conditional — it says `i64` and `size_t` once, for all of them.
 * C's `long` is 64-bit on LP64 (mac/linux) and 32-bit on LLP64 (Win64),
 * so a `long` here would silently disagree with its own declaration on
 * exactly one target, and the RESULT direction is the dangerous half:
 * a 32-bit return read as i64 leaves the upper half unspecified, which
 * flips the sign and turns every `if r < 0` into a coin flip (the class
 * tests/abi/run.sh exists for). Use `long long` for results and `size_t`
 * for counts — both are 64-bit on every target we ship. tests/abi/run.sh
 * gates this; ctime.c was written this way from the start.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SCALY_EIO_MAX_EVENTS 64

#ifdef __APPLE__

#include <sys/event.h>

/* kevent(2) takes a changelist and an eventlist in the SAME syscall, so
 * arms are buffered here and submitted with the next wait — one syscall
 * where the epoll backend needs one epoll_ctl per arm. A full buffer
 * flushes early (submit-only kevent), so batching degrades gracefully,
 * never drops. THREAD-LOCAL since milestone 2.4: every thread may run
 * its own scheduler + poller, and arm/wait always happen on the
 * poller's home thread (cross-thread wakes go through scaly_eio_wake,
 * which submits its own kevent and never touches this buffer).
 * Per-change kernel errors come back as EV_ERROR entries in the eventlist
 * (not a -1 return); any such entry is a genuine bug (e.g. a closed fd)
 * and fails the wait loudly, matching the old arm-time rc check. */
#define SCALY_EIO_MAX_CHANGES 256
static __thread struct kevent scaly_eio_changes[SCALY_EIO_MAX_CHANGES];
static __thread int scaly_eio_nchanges = 0;
static __thread int scaly_eio_changes_q = -1;

int scaly_eio_create(void)
{
    return kqueue();
}

static int scaly_eio_flush(int q)
{
    int rc = 0;
    if (scaly_eio_nchanges > 0)
        rc = kevent(q, scaly_eio_changes, scaly_eio_nchanges, 0, 0, 0);
    scaly_eio_nchanges = 0;
    return rc < 0 ? -1 : 0;
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    if (scaly_eio_changes_q != q)
    {
        /* a different queue's arms are pending: flush them there first */
        if (scaly_eio_changes_q >= 0 && scaly_eio_flush(scaly_eio_changes_q) < 0)
            return -1;
        scaly_eio_changes_q = q;
    }
    if (scaly_eio_nchanges == SCALY_EIO_MAX_CHANGES && scaly_eio_flush(q) < 0)
        return -1;
    EV_SET(&scaly_eio_changes[scaly_eio_nchanges], fd,
           for_write ? EVFILT_WRITE : EVFILT_READ,
           EV_ADD | EV_ONESHOT, 0, 0, tag);
    scaly_eio_nchanges++;
    return 0;
}

/* ms < 0 blocks until an event fires; ms >= 0 returns 0 if the timeout
 * elapses first (the deadlock-detection wait, milestone 2.4 residual). */
static int scaly_eio_wait_ms(int q, void** tags, int max, int ms)
{
    struct kevent evs[SCALY_EIO_MAX_EVENTS];
    struct kevent* chg = 0;
    struct timespec ts, *tsp = 0;
    int nchg = 0, n, i, out = 0;
    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    if (scaly_eio_changes_q == q)
    {
        chg = scaly_eio_changes;
        nchg = scaly_eio_nchanges;
        scaly_eio_nchanges = 0;
    }
    if (ms >= 0)
    {
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (long)(ms % 1000) * 1000000L;
        tsp = &ts;
    }
    do
    {
        n = kevent(q, chg, nchg, evs, max, tsp);
        /* EINTR fires during the wait, AFTER the changelist was applied;
         * chg stays for the retry anyway — re-adding an identical oneshot
         * event is idempotent (same fd/filter/udata overwrites). */
    }
    while (n < 0 && errno == EINTR);
    if (n < 0)
        return -1;
    for (i = 0; i < n; i++)
    {
        if (evs[i].flags & EV_ERROR)
            return -1;
        tags[out++] = evs[i].udata;
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

/* Cross-thread wake channel (milestone 2.4, shim rule (a): EVFILT_USER is
 * kqueue-only, eventfd is linux-only). wake_create registers a persistent
 * user event on q that reports `tag` when triggered; EV_CLEAR resets it on
 * retrieval, so no rearm and no drain. Returns the wake handle (the user
 * event's ident here, the eventfd on linux), -1 on failure. scaly_eio_wake
 * may be called from ANY thread — it submits its own kevent and never
 * touches the thread-local arm buffer; triggers before or during a wait
 * are never lost (a pending trigger completes the next wait immediately),
 * and multiple triggers coalesce into one report. */
int scaly_eio_wake_create(int q, void* tag)
{
    struct kevent ev;
    EV_SET(&ev, 1, EVFILT_USER, EV_ADD | EV_CLEAR, 0, 0, tag);
    if (kevent(q, &ev, 1, 0, 0, 0) < 0)
        return -1;
    return 1;
}

int scaly_eio_wake(int q, int w)
{
    struct kevent ev;
    EV_SET(&ev, w, EVFILT_USER, 0, NOTE_TRIGGER, 0, 0);
    return kevent(q, &ev, 1, 0, 0, 0) < 0 ? -1 : 0;
}

/* Close the wake channel (milestone 2.5 residual). Shim-owned (rule a):
 * whether the wake handle IS a file descriptor is OS-specific — here it
 * is just the user event's ident (kernel state of the queue itself), so
 * deregister it; on linux it is an eventfd that must be closed. */
int scaly_eio_wake_close(int q, int w)
{
    struct kevent ev;
    EV_SET(&ev, w, EVFILT_USER, EV_DELETE, 0, 0, 0);
    return kevent(q, &ev, 1, 0, 0, 0) < 0 ? -1 : 0;
}

#else

#include <sys/epoll.h>

int scaly_eio_create(void)
{
    return epoll_create1(0);
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    struct epoll_event ev;
    int rc;
    ev.events = (for_write ? EPOLLOUT : EPOLLIN) | EPOLLONESHOT;
    ev.data.ptr = tag;
    rc = epoll_ctl(q, EPOLL_CTL_ADD, fd, &ev);
    if (rc < 0 && errno == EEXIST)
        rc = epoll_ctl(q, EPOLL_CTL_MOD, fd, &ev);
    return rc;
}

/* ms < 0 blocks until an event fires; ms >= 0 returns 0 if the timeout
 * elapses first (the deadlock-detection wait, milestone 2.4 residual). */
static int scaly_eio_wait_ms(int q, void** tags, int max, int ms)
{
    struct epoll_event evs[SCALY_EIO_MAX_EVENTS];
    int n, i;
    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    do
        n = epoll_wait(q, evs, max, ms);
    while (n < 0 && errno == EINTR);
    for (i = 0; i < n; i++)
        tags[i] = evs[i].data.ptr;
    return n;
}

int scaly_eio_wait(int q, void** tags, int max)
{
    return scaly_eio_wait_ms(q, tags, max, -1);
}

int scaly_eio_wait_timeout(int q, void** tags, int max, int ms)
{
    return scaly_eio_wait_ms(q, tags, max, ms);
}

/* Cross-thread wake channel (milestone 2.4): an eventfd registered
 * edge-triggered (EPOLLET, NOT oneshot) reporting `tag`. Edge-triggered
 * means each write is reported exactly once and the counter never needs
 * a reset read — a write before or during an epoll_wait completes that
 * wait, writes between waits coalesce into one report. Returns the
 * eventfd (the wake handle scaly_eio_wake writes to), -1 on failure.
 * scaly_eio_wake may be called from ANY thread. */
#include <sys/eventfd.h>

int scaly_eio_wake_create(int q, void* tag)
{
    struct epoll_event ev;
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0)
        return -1;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = tag;
    if (epoll_ctl(q, EPOLL_CTL_ADD, fd, &ev) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

int scaly_eio_wake(int q, int w)
{
    unsigned long long one = 1;
    ssize_t r;
    (void)q;
    do
        r = write(w, &one, sizeof one);
    while (r < 0 && errno == EINTR);
    return r == (ssize_t)sizeof one ? 0 : -1;
}

/* Close the wake channel (milestone 2.5 residual). Shim-owned (rule a):
 * the handle is a real eventfd here — deregister and close it; on
 * darwin it is only a kqueue ident. */
int scaly_eio_wake_close(int q, int w)
{
    epoll_ctl(q, EPOLL_CTL_DEL, w, 0);
    return close(w);
}

#endif

/* Close the poller QUEUE itself. Both POSIX backends make it a real file
 * descriptor (a kqueue fd, an epoll fd), so this is one line here — it exists
 * for the same reason scaly_eio_wake_close does: what the queue IS differs per
 * OS, and on Windows it is an IOCP HANDLE that the CRT's close() knows nothing
 * about. The caller (Io.close_poller) must not have to know which. */
int scaly_eio_close(int q)
{
    return close(q);
}

int scaly_eio_set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

long long scaly_eio_read(int fd, void* buf, size_t count)
{
    long long r;
    do
        r = read(fd, buf, count);
    while (r < 0 && errno == EINTR);
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;
    return r;
}

/* (c) errno accessor: errno is a thread-local macro backed by a per-OS
 * accessor symbol, unreadable through a direct Scaly extern. Callers must
 * read it on the same thread, immediately after the failing libc call. */
int scaly_eio_errno(void)
{
    return errno;
}

long long scaly_eio_write(int fd, const void* buf, size_t count)
{
    long long r;
    do
        r = write(fd, buf, count);
    while (r < 0 && errno == EINTR);
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;
    return r;
}

/* Suppress SIGPIPE per SOCKET on darwin (SO_NOSIGPIPE — there is no
 * MSG_NOSIGNAL send flag there); linux suppresses per WRITE in
 * scaly_eio_tcp_write instead. A cluster write to a peer that died
 * mid-frame must surface as -1/EPIPE, never kill the process (stage 7,
 * milestone 7.3). Rule (a): both constants are OS-specific. */
static void scaly_eio_sock_init(int fd)
{
#ifdef __APPLE__
    int one = 1;
    setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &one, sizeof one);
#else
    (void)fd;
#endif
}

static int scaly_eio_tcp_listen_at(unsigned int ip_host_order, int port)
{
    struct sockaddr_in addr;
    int one = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    scaly_eio_sock_init(fd);
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(ip_host_order);
    if (bind(fd, (struct sockaddr*)&addr, sizeof addr) < 0
        || listen(fd, 64) < 0
        || scaly_eio_set_nonblocking(fd) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

/* A nonblocking TCP socket listening on 127.0.0.1:port (port 0 asks the
 * kernel for an ephemeral port — scaly_eio_tcp_port reads the assignment).
 * Returns the fd, -1 on failure. */
int scaly_eio_tcp_listen(int port)
{
    return scaly_eio_tcp_listen_at(INADDR_LOOPBACK, port);
}

/* Like scaly_eio_tcp_listen but bound to INADDR_ANY — a cluster node
 * accepting real remote peers (stage 7, milestone 7.3). */
int scaly_eio_tcp_listen_any(int port)
{
    return scaly_eio_tcp_listen_at(INADDR_ANY, port);
}

/* The local port a bound socket ended up on, -1 on failure. */
int scaly_eio_tcp_port(int fd)
{
    struct sockaddr_in addr;
    socklen_t len = sizeof addr;
    if (getsockname(fd, (struct sockaddr*)&addr, &len) < 0)
        return -1;
    return ntohs(addr.sin_port);
}

/* Connect to 127.0.0.1:port with a blocking handshake (instant against a
 * live local listener; the caller makes the fd nonblocking afterwards for
 * suspending I/O). Returns the fd, -1 on failure. */
int scaly_eio_tcp_connect(int port)
{
    struct sockaddr_in addr;
    int rc;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    do
        rc = connect(fd, (struct sockaddr*)&addr, sizeof addr);
    while (rc < 0 && errno == EINTR);
    if (rc < 0)
    {
        close(fd);
        return -1;
    }
    scaly_eio_sock_init(fd);
    return fd;
}

/* Resolve host by name and connect to host:port — real hosts for the
 * stage-7 Node runtime (milestone 7.3). Shim rule (a): struct addrinfo's
 * layout and the AI_/AF_ constant values are OS-specific, so getaddrinfo
 * is unreachable from a portable Scaly extern. Blocking handshake like
 * scaly_eio_tcp_connect; tries every returned address. Returns the fd,
 * -1 on failure (resolution or connection). */
int scaly_eio_tcp_connect_host(const char* host, int port)
{
    struct addrinfo hints;
    struct addrinfo* res = 0;
    struct addrinfo* ai;
    char portbuf[16];
    int fd = -1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(portbuf, sizeof portbuf, "%d", port);
    if (getaddrinfo(host, portbuf, &hints, &res) != 0)
        return -1;
    for (ai = res; ai; ai = ai->ai_next)
    {
        int rc;
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0)
            continue;
        do
            rc = connect(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen);
        while (rc < 0 && errno == EINTR);
        if (rc == 0)
        {
            scaly_eio_sock_init(fd);
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    return fd;
}

/* Socket write that never raises SIGPIPE: send() with MSG_NOSIGNAL on
 * linux; darwin lacks the flag, so sockets carry SO_NOSIGPIPE from
 * scaly_eio_sock_init and plain send() suffices. Same EINTR retry and
 * EAGAIN/EWOULDBLOCK -> -2 mapping as scaly_eio_write. */
long long scaly_eio_tcp_write(int fd, const void* buf, size_t count)
{
    long long r;
#ifdef __APPLE__
    int flags = 0;
#else
    int flags = MSG_NOSIGNAL;
#endif
    do
        r = send(fd, buf, count, flags);
    while (r < 0 && errno == EINTR);
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;
    return r;
}

/* Fiber guard-page overflow diagnostics (shim-owned per containment rule
 * (a): struct sigaction and stack_t layouts are OS-specific, and the
 * handler must run on an alternate stack — the overflowed fiber stack has
 * no room left). The classifier is a Scaly function that reads globals
 * only; when it recognizes the fault address as the running fiber's guard
 * page, the process dies with a message and exit 108. Any other fault
 * resets to the default action and RETURNS — re-executing the faulting
 * access then crashes exactly as without the handler (signal, core,
 * si_addr all preserved). */
static int (*scaly_guard_classify)(void*);
static char scaly_guard_altstack[32768];

static void scaly_guard_handler(int sig, siginfo_t* si, void* ctx)
{
    (void)ctx;
    if (scaly_guard_classify && si && scaly_guard_classify(si->si_addr))
    {
        static const char msg[] = "fiber stack overflow (guard page hit)\n";
        ssize_t w = write(2, msg, sizeof msg - 1);
        (void)w;
        _exit(108);
    }
    signal(sig, SIG_DFL);
}

/* Establish the guard region at the LOW end of a fiber stack. It is a shim
 * call rather than a plain mprotect at the call site because it exports the
 * QUESTION ("make this the stack's overflow guard") and the two platforms
 * answer it with different MECHANISMS, not just different spellings — see the
 * Windows file, where PAGE_NOACCESS would leave the kernel nowhere to build
 * the exception record and the overflow would die before any handler ran. */
#include <sys/mman.h>

int scaly_stack_guard(void* base, size_t len)
{
    return mprotect(base, len, PROT_NONE);
}

int scaly_guard_install(int (*classify)(void*))
{
    stack_t ss;
    struct sigaction sa;
    scaly_guard_classify = classify;
    ss.ss_sp = scaly_guard_altstack;
    ss.ss_size = sizeof scaly_guard_altstack;
    ss.ss_flags = 0;
    if (sigaltstack(&ss, 0) < 0)
        return -1;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = scaly_guard_handler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGSEGV, &sa, 0) < 0)
        return -1;
    if (sigaction(SIGBUS, &sa, 0) < 0)
        return -1;
    return 0;
}

/* Accept a pending connection. Returns the new fd (blocking mode — the
 * caller decides), -2 when none is pending (park on readability), -1 on
 * a hard error. A connection that died in the backlog (ECONNABORTED) is
 * skipped, not reported. */
int scaly_eio_accept(int fd)
{
    for (;;)
    {
        int c = accept(fd, 0, 0);
        if (c >= 0)
        {
            scaly_eio_sock_init(c);
            return c;
        }
        if (errno == EINTR || errno == ECONNABORTED)
            continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return -2;
        return -1;
    }
}

/* Online CPU count — the ncpu-based worker default for task pools
 * (stage-3 milestone 3.3). Shim rule (a): the _SC_NPROCESSORS_ONLN
 * constant's VALUE is OS-specific (darwin 58, glibc 84), so a Scaly
 * extern cannot pass it portably — the seed ships one scaly.ll for
 * all targets. Never less than 1. */
int scaly_eio_ncpu(void)
{
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 1)
        return 1;
    return (int)n;
}

/* Monotonic nanosecond clock — the deferred parallel-for driver's
 * calibration source (stage-4 milestone 4.2). Shim rule (a): the
 * CLOCK_MONOTONIC clockid VALUE is OS-specific (glibc 1, darwin 6),
 * so a Scaly extern cannot pass it portably. */
long long scaly_eio_now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/* Filesystem block size of a file — OpenSP's
 * PosixBaseStorageObject::getBlockSize (SP_STAT_BLKSIZE): st_blksize for
 * regular files, the 8192 default otherwise. The parser's read-block
 * boundary replication (data-token splits observable through the DSSSL
 * grove) needs the exact per-file value. Shim rule (a): struct stat's
 * layout is OS-specific. */
#include <sys/stat.h>
long long scaly_eio_blksize_path(const char *path)
{
    struct stat sb;
    if (stat(path, &sb) < 0)
        return 8192;
    if (!S_ISREG(sb.st_mode))
        return 8192;
    return (long long)sb.st_blksize;
}

long long scaly_eio_blksize_fd(int fd)
{
    struct stat sb;
    if (fstat(fd, &sb) < 0)
        return 8192;
    if (!S_ISREG(sb.st_mode))
        return 8192;
    return (long long)sb.st_blksize;
}

/* 64-bit file position — replaces the direct `fseek`/`ftell` externs.
 *
 * C's fseek/ftell carry `long` offsets, i.e. 64-bit on LP64 and 32-bit on
 * LLP64 (Win64), so ONE Scaly declaration cannot describe both (check 4 of
 * tests/abi/run.sh). And the consequence is worse than a width: `ftell` is how
 * scaly/os/File.scaly measures a FILE's SIZE, so a 32-bit result would cap
 * every source file at 2 GB and turn the `< 0` error test into a coin flip —
 * the sign-flip class check 1 of that suite exists for.
 *
 * The 64-bit variants exist on every target but under OS-SPECIFIC NAMES —
 * ftello/fseeko on POSIX, _ftelli64/_fseeki64 on MSVC — which is containment
 * rule (a) verbatim, hence a shim rather than a direct extern. The Windows arm
 * is one #ifdef and two names; it is deliberately NOT written yet, because it
 * cannot be verified on an LP64 host (stage 7 brocken 3 owns it).
 *
 * FILE* crosses as void* so the Scaly side keeps the `pointer[void]` it
 * already used for the stream, and EVERY other value crosses 64-bit wide —
 * `whence` included, though C's own fseek takes an `int` there. That is the
 * rule ctime.c states in its header: a 64-bit boundary lets a Scaly `int`
 * parameter match without an i32 declaration, so the call sites need no cast
 * and the named SEEK_* constants (declared `int`) pass straight through. */
long long scaly_eio_tell(void* stream)
{
    return (long long)ftello((FILE*)stream);
}

long long scaly_eio_seek(void* stream, long long offset, long long whence)
{
    return (long long)fseeko((FILE*)stream, (off_t)offset, (int)whence);
}

/* ---- OS facts the runtime cannot ask for directly ----------------------
 *
 * These are not evented I/O, and putting them here stretches this file's name.
 * The alternative was a fourth shim, which would have to be threaded through
 * every archive build and every explicit object list (build-from-seed.sh,
 * seed.sh, verify-seed.sh, install.sh, link-lto.sh and two suites) for two
 * functions apiece. This file is already where the runtime's OS questions land
 * — ncpu, the monotonic clock, st_blksize — so they join those.
 *
 * ALIGNED ALLOCATION comes in a PAIR, and the pairing is the whole point
 * (stage 7, brocken 5). POSIX pairs aligned_alloc with plain free(); Windows
 * has no aligned_alloc at all, only _aligned_malloc, whose memory MUST be
 * released with _aligned_free — passing it to free() is undefined. So the two
 * halves cannot be split across the C boundary: a Scaly side that called
 * aligned_alloc and free would be correct on three targets and corrupt the
 * heap on the fourth. Every allocation in the runtime that is later freed goes
 * through this pair; a `grep -c 'malloc\|free' packages/scaly` that finds
 * anything else is the signal that the invariant broke.
 *
 * THE STACK LIMIT replaces a direct getrlimit(RLIMIT_STACK), which Windows
 * does not have — and the difference is not only the spelling: POSIX reports a
 * PROCESS resource limit, Windows reports the CURRENT THREAD's bounds. Asking
 * "how big is the stack I am running on" is the question the caller
 * (Planner.check_nesting_stack) actually has, and it is the one that survives
 * the translation. 0 means "cannot tell", and the caller falls back.
 */

#include <sys/resource.h>
#include <stdlib.h>

long long scaly_stack_limit(void)
{
    struct rlimit rl;
    if (getrlimit(RLIMIT_STACK, &rl) != 0)
        return 0;
    if (rl.rlim_cur == RLIM_INFINITY)
        return 0;
    return (long long)rl.rlim_cur;
}

/* --- fault injection for the out-of-memory gates -------------------------
 *
 * ★★★It exists for the same reason SCALYC_STACK_BUDGET exists in the
 * planner: an out-of-memory trap cannot be exercised deterministically any
 * other way, and a trap no test ever fires is a trap that may have stopped
 * working (TRAPS.md 1.5). `scaly_alloc_fail_after(n)` makes the (n+1)-th
 * following aligned allocation answer NULL, once; `scaly_alloc_fail_after(-1)`
 * disarms it.
 *
 * ★It is a FUNCTION and not an environment variable because the variable
 * would have to be read before the prelude's first allocation, which happens
 * long before a fixture's first statement.
 *
 * ★Plain statics, not __thread: the arming test is single-threaded, and a
 * per-thread counter would make "the next allocation" mean something else
 * depending on who allocates. Nothing arms it unless a test does.
 */
static long long scaly_alloc_fail_left = -1;

void scaly_alloc_fail_after(long long n)
{
    scaly_alloc_fail_left = n;
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
    return aligned_alloc(alignment, size);
}

void scaly_aligned_free(void* p)
{
    free(p);
}
