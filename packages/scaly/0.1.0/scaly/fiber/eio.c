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
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SCALY_EIO_MAX_EVENTS 64

#ifdef __APPLE__

#include <sys/event.h>

int scaly_eio_create(void)
{
    return kqueue();
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    struct kevent ch;
    EV_SET(&ch, fd, for_write ? EVFILT_WRITE : EVFILT_READ,
           EV_ADD | EV_ONESHOT, 0, 0, tag);
    return kevent(q, &ch, 1, 0, 0, 0);
}

int scaly_eio_wait(int q, void** tags, int max)
{
    struct kevent evs[SCALY_EIO_MAX_EVENTS];
    int n, i;
    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    do
        n = kevent(q, 0, 0, evs, max, 0);
    while (n < 0 && errno == EINTR);
    for (i = 0; i < n; i++)
        tags[i] = evs[i].udata;
    return n;
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

int scaly_eio_wait(int q, void** tags, int max)
{
    struct epoll_event evs[SCALY_EIO_MAX_EVENTS];
    int n, i;
    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    do
        n = epoll_wait(q, evs, max, -1);
    while (n < 0 && errno == EINTR);
    for (i = 0; i < n; i++)
        tags[i] = evs[i].data.ptr;
    return n;
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
