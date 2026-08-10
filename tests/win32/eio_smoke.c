/* Functional smoke test for the Win64 IOCP backend (stage 7, brocken 2).
 *
 * WHY THIS EXISTS. The IOCP backend is the one piece of the Windows port that
 * had to manufacture semantics rather than translate them — kqueue and epoll
 * report READINESS, IOCP reports COMPLETION — and it was written on a host
 * that cannot run a line of it. Assembling and compiling prove only that the
 * API calls exist. This harness proves the backend actually WORKS, and it does
 * so without the Scaly runtime: it links exactly two objects (eio_win.o and
 * posixcompat.o) plus this file, so it can run in CI long before enough of the
 * substrate exists to link a Scaly program. The riskiest artifact of the port
 * thereby becomes the earliest tested one.
 *
 * It exercises the three arming shapes that differ from each other on
 * Windows — AcceptEx for a listener, zero-byte WSARecv for a connected socket,
 * PostQueuedCompletionStatus for a cross-thread wake — plus the timeout path
 * and the byte transfer. A failure prints which step failed and exits nonzero.
 */

#include <stdio.h>
#include <string.h>

/* The shim's API; there is no header for it (the consumers are Scaly extern
 * declarations), so the prototypes are restated here. They must match
 * eio_win.c exactly — a mismatch is precisely what this test should catch. */
int       scaly_eio_create(void);
int       scaly_eio_arm(int q, int fd, int for_write, void* tag);
int       scaly_eio_wait(int q, void** tags, int max);
int       scaly_eio_wait_timeout(int q, void** tags, int max, int ms);
int       scaly_eio_wake_create(int q, void* tag);
int       scaly_eio_wake(int q, int w);
int       scaly_eio_wake_close(int q, int w);
int       scaly_eio_set_nonblocking(int fd);
long long scaly_eio_read(int fd, void* buf, unsigned long long count);
long long scaly_eio_write(int fd, const void* buf, unsigned long long count);
int       scaly_eio_tcp_listen(int port);
int       scaly_eio_tcp_port(int fd);
int       scaly_eio_tcp_connect(int port);
int       scaly_eio_accept(int fd);
int       scaly_eio_ncpu(void);
long long scaly_eio_now_ns(void);

/* From the POSIX-compat shim — exercised here too, since the wake is only
 * meaningful across threads. */
int pthread_create(unsigned long long* t, void* attr, void* start, void* arg);
int pthread_join(unsigned long long t, void* retval);

static int failures;

static void check(int ok, const char* what)
{
    printf("  %-46s %s\n", what, ok ? "ok" : "FAIL");
    if (!ok)
        failures++;
}

#define TAG_ACCEPT ((void*)0x1001)
#define TAG_READ   ((void*)0x1002)
#define TAG_WAKE   ((void*)0x1003)

static int wake_q, wake_w;

static void* waker(void* arg)
{
    (void)arg;
    scaly_eio_wake(wake_q, wake_w);
    return 0;
}

int main(void)
{
    void* tags[8];
    int q, listener, client, server, port, n, w;
    char buf[16];
    long long r, t0, t1;
    unsigned long long th;

    printf("eio_smoke (Win64 IOCP backend)\n");

    q = scaly_eio_create();
    check(q > 0, "scaly_eio_create");

    listener = scaly_eio_tcp_listen(0);
    check(listener > 0, "tcp_listen(0) on loopback");

    port = scaly_eio_tcp_port(listener);
    check(port > 0, "tcp_port reports the ephemeral port");

    /* AcceptEx must be posted BEFORE the connection arrives — that is the
     * whole difference from a readiness backend, where you arm and then call
     * accept(). */
    check(scaly_eio_arm(q, listener, 0, TAG_ACCEPT) == 0, "arm(listener) -> AcceptEx");

    client = scaly_eio_tcp_connect(port);
    check(client > 0, "tcp_connect to it");

    n = scaly_eio_wait(q, tags, 8);
    check(n == 1 && tags[0] == TAG_ACCEPT, "wait reports the accept tag");

    server = scaly_eio_accept(listener);
    check(server > 0, "accept takes the AcceptEx socket");

    /* Zero-byte WSARecv readiness: the arm must NOT consume the bytes, so the
     * subsequent read has to see all of them. */
    check(scaly_eio_arm(q, server, 0, TAG_READ) == 0, "arm(server) -> zero-byte WSARecv");

    r = scaly_eio_write(client, "hello", 5);
    check(r == 5, "write 5 bytes from the client");

    n = scaly_eio_wait(q, tags, 8);
    check(n == 1 && tags[0] == TAG_READ, "wait reports the read tag");

    memset(buf, 0, sizeof buf);
    r = scaly_eio_read(server, buf, sizeof buf);
    check(r == 5 && memcmp(buf, "hello", 5) == 0,
          "the arm did not consume the bytes");

    /* Cross-thread wake: PostQueuedCompletionStatus with a NULL OVERLAPPED is
     * how wait() tells a wake from a completion. */
    w = scaly_eio_wake_create(q, TAG_WAKE);
    check(w > 0, "wake_create");
    wake_q = q;
    wake_w = w;
    check(pthread_create(&th, 0, (void*)waker, 0) == 0, "spawn a waking thread");
    n = scaly_eio_wait(q, tags, 8);
    check(n == 1 && tags[0] == TAG_WAKE, "wait reports the wake tag");
    check(pthread_join(th, 0) == 0, "join the waking thread");
    check(scaly_eio_wake_close(q, w) == 0, "wake_close");

    /* Nothing armed: the timeout path must return 0, not block and not fail. */
    t0 = scaly_eio_now_ns();
    n = scaly_eio_wait_timeout(q, tags, 8, 50);
    t1 = scaly_eio_now_ns();
    check(n == 0, "wait_timeout returns 0 on timeout");
    check(t1 - t0 >= 30000000LL, "and it actually waited (>= 30 ms)");

    check(scaly_eio_set_nonblocking(listener) == 0, "set_nonblocking");
    check(scaly_eio_accept(listener) == -2, "accept reports -2 when idle");

    check(scaly_eio_ncpu() > 0, "ncpu");
    check(scaly_eio_now_ns() > 0, "now_ns");

    printf(failures ? "eio_smoke: %d FAIL\n" : "eio_smoke: PASS\n", failures);
    return failures ? 1 : 0;
}
