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

/* sched_getaffinity and CPU_COUNT (scaly_eio_ncpu) are GNU extensions:
 * glibc declares them only under _GNU_SOURCE, which must precede the
 * first system header. */
#ifdef __linux__
#define _GNU_SOURCE
#include <sched.h>
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SCALY_EIO_MAX_EVENTS 64

/* The listen backlog asked of the kernel. Every kernel clamps it to its own
 * ceiling (linux net.core.somaxconn, darwin kern.ipc.somaxconn), so asking
 * for the most means getting whatever the host is configured to give. 64
 * was the value until 2026-09-28: a server under a few thousand connections
 * that reconnect (HttpArena's limited-conn profile: 4096 connections, a new
 * one every ten requests) drops SYNs against a queue that short. */
#define SCALY_EIO_LISTEN_BACKLOG 65535

/* A WATCHED descriptor's event (scaly_eio_watch) comes back as its tag —
 * a record aligned to 8 — with bit 2 set and bits 0/1 naming the
 * directions that fired. The other tags keep their low bits clear (task
 * records) or are the sentinels 0 (the main context) and 1 (the wake
 * handle), so the Scaly side (Scheduler.poll_io_by) tells them apart. */
#define SCALY_EIO_WATCH       4u
#define SCALY_EIO_WATCH_READ  1u
#define SCALY_EIO_WATCH_WRITE 2u

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

static int scaly_eio_arm_raw(int q, int fd, short filter, unsigned short flags, void* tag)
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
    EV_SET(&scaly_eio_changes[scaly_eio_nchanges], fd, filter, flags, 0, 0, tag);
    scaly_eio_nchanges++;
    return 0;
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    return scaly_eio_arm_raw(q, fd, for_write ? EVFILT_WRITE : EVFILT_READ,
                             EV_ADD | EV_ONESHOT, tag);
}

/* Watch fd for its whole life, edge-triggered in both directions
 * (EV_CLEAR, not oneshot): no re-arm per wait. The two filters report
 * tag|5 (readable) and tag|6 (writable) — see SCALY_EIO_WATCH above.
 * Submitted AT ONCE, not buffered like an arm: a watched stream may be
 * served and closed before the next wait, and a buffered change for a
 * descriptor closed meanwhile comes back as EV_ERROR (EBADF) and fails
 * that wait — an arm is only ever made by a task about to park on a live
 * descriptor, a watch by a task about to READ one. One kevent per
 * stream, where the oneshot path costs one change per wait. */
int scaly_eio_watch(int q, int fd, void* tag)
{
    size_t t = (size_t)tag;
    struct kevent ch[2];
    EV_SET(&ch[0], fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0,
           (void*)(t | SCALY_EIO_WATCH | SCALY_EIO_WATCH_READ));
    EV_SET(&ch[1], fd, EVFILT_WRITE, EV_ADD | EV_CLEAR, 0, 0,
           (void*)(t | SCALY_EIO_WATCH | SCALY_EIO_WATCH_WRITE));
    return kevent(q, ch, 2, 0, 0, 0) < 0 ? -1 : 0;
}

/* ns < 0 blocks until an event fires; ns >= 0 returns 0 if the timeout
 * elapses first. In nanoseconds down to the kernel (2026-09-30): a timer
 * rounded up to whole milliseconds woke a 10 ms sleep 1-2 ms late, and a
 * wait that ended a hair before its deadline waited a whole millisecond
 * more -- HttpArena's async profile is a 10 ms sleep per request. */
static int scaly_eio_wait_ns(int q, void** tags, int max, long long ns)
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
    if (ns >= 0)
    {
        ts.tv_sec = (time_t)(ns / 1000000000LL);
        ts.tv_nsec = (long)(ns % 1000000000LL);
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
    return scaly_eio_wait_ns(q, tags, max, -1);
}

int scaly_eio_wait_timeout(int q, void** tags, int max, int ms)
{
    return scaly_eio_wait_ns(q, tags, max, ms < 0 ? -1 : (long long)ms * 1000000LL);
}

/* The poller's wait bounded in nanoseconds (Scheduler.poll_io_by's timers). */
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns)
{
    return scaly_eio_wait_ns(q, tags, max, ns < 0 ? 0 : ns);
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

/* The completion path (io_uring, linux): not on kqueue. */
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

/* Which backend the poller is (see the linux twin): kqueue. */
int scaly_eio_backend(int q)
{
    (void)q;
    return 2;
}

#else

#include <linux/io_uring.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

/* ---- io_uring (2026-09-28, ROADMAP-http.md) ------------------------------
 *
 * The poller is an io_uring when the kernel offers one this shim can use
 * (IORING_FEAT_EXT_ARG, 5.11: a wait with a timeout; NODROP; SINGLE_MMAP)
 * and nothing refuses it (a container's default seccomp profile does:
 * io_uring_setup fails with EPERM) — else, and with SCALY_IO_URING=0,
 * epoll as before. Raw system calls, no liburing: one C file stays one C
 * file.
 *
 * The contract of the functions below does not change: arm is a oneshot
 * IORING_OP_POLL_ADD reporting its tag, wait is ONE io_uring_enter that
 * submits every queued entry and waits for completions, the wake handle
 * is an eventfd polled by the ring. What the ring adds is the COMPLETION
 * model a stream uses (scaly_eio_submit_recv/_send, Io.read_stream): the
 * receive or send itself is queued, the task parks, and the result lands
 * in the tag's record — so every connection of a scheduler shares one
 * system call per round, where epoll costs a read and a write each.
 *
 * A completion's tag is a record aligned to 16 whose first two words take
 * the result (recv at +0, send at +8); it comes back with bit 3 set AND
 * bit 0 (recv) or bit 1 (send) — a task record's low three bits are clear
 * but bit 3 may be set, so bit 3 alone marks nothing. The wake poll uses
 * the reserved user_data SCALY_RING_WAKE (7), which no other tag can have
 * (the sentinels are 0 and 1, a completion is at least 16 + 9). */

#define SCALY_EIO_COMPLETION 8u
#define SCALY_RING_WAKE 7ull
#define SCALY_RING_ENTRIES 4096u
#define SCALY_RING_MAX 65536

struct scaly_ring
{
    int fd;
    unsigned entries;
    unsigned *sq_head, *sq_tail, *sq_mask, *sq_array;
    unsigned *cq_head, *cq_tail, *cq_mask;
    struct io_uring_sqe* sqes;
    struct io_uring_cqe* cqes;
    void* ring_ptr;
    size_t ring_sz, sqes_sz;
    unsigned to_submit;
    int wake_fd;
    void* wake_tag;
};

/* One slot per ring descriptor. A ring is created, used and closed by ONE
 * thread (its scheduler's); cross-thread wakes write the eventfd and never
 * touch the ring, so a slot is never read by another thread. */
static struct scaly_ring* scaly_rings[SCALY_RING_MAX];

static struct scaly_ring* scaly_ring_of(int q)
{
    return (q >= 0 && q < SCALY_RING_MAX) ? scaly_rings[q] : 0;
}

static int scaly_ring_create(void)
{
    struct io_uring_params p;
    struct scaly_ring* r;
    const char* env = getenv("SCALY_IO_URING");
    char* base;
    int fd;
    if (env != NULL && env[0] == '0')
        return -1;
    /* One thread submits and waits (a scheduler's): completions are then
     * run in a batch when it waits (DEFER_TASKRUN, 6.1) instead of
     * interrupting it one by one. A kernel without the flags refuses
     * them with EINVAL; the ring is then made without. */
    memset(&p, 0, sizeof p);
    p.flags = IORING_SETUP_SINGLE_ISSUER | IORING_SETUP_DEFER_TASKRUN;
    fd = (int)syscall(__NR_io_uring_setup, SCALY_RING_ENTRIES, &p);
    if (fd < 0 && errno == EINVAL)
    {
        memset(&p, 0, sizeof p);
        fd = (int)syscall(__NR_io_uring_setup, SCALY_RING_ENTRIES, &p);
    }
    if (fd < 0)
        return -1;
    if (fd >= SCALY_RING_MAX
        || !(p.features & IORING_FEAT_EXT_ARG)
        || !(p.features & IORING_FEAT_NODROP)
        || !(p.features & IORING_FEAT_SINGLE_MMAP))
    {
        close(fd);
        return -1;
    }
    r = (struct scaly_ring*)calloc(1, sizeof *r);
    if (r == NULL)
    {
        close(fd);
        return -1;
    }
    r->ring_sz = p.sq_off.array + p.sq_entries * sizeof(unsigned);
    if (p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe) > r->ring_sz)
        r->ring_sz = p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe);
    r->ring_ptr = mmap(0, r->ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
                       fd, IORING_OFF_SQ_RING);
    r->sqes_sz = p.sq_entries * sizeof(struct io_uring_sqe);
    r->sqes = (struct io_uring_sqe*)mmap(0, r->sqes_sz, PROT_READ | PROT_WRITE,
                                         MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQES);
    if (r->ring_ptr == MAP_FAILED || (void*)r->sqes == MAP_FAILED)
    {
        if (r->ring_ptr != MAP_FAILED)
            munmap(r->ring_ptr, r->ring_sz);
        if ((void*)r->sqes != MAP_FAILED)
            munmap(r->sqes, r->sqes_sz);
        free(r);
        close(fd);
        return -1;
    }
    base = (char*)r->ring_ptr;
    r->fd = fd;
    r->entries = p.sq_entries;
    r->sq_head = (unsigned*)(base + p.sq_off.head);
    r->sq_tail = (unsigned*)(base + p.sq_off.tail);
    r->sq_mask = (unsigned*)(base + p.sq_off.ring_mask);
    r->sq_array = (unsigned*)(base + p.sq_off.array);
    r->cq_head = (unsigned*)(base + p.cq_off.head);
    r->cq_tail = (unsigned*)(base + p.cq_off.tail);
    r->cq_mask = (unsigned*)(base + p.cq_off.ring_mask);
    r->cqes = (struct io_uring_cqe*)(base + p.cq_off.cqes);
    r->wake_fd = -1;
    scaly_rings[fd] = r;
    return fd;
}

/* Submit what is queued; with wait, also wait for min_complete
 * completions, at most ms milliseconds (ms < 0: no bound). 0 on success
 * and on an expired timeout, -1 on a hard error. */
static int scaly_ring_enter(struct scaly_ring* r, unsigned min_complete, int wait, long long ns)
{
    struct io_uring_getevents_arg arg;
    struct __kernel_timespec ts;
    unsigned flags = 0;
    void* argp = 0;
    size_t argsz = 0;
    int rc;
    if (wait)
    {
        flags |= IORING_ENTER_GETEVENTS;
        if (ns >= 0)
        {
            ts.tv_sec = ns / 1000000000LL;
            ts.tv_nsec = ns % 1000000000LL;
            memset(&arg, 0, sizeof arg);
            arg.ts = (unsigned long long)(size_t)&ts;
            flags |= IORING_ENTER_EXT_ARG;
            argp = &arg;
            argsz = sizeof arg;
        }
    }
    for (;;)
    {
        rc = (int)syscall(__NR_io_uring_enter, r->fd, r->to_submit, wait ? min_complete : 0,
                          flags, argp, argsz);
        if (rc >= 0)
        {
            r->to_submit -= (unsigned)rc;
            return 0;
        }
        if (errno == EINTR)
            continue;
        /* the timeout expired; the completion queue is backed up (NODROP:
         * nothing is lost, the caller reaps and comes back) */
        if (errno == ETIME || errno == EBUSY || errno == EAGAIN)
            return 0;
        return -1;
    }
}

static struct io_uring_sqe* scaly_ring_sqe(struct scaly_ring* r)
{
    unsigned tail = *r->sq_tail;
    unsigned head = __atomic_load_n(r->sq_head, __ATOMIC_ACQUIRE);
    unsigned idx;
    struct io_uring_sqe* sqe;
    if (tail - head >= r->entries)
    {
        if (scaly_ring_enter(r, 0, 0, 0) < 0)
            return 0;
        head = __atomic_load_n(r->sq_head, __ATOMIC_ACQUIRE);
        if (tail - head >= r->entries)
            return 0;
    }
    idx = tail & *r->sq_mask;
    sqe = &r->sqes[idx];
    memset(sqe, 0, sizeof *sqe);
    r->sq_array[idx] = idx;
    return sqe;
}

static void scaly_ring_commit(struct scaly_ring* r)
{
    __atomic_store_n(r->sq_tail, *r->sq_tail + 1, __ATOMIC_RELEASE);
    r->to_submit++;
}

static int scaly_ring_poll(struct scaly_ring* r, int fd, unsigned events, unsigned long long ud)
{
    struct io_uring_sqe* sqe = scaly_ring_sqe(r);
    if (sqe == 0)
        return -1;
    sqe->opcode = IORING_OP_POLL_ADD;
    sqe->fd = fd;
    sqe->poll32_events = events;
    sqe->user_data = ud;
    scaly_ring_commit(r);
    return 0;
}

static int scaly_ring_wait(struct scaly_ring* r, void** tags, int max, long long ns)
{
    unsigned head = *r->cq_head;
    unsigned tail = __atomic_load_n(r->cq_tail, __ATOMIC_ACQUIRE);
    int out = 0;
    if (head == tail)
    {
        if (scaly_ring_enter(r, 1, 1, ns) < 0)
            return -1;
        tail = __atomic_load_n(r->cq_tail, __ATOMIC_ACQUIRE);
    }
    else if (r->to_submit > 0 && scaly_ring_enter(r, 0, 1, 0) < 0)
        return -1;
    while (head != tail && out < max)
    {
        struct io_uring_cqe* c = &r->cqes[head & *r->cq_mask];
        unsigned long long ud = c->user_data;
        int res = c->res;
        head++;
        if (ud == SCALY_RING_WAKE)
        {
            /* the wake eventfd: reset its counter, poll it again */
            unsigned long long v;
            if (r->wake_fd >= 0)
            {
                while (read(r->wake_fd, &v, sizeof v) > 0)
                {
                }
                scaly_ring_poll(r, r->wake_fd, POLLIN, SCALY_RING_WAKE);
                tags[out++] = r->wake_tag;
            }
        }
        else if ((ud & SCALY_EIO_COMPLETION) && (ud & (SCALY_EIO_WATCH_READ | SCALY_EIO_WATCH_WRITE)))
        {
            /* A completion: bit 3 AND a direction bit. Bit 3 alone is no
             * mark — a task record (a poll's tag) is only 8-aligned and
             * may have it set; taking one for a completion wrote the
             * result 8 bytes into a neighbour on the scheduler's page. */
            long long* cell = (long long*)(size_t)(ud & ~15ull);
            cell[(ud & SCALY_EIO_WATCH_WRITE) ? 1 : 0] = res;
            tags[out++] = (void*)(size_t)ud;
        }
        else
            tags[out++] = (void*)(size_t)ud;
    }
    __atomic_store_n(r->cq_head, head, __ATOMIC_RELEASE);
    return out;
}

static int scaly_ring_submit_io(int q, int opcode, int fd, void* buf, size_t count, void* tag,
                                unsigned direction)
{
    struct scaly_ring* r = scaly_ring_of(q);
    struct io_uring_sqe* sqe;
    if (r == 0)
        return -2;
    sqe = scaly_ring_sqe(r);
    if (sqe == 0)
        return -1;
    sqe->opcode = (unsigned char)opcode;
    sqe->fd = fd;
    sqe->addr = (unsigned long long)(size_t)buf;
    sqe->len = (unsigned)(count > 0x7FFFFFFFu ? 0x7FFFFFFFu : count);
    sqe->msg_flags = opcode == IORING_OP_SEND ? MSG_NOSIGNAL : 0;
    sqe->user_data = (unsigned long long)(size_t)tag | SCALY_EIO_COMPLETION | direction;
    scaly_ring_commit(r);
    return 0;
}

/* Queue a receive of up to count bytes into buf, reported as tag|9 with the
 * byte count (0 at end of stream, -errno on an error) at tag+0. -2 when
 * the poller is not a ring. */
int scaly_eio_submit_recv(int q, int fd, void* buf, size_t count, void* tag)
{
    return scaly_ring_submit_io(q, IORING_OP_RECV, fd, buf, count, tag, SCALY_EIO_WATCH_READ);
}

/* Queue a send of count bytes, reported as tag|10 with the count sent
 * (-errno on an error) at tag+8. -2 when the poller is not a ring. */
int scaly_eio_submit_send(int q, int fd, void* buf, size_t count, void* tag)
{
    return scaly_ring_submit_io(q, IORING_OP_SEND, fd, buf, count, tag, SCALY_EIO_WATCH_WRITE);
}

/* Does the poller take receives and sends (a ring)? 1 or 0. */
int scaly_eio_completions(int q)
{
    return scaly_ring_of(q) != 0;
}

/* Which backend the poller is: 1 epoll, 2 kqueue, 3 io_uring, 4 IOCP
 * (Io.backend; tests/fiber/io_backend.scaly names it). */
int scaly_eio_backend(int q)
{
    return scaly_ring_of(q) != 0 ? 3 : 1;
}

static void scaly_ring_close(int q)
{
    struct scaly_ring* r = scaly_ring_of(q);
    if (r == 0)
        return;
    scaly_rings[q] = 0;
    munmap(r->sqes, r->sqes_sz);
    munmap(r->ring_ptr, r->ring_sz);
    free(r);
}

/* ---- the poller: a ring, else epoll -------------------------------------- */

int scaly_eio_create(void)
{
    int fd = scaly_ring_create();
    if (fd >= 0)
        return fd;
    return epoll_create1(0);
}

/* Watch fd for its whole life, edge-triggered in both directions
 * (EPOLLET, not oneshot): no epoll_ctl per wait. The event reports
 * tag|4 plus the directions that fired (the wait below decodes them).
 * -2 on a ring, whose streams take the completion path instead. */
int scaly_eio_watch(int q, int fd, void* tag)
{
    struct epoll_event ev;
    int rc;
    if (scaly_ring_of(q) != 0)
        return -2;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    ev.data.u64 = (unsigned long long)(size_t)tag | SCALY_EIO_WATCH;
    rc = epoll_ctl(q, EPOLL_CTL_ADD, fd, &ev);
    if (rc < 0 && errno == EEXIST)
        rc = epoll_ctl(q, EPOLL_CTL_MOD, fd, &ev);
    return rc;
}

int scaly_eio_arm(int q, int fd, int for_write, void* tag)
{
    struct epoll_event ev;
    struct scaly_ring* r = scaly_ring_of(q);
    int rc;
    if (r != 0)
        return scaly_ring_poll(r, fd, for_write ? POLLOUT : POLLIN, (unsigned long long)(size_t)tag);
    ev.events = (for_write ? EPOLLOUT : EPOLLIN) | EPOLLONESHOT;
    ev.data.ptr = tag;
    rc = epoll_ctl(q, EPOLL_CTL_ADD, fd, &ev);
    if (rc < 0 && errno == EEXIST)
        rc = epoll_ctl(q, EPOLL_CTL_MOD, fd, &ev);
    return rc;
}

/* epoll_pwait2 (5.11) takes a timespec; before it, epoll_wait's whole
 * milliseconds, rounded up. Remembered once the kernel says ENOSYS. */
static int scaly_eio_no_pwait2 = 0;

static int scaly_epoll_wait_ns(int q, struct epoll_event* evs, int max, long long ns)
{
    if (ns < 0)
        return epoll_wait(q, evs, max, -1);
#ifdef __NR_epoll_pwait2
    if (!scaly_eio_no_pwait2)
    {
        struct timespec ts;
        int n;
        ts.tv_sec = (time_t)(ns / 1000000000LL);
        ts.tv_nsec = (long)(ns % 1000000000LL);
        n = (int)syscall(__NR_epoll_pwait2, q, evs, max, &ts, (void*)0, (size_t)0);
        if (n >= 0 || errno != ENOSYS)
            return n;
        scaly_eio_no_pwait2 = 1;
    }
#endif
    return epoll_wait(q, evs, max, (int)((ns + 999999LL) / 1000000LL));
}

/* ns < 0 blocks until an event fires; ns >= 0 returns 0 if the timeout
 * elapses first. In nanoseconds down to the kernel (2026-09-30): a timer
 * rounded up to whole milliseconds woke a 10 ms sleep 1.9 ms late on the
 * mean (a wait that ended a hair before its deadline waited a whole
 * millisecond more) -- HttpArena's async profile is a 10 ms sleep per
 * request. */
static int scaly_eio_wait_ns(int q, void** tags, int max, long long ns)
{
    struct epoll_event evs[SCALY_EIO_MAX_EVENTS];
    struct scaly_ring* r = scaly_ring_of(q);
    int n, i;
    if (max > SCALY_EIO_MAX_EVENTS)
        max = SCALY_EIO_MAX_EVENTS;
    if (r != 0)
        return scaly_ring_wait(r, tags, max, ns);
    do
        n = scaly_epoll_wait_ns(q, evs, max, ns);
    while (n < 0 && errno == EINTR);
    for (i = 0; i < n; i++)
    {
        size_t t = (size_t)evs[i].data.u64;
        if (t & SCALY_EIO_WATCH)
        {
            unsigned int e = evs[i].events;
            if (e & (EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                t |= SCALY_EIO_WATCH_READ;
            if (e & (EPOLLOUT | EPOLLHUP | EPOLLERR))
                t |= SCALY_EIO_WATCH_WRITE;
        }
        tags[i] = (void*)t;
    }
    return n;
}

int scaly_eio_wait(int q, void** tags, int max)
{
    return scaly_eio_wait_ns(q, tags, max, -1);
}

int scaly_eio_wait_timeout(int q, void** tags, int max, int ms)
{
    return scaly_eio_wait_ns(q, tags, max, ms < 0 ? -1 : (long long)ms * 1000000LL);
}

/* The poller's wait bounded in nanoseconds (Scheduler.poll_io_by's timers). */
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns)
{
    return scaly_eio_wait_ns(q, tags, max, ns < 0 ? 0 : ns);
}

/* Cross-thread wake channel (milestone 2.4): an eventfd reporting `tag`.
 * Under epoll it is registered edge-triggered (EPOLLET, NOT oneshot): each
 * write is reported exactly once and the counter never needs a reset read.
 * On a ring it is polled (POLL_ADD, re-armed and drained at every report,
 * see scaly_ring_wait). A write before or during a wait completes that
 * wait, writes between waits coalesce into one report. Returns the eventfd
 * (the wake handle scaly_eio_wake writes to), -1 on failure.
 * scaly_eio_wake may be called from ANY thread. */
int scaly_eio_wake_create(int q, void* tag)
{
    struct epoll_event ev;
    struct scaly_ring* r = scaly_ring_of(q);
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0)
        return -1;
    if (r != 0)
    {
        r->wake_fd = fd;
        r->wake_tag = tag;
        if (scaly_ring_poll(r, fd, POLLIN, SCALY_RING_WAKE) < 0)
        {
            r->wake_fd = -1;
            close(fd);
            return -1;
        }
        return fd;
    }
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
 * darwin it is only a kqueue ident. A ring's pending poll of it ends
 * with the ring. */
int scaly_eio_wake_close(int q, int w)
{
    struct scaly_ring* r = scaly_ring_of(q);
    if (r != 0)
        r->wake_fd = -1;
    else
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
#if !defined(__APPLE__)
    scaly_ring_close(q);
#endif
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

/* SO_REUSEPORT on a socket not bound yet: several sockets on one port and
 * the kernel spreading the peers over them -- a QUIC listener per scheduler
 * thread (https.H3). Shim rule (a): the constant's value is the OS's (15 on
 * Linux, 0x200 on Darwin). 0 when set, -1 when it cannot be. */
int scaly_eio_reuseport(int fd)
{
#ifdef SO_REUSEPORT
    int one = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &one, sizeof one) == 0 ? 0 : -1;
#else
    (void)fd;
    return -1;
#endif
}

/* One datagram from a non-blocking UDP socket into buf and its sender's
 * address into addr (addrcap bytes, its length through addrlen): the byte
 * count, -2 when none is waiting, -1 on an error. Shim rule (a): struct
 * sockaddr and socklen_t are the OS's; (c): errno. The address stays an
 * opaque blob on the Scaly side (h3 hands it to ngtcp2). */
long long scaly_eio_udp_recv(int fd, void* buf, size_t len, void* addr, size_t addrcap,
                             size_t* addrlen)
{
    socklen_t al = (socklen_t)addrcap;
    ssize_t n;
    do
        n = recvfrom(fd, buf, len, 0, (struct sockaddr*)addr, &al);
    while (n < 0 && errno == EINTR);
    if (n < 0)
        return (errno == EAGAIN || errno == EWOULDBLOCK) ? -2 : -1;
    *addrlen = (size_t)al;
    return (long long)n;
}

/* One datagram to addr (a blob scaly_eio_udp_recv gave): the byte count,
 * -2 when the socket's buffer is full, -1 on an error. Rules (a), (c). */
long long scaly_eio_udp_send(int fd, const void* buf, size_t len, const void* addr, size_t addrlen)
{
    ssize_t n;
    do
        n = sendto(fd, buf, len, 0, (const struct sockaddr*)addr, (socklen_t)addrlen);
    while (n < 0 && errno == EINTR);
    if (n < 0)
        return (errno == EAGAIN || errno == EWOULDBLOCK) ? -2 : -1;
    return (long long)n;
}

/* A train of datagrams to addr: len bytes, every one segment bytes but the
 * last, which may be shorter. Linux sends it in one call (UDP GSO,
 * UDP_SEGMENT); elsewhere, or where the kernel refuses GSO, one sendto
 * each. The bytes that went (all of len, or fewer when the socket's buffer
 * filled), -1 on an error. SCALY_UDP_GSO=0 sends one at a time. Rules (a)
 * (UDP_SEGMENT and its cmsg are Linux's) and (c). */
long long scaly_eio_udp_send_train(int fd, const void* buf, size_t len, size_t segment,
                                   const void* addr, size_t addrlen)
{
    size_t off = 0;
#if defined(__linux__) && defined(UDP_SEGMENT)
    static __thread int no_gso = -1;
    if (no_gso < 0)
    {
        const char* env = getenv("SCALY_UDP_GSO");
        no_gso = env != NULL && env[0] == '0';
    }
    if (!no_gso && len > segment)
    {
        struct msghdr msg;
        struct iovec iov;
        char ctrl[CMSG_SPACE(sizeof(uint16_t))];
        struct cmsghdr* cm;
        ssize_t n;
        memset(&msg, 0, sizeof msg);
        memset(ctrl, 0, sizeof ctrl);
        iov.iov_base = (void*)buf;
        iov.iov_len = len;
        msg.msg_name = (void*)addr;
        msg.msg_namelen = (socklen_t)addrlen;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl;
        msg.msg_controllen = sizeof ctrl;
        cm = CMSG_FIRSTHDR(&msg);
        cm->cmsg_level = SOL_UDP;
        cm->cmsg_type = UDP_SEGMENT;
        cm->cmsg_len = CMSG_LEN(sizeof(uint16_t));
        *(uint16_t*)CMSG_DATA(cm) = (uint16_t)segment;
        do
            n = sendmsg(fd, &msg, 0);
        while (n < 0 && errno == EINTR);
        if (n >= 0)
            return (long long)n;
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        /* a kernel without GSO: never again; anything else (a train past
         * the 64 KiB GSO takes: EMSGSIZE): this one a datagram at a time --
         * never dropped, as ngtcp2 counts them sent */
        if (errno == EIO || errno == ENOPROTOOPT || errno == EOPNOTSUPP)
            no_gso = 1;
    }
#endif
    while (off < len)
    {
        size_t k = len - off < segment ? len - off : segment;
        ssize_t n;
        do
            n = sendto(fd, (const char*)buf + off, k, 0, (const struct sockaddr*)addr,
                       (socklen_t)addrlen);
        while (n < 0 && errno == EINTR);
        if (n < 0)
            return (errno == EAGAIN || errno == EWOULDBLOCK) ? (long long)off : -1;
        off += k;
    }
    return (long long)off;
}

/* The IPv6 loopback address ::1 with port into addr (addrcap bytes), its
 * length through len: 0, or -1. Rule (a): struct sockaddr_in6 is the OS's. */
int scaly_eio_loopback6(int port, void* addr, size_t addrcap, size_t* len)
{
    struct sockaddr_in6 a;
    if (addrcap < sizeof a)
        return -1;
    memset(&a, 0, sizeof a);
    a.sin6_family = AF_INET6;
    a.sin6_port = htons((unsigned short)port);
    a.sin6_addr = in6addr_loopback;
    memcpy(addr, &a, sizeof a);
    *len = sizeof a;
    return 0;
}

/* A socket's own address into addr (addrcap bytes), its length through
 * len: 0, or -1. Rule (a). */
int scaly_eio_sockname(int fd, void* addr, size_t addrcap, size_t* len)
{
    socklen_t al = (socklen_t)addrcap;
    if (getsockname(fd, (struct sockaddr*)addr, &al) != 0)
        return -1;
    *len = (size_t)al;
    return 0;
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
        || listen(fd, SCALY_EIO_LISTEN_BACKLOG) < 0
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

/* Is this the unspecified IPv6 address "::"? Byte by byte: the IN6_ macros
 * and in6addr_any are spelled differently on each platform. */
static int scaly_eio_in6_any(const struct sockaddr* sa)
{
    const unsigned char* b = (const unsigned char*)&((const struct sockaddr_in6*)sa)->sin6_addr;
    int i;
    for (i = 0; i < 16; i++)
        if (b[i] != 0)
            return 0;
    return 1;
}

/* Listen on host:port, host a NUMERIC address of either family ("::1",
 * "::", "127.0.0.1", "0.0.0.0" — a name would bind whichever family the
 * resolver lists first). "::" is dual-stack: IPV6_V6ONLY is switched off
 * explicitly (its default differs between systems), so the socket takes IPv4
 * connections too, as mapped addresses. Shim rule (a): struct addrinfo, the
 * AI_/AF_ constants and the IPV6_V6ONLY option are OS-specific. Returns the
 * fd, -1 on failure (a malformed address, a family the host lacks, a port in
 * use). */
int scaly_eio_tcp_listen_host(const char* host, int port)
{
    struct addrinfo hints;
    struct addrinfo* res = 0;
    struct addrinfo* ai;
    char portbuf[16];
    int fd = -1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
    snprintf(portbuf, sizeof portbuf, "%d", port);
    if (getaddrinfo(host, portbuf, &hints, &res) != 0)
        return -1;
    for (ai = res; ai; ai = ai->ai_next)
    {
        int one = 1;
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0)
            continue;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
        if (ai->ai_family == AF_INET6)
        {
            int v6only = scaly_eio_in6_any(ai->ai_addr) ? 0 : 1;
            setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof v6only);
        }
        scaly_eio_sock_init(fd);
        if (bind(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen) == 0
            && listen(fd, SCALY_EIO_LISTEN_BACKLOG) == 0
            && scaly_eio_set_nonblocking(fd) == 0)
            break;
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    return fd;
}

/* Listen on every interface of BOTH families — the dual-stack "::" — or on
 * IPv4's INADDR_ANY where the host has no IPv6 (a cluster node accepting
 * real remote peers, stage 7, milestone 7.3). */
int scaly_eio_tcp_listen_any(int port)
{
    int fd = scaly_eio_tcp_listen_host("::", port);
    if (fd >= 0)
        return fd;
    return scaly_eio_tcp_listen_at(INADDR_ANY, port);
}

/* The local port a bound socket ended up on, -1 on failure; either family. */
int scaly_eio_tcp_port(int fd)
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof addr;
    if (getsockname(fd, (struct sockaddr*)&addr, &len) < 0)
        return -1;
    if (addr.ss_family == AF_INET6)
        return ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
    return ntohs(((struct sockaddr_in*)&addr)->sin_port);
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
    /* either family, in the resolver's order ("localhost" is ::1 and
     * 127.0.0.1): the first address that connects wins */
    hints.ai_family = AF_UNSPEC;
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

/* TCP_NODELAY on or off (1/0): Nagle's coalescing of small writes. Shim
 * rule (a): the option's level and name come from <netinet/tcp.h>, and the
 * Windows twin passes a SOCKET, which is not an int there. 0 on success, -1
 * on an error (a descriptor that is not a TCP socket). */
int scaly_eio_tcp_nodelay(int fd, int on)
{
    int v = on ? 1 : 0;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof v) == 0 ? 0 : -1;
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

/* Usable CPU count — the ncpu-based worker default for task pools
 * (stage-3 milestone 3.3): the affinity mask on linux, the online CPUs
 * elsewhere. Shim rule (a): the _SC_NPROCESSORS_ONLN constant's VALUE is
 * OS-specific (darwin 58, glibc 84) and cpu_set_t's layout is glibc's, so
 * a Scaly extern cannot reach either portably — the seed ships one
 * scaly.ll for all targets. Never less than 1. */
int scaly_eio_ncpu(void)
{
    /* SCALY_WORKERS=<n> caps the default pool (the OMP_NUM_THREADS idea):
     * SCALY_WORKERS=1 is the sequential baseline a self-scaling measurement
     * compares against, without touching the program (tests/selfscale). */
    const char *w = getenv("SCALY_WORKERS");
    if (w != NULL && *w != '\0') {
        long k = strtol(w, NULL, 10);
        if (k >= 1)
            return (int)k;
    }
#ifdef __linux__
    /* The CPUs this process may RUN on, not the CPUs the machine has: a
     * container started with a cpuset (docker --cpuset-cpus, a benchmark
     * harness pinning the server to half the machine) sees every online
     * CPU through sysconf, and a scheduler per online CPU would put two
     * on each CPU it was given. darwin has no affinity masks. */
    {
        cpu_set_t set;
        CPU_ZERO(&set);
        if (sched_getaffinity(0, sizeof set, &set) == 0) {
            int k = CPU_COUNT(&set);
            if (k >= 1)
                return k;
        }
    }
#endif
    long n = sysconf(_SC_NPROCESSORS_ONLN);
    if (n < 1)
        return 1;
    return (int)n;
}

/* Sleep for us microseconds — the fork-join ticker's pace (ROADMAP 4.8,
 * route 3). Shim rule (a): struct timespec and nanosleep are POSIX, the
 * Windows twin is a high-resolution waitable timer. */
void scaly_eio_sleep_us(unsigned int us)
{
    struct timespec ts;
    ts.tv_sec = us / 1000000u;
    ts.tv_nsec = (long)(us % 1000000u) * 1000L;
    nanosleep(&ts, NULL);
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

/* What a file is on disk, for a cache that must follow it and for OpenSP's
 * read-block quantum: out[0] its size, out[1] its mtime in nanoseconds,
 * out[2] its inode, out[3] st_blksize for a regular file and 8192 otherwise
 * (PosixBaseStorageObject::getBlockSize under SP_STAT_BLKSIZE -- the
 * parser's read-block boundaries, which split data tokens, are observable
 * through the DSSSL grove). stat(path) when there is a path, else
 * fstat(fd). 0, or -1 when there is no such file (out untouched). Shim
 * rule (a): struct stat's layout, and the name of its nanosecond mtime,
 * are OS-specific. It replaces scaly_eio_blksize_path/_fd (2026-09-30). */
#include <sys/stat.h>
int scaly_eio_stat(const char *path, int fd, long long *out)
{
    struct stat sb;
    if ((path != 0 ? stat(path, &sb) : fstat(fd, &sb)) < 0)
        return -1;
    out[0] = (long long)sb.st_size;
#if defined(__APPLE__)
    out[1] = (long long)sb.st_mtimespec.tv_sec * 1000000000LL + sb.st_mtimespec.tv_nsec;
#else
    out[1] = (long long)sb.st_mtim.tv_sec * 1000000000LL + sb.st_mtim.tv_nsec;
#endif
    out[2] = (long long)sb.st_ino;
    out[3] = S_ISREG(sb.st_mode) ? (long long)sb.st_blksize : 8192;
    return 0;
}

/* The wall clock in nanoseconds since the Unix epoch -- the clock a file's
 * mtime (scaly_eio_stat) is taken from, so the two can be compared: a cache
 * asks whether a file was written so recently that a second write could carry
 * the same stamp (http's static files). Shim rule (a), as for the monotonic
 * clock: the clock id's value is OS-specific. */
long long scaly_eio_wall_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000LL + ts.tv_nsec;
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

/* THE SIZED THREAD SPAWN, shim category (a): pthread_attr_t is an opaque,
 * OS-specific block, so the stack size cannot be set from the Scaly side of
 * a direct pthread_create extern. A thread a Scaly program spawns runs what
 * its spawner runs -- tscaly's checker pool ran the checker on workers with
 * macOS's 512 KB secondary-thread default while the main thread had 64 MB,
 * and the TypeScript compiler's own sources overflowed it on the first file
 * (2026-09-21). stack_size 0 keeps the platform default; the handle comes
 * back as the size_t the Scaly side carries pthread_t in.
 */
#include <pthread.h>

int scaly_thread_spawn_sized(size_t* thread, void* start, void* arg, size_t stack_size)
{
    pthread_t t;
    pthread_attr_t attr;
    int rc;
    if (pthread_attr_init(&attr) != 0)
        return -1;
    if (stack_size > 0 && pthread_attr_setstacksize(&attr, stack_size) != 0) {
        pthread_attr_destroy(&attr);
        return -1;
    }
    rc = pthread_create(&t, &attr, (void* (*)(void*))start, arg);
    pthread_attr_destroy(&attr);
    if (rc != 0)
        return rc;
    *thread = (size_t)t;
    return 0;
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

/* ---- DIRECTORY ENTRIES, shim category (a) --------------------------------
 *
 * struct dirent's layout, and whether d_type is filled at all, differ per
 * libc, so the Scaly side cannot read an entry through a direct readdir
 * extern. It asks for the NEXT NAME instead: scaly_eio_dir_next writes it
 * NUL-terminated into buf, answers its length and sets *is_dir; -1 is the
 * end. "." and ".." are never answered, and a name that does not fit cap is
 * skipped. The handle is the DIR* itself. First user: the one tool finding a
 * package's files (ROADMAP-public.md, stage A).
 */
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

void* scaly_eio_dir_open(const char* path)
{
    return (void*)opendir(path);
}

long long scaly_eio_dir_next(void* dir, char* buf, size_t cap, int* is_dir)
{
    DIR* d = (DIR*)dir;
    struct dirent* e;
    while ((e = readdir(d)) != NULL) {
        size_t n = strlen(e->d_name);
        if (e->d_name[0] == '.' && (n == 1 || (n == 2 && e->d_name[1] == '.')))
            continue;
        if (n + 1 > cap)
            continue;
        memcpy(buf, e->d_name, n + 1);
        int kind = -1;
#ifdef DT_DIR
        if (e->d_type == DT_DIR)
            kind = 1;
        else if (e->d_type == DT_REG)
            kind = 0;
#endif
        if (kind < 0) {
            struct stat st;
            kind = fstatat(dirfd(d), e->d_name, &st, 0) == 0 && S_ISDIR(st.st_mode) ? 1 : 0;
        }
        *is_dir = kind;
        return (long long)n;
    }
    return -1;
}

int scaly_eio_dir_close(void* dir)
{
    return closedir((DIR*)dir);
}

/* ---- LOADING A C LIBRARY INTO THE PROCESS, shim category (a) --------------
 *
 * `scaly run` / `scaly test` JIT a program whose packages name C libraries
 * (`extern ssl, crypto`); their symbols must be visible to the JIT's lookup in
 * this process, so each is loaded RTLD_GLOBAL -- a constant whose value
 * differs per libc (0x8 on macOS, 0x100 on glibc). 0 for loaded, -1 not.
 */
#include <dlfcn.h>

int scaly_eio_load_library(const char* path)
{
    return dlopen(path, RTLD_NOW | RTLD_GLOBAL) != NULL ? 0 : -1;
}

/* ---- A DOUBLE AS TEXT, shim category (b) -----------------------------------
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

/* ---- THE TERMINAL'S RAW MODE, shim category (a) ----------------------------
 *
 * The REPL edits a line itself (arrows, history), which needs the terminal
 * in raw mode while it reads: struct termios and its flags differ per libc.
 * raw saves the mode it found and answers 0 (-1 when fd is no terminal);
 * restore puts the saved mode back. Signals stay the REPL's own: Ctrl-C
 * arrives as byte 3 while a line is edited, and the terminal's mode is back
 * to normal whenever an entry runs.
 */
#include <termios.h>
#include <unistd.h>

static struct termios scaly_term_saved;
static int scaly_term_raw_on = 0;

int scaly_eio_term_isatty(int fd)
{
    return isatty(fd) ? 1 : 0;
}

int scaly_eio_term_raw(int fd)
{
    struct termios raw;
    if (!isatty(fd) || tcgetattr(fd, &scaly_term_saved) != 0)
        return -1;
    raw = scaly_term_saved;
    raw.c_iflag &= ~(tcflag_t)(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(tcflag_t)(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, &raw) != 0)
        return -1;
    scaly_term_raw_on = 1;
    return 0;
}

int scaly_eio_term_restore(int fd)
{
    if (!scaly_term_raw_on)
        return 0;
    scaly_term_raw_on = 0;
    return tcsetattr(fd, TCSAFLUSH, &scaly_term_saved) == 0 ? 0 : -1;
}

/* ---- A CHILD THAT RUNS THIS PROGRAM AGAIN, shim category (a) --------------
 *
 * A program that isolates dangerous work in a child (scalyls' worker) starts
 * ITSELF again, with a mode word and the two pipe ends as arguments; `main`
 * asks scaly_proc_worker_fds whether it is that child. One model for every
 * target: here it is fork + execv, on Windows CreateProcess (eio_windows.c) —
 * Windows has no fork, and a forked child that carries on WITHOUT exec keeps
 * a copy of a process whose other threads do not exist in it.
 *
 * In C because finding one's own executable is per OS (_NSGetExecutablePath,
 * /proc/self/exe, GetModuleFileName), and because the child's handles are
 * file descriptors here and HANDLEs there, so the Scaly side asks questions:
 *
 *   scaly_proc_spawn_self   start the child; its pid, the parent's write end
 *                           (requests) and read end (answers). 0, or -1.
 *                           The child's stdin and stdout are the null device:
 *                           it talks over the pipes only, and a stray print
 *                           must not reach the parent's own stdout. stderr is
 *                           inherited (crash output).
 *   scaly_proc_worker_fds   1 when this process is such a child (argv[1] is
 *                           `mode`), with its read and write end; else 0.
 *   scaly_proc_reap         kill the child and collect it.
 *   scaly_proc_wait_readable  poll(2) on ONE descriptor: > 0 readable or the
 *                           other side is gone, 0 timeout, < 0 error. A
 *                           question of its own because Windows' poll answers
 *                           for sockets only (posixcompat_windows.c).
 *   scaly_proc_stdio_binary nothing here; Windows opens stdin and stdout in
 *                           text mode, which rewrites the line ends of a
 *                           framed protocol.
 *   scaly_eio_is_symlink    1 when the path itself is a symbolic link — a
 *                           tree walk that must not follow one asks before it
 *                           descends (scaly_eio_dir_next answers is_dir for
 *                           the link's TARGET).
 */
#include <poll.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

static int scaly_proc_self_path(char* buf, size_t cap)
{
#ifdef __APPLE__
    uint32_t n = (uint32_t)cap;
    return _NSGetExecutablePath(buf, &n) == 0 ? 0 : -1;
#else
    ssize_t n = readlink("/proc/self/exe", buf, cap - 1);
    if (n <= 0)
        return -1;
    buf[n] = 0;
    return 0;
#endif
}

int scaly_proc_spawn_self(const char* mode, long long* out_pid, int* out_write_fd, int* out_read_fd)
{
    char self[4096];
    char a_in[16];
    char a_out[16];
    int p2c[2];
    int c2p[2];
    pid_t pid;

    if (scaly_proc_self_path(self, sizeof self) != 0)
        return -1;
    /* writing to a dead child must be an error, not the parent's death */
    signal(SIGPIPE, SIG_IGN);
    if (pipe(p2c) != 0)
        return -1;
    if (pipe(c2p) != 0) {
        close(p2c[0]);
        close(p2c[1]);
        return -1;
    }
    snprintf(a_in, sizeof a_in, "%d", p2c[0]);
    snprintf(a_out, sizeof a_out, "%d", c2p[1]);

    pid = fork();
    if (pid < 0) {
        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        return -1;
    }
    if (pid == 0) {
        /* between fork and exec: async-signal-safe calls only */
        char* argv[5];
        int nul = open("/dev/null", O_RDWR);
        if (nul >= 0) {
            dup2(nul, 0);
            dup2(nul, 1);
            if (nul > 1)
                close(nul);
        }
        close(p2c[1]);
        close(c2p[0]);
        argv[0] = self;
        argv[1] = (char*)mode;
        argv[2] = a_in;
        argv[3] = a_out;
        argv[4] = NULL;
        execv(self, argv);
        _exit(127);
    }
    close(p2c[0]);
    close(c2p[1]);
    *out_pid = (long long)pid;
    *out_write_fd = p2c[1];
    *out_read_fd = c2p[0];
    return 0;
}

int scaly_proc_worker_fds(long long argc, char** argv, const char* mode, int* in_fd, int* out_fd)
{
    if (argc < 4 || strcmp(argv[1], mode) != 0)
        return 0;
    *in_fd = atoi(argv[2]);
    *out_fd = atoi(argv[3]);
    return 1;
}

int scaly_proc_reap(long long pid)
{
    int status = 0;
    kill((pid_t)pid, SIGKILL);
    return waitpid((pid_t)pid, &status, 0) < 0 ? -1 : 0;
}

int scaly_proc_wait_readable(int fd, int timeout_ms)
{
    struct pollfd p;
    p.fd = fd;
    p.events = POLLIN;
    p.revents = 0;
    return poll(&p, 1, timeout_ms);
}

void scaly_proc_stdio_binary(void)
{
}

/* The JIT's memory arena is a Windows question (eio_windows.c): POSIX JIT
 * memory comes from mmap near itself, and ORC's default manager stays. */
void scaly_jit_use_arena(void* builder)
{
    (void)builder;
}

/* The file of the running program, symbolic links resolved, as
 * cli.adopt_home asks it: a handed-out program finds its packages beside
 * itself. Rule (a): _NSGetExecutablePath on macOS (which may answer a path
 * through a link, hence realpath), /proc/self/exe on Linux,
 * GetModuleFileName in eio_windows.c. The length, or -1 for "not known"
 * (another system, or a path that does not fit) -- the caller then resolves
 * as it always did. */
#if defined(__APPLE__)
#include <limits.h>
#include <mach-o/dyld.h>
#endif
int scaly_eio_self_path(char* buffer, size_t capacity)
{
#if defined(__APPLE__)
    char raw[PATH_MAX];
    char real[PATH_MAX];
    uint32_t size = (uint32_t)sizeof raw;
    if (_NSGetExecutablePath(raw, &size) != 0 || realpath(raw, real) == NULL)
        return -1;
    size_t n = strlen(real);
    if (n == 0 || n >= capacity)
        return -1;
    memcpy(buffer, real, n + 1);
    return (int)n;
#elif defined(__linux__)
    ssize_t n = capacity > 0 ? readlink("/proc/self/exe", buffer, capacity) : -1;
    if (n <= 0 || (size_t)n >= capacity)
        return -1;
    buffer[n] = 0;
    return (int)n;
#else
    (void)buffer; (void)capacity;
    return -1;
#endif
}

int scaly_eio_is_symlink(const char* path)
{
    struct stat st;
    return lstat(path, &st) == 0 && S_ISLNK(st.st_mode) ? 1 : 0;
}
