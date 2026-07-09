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
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
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

int scaly_eio_set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

long scaly_eio_read(int fd, void* buf, unsigned long count)
{
    long r;
    do
        r = read(fd, buf, count);
    while (r < 0 && errno == EINTR);
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;
    return r;
}

long scaly_eio_write(int fd, const void* buf, unsigned long count)
{
    long r;
    do
        r = write(fd, buf, count);
    while (r < 0 && errno == EINTR);
    if (r < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -2;
    return r;
}

/* A nonblocking TCP socket listening on 127.0.0.1:port (port 0 asks the
 * kernel for an ephemeral port — scaly_eio_tcp_port reads the assignment).
 * Returns the fd, -1 on failure. */
int scaly_eio_tcp_listen(int port)
{
    struct sockaddr_in addr;
    int one = 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(fd, (struct sockaddr*)&addr, sizeof addr) < 0
        || listen(fd, 64) < 0
        || scaly_eio_set_nonblocking(fd) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
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
    return fd;
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
            return c;
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
long scaly_eio_now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}
