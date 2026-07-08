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
 */

#include <errno.h>
#include <fcntl.h>
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
