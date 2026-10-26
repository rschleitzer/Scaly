/* The runtime's OS questions on WebAssembly (wasm32-wasip1), the twin of
 * eio.c and eio_windows.c: the tool takes it by its name's `_wasm` where the
 * target is wasm, as it takes eio_windows.c on Windows.
 *
 * The wasm profile is one core and one thread, without fibers: WASI preview 1
 * has no threads, no signals, no sockets a program may open, no poller and no
 * child processes. So the answers are of three kinds:
 *
 *   - the same as POSIX through WASI: read and write (blocking -- nothing here
 *     is non-blocking, so no call ever answers -2), the clocks, sleeping, a
 *     file's facts, directories, a FILE's position, aligned allocation, a
 *     double as text, whether a descriptor is a terminal;
 *   - the honest refusal: the evented-I/O queue, every socket, a thread, a
 *     child process, a library to load answer -1 (or nothing), which is what
 *     their callers already handle where an OS call fails;
 *   - the question that has a fixed answer: one CPU (so a `for` runs inline and
 *     no task pool starts a thread), no stack limit to tell (0), no guard page.
 *
 * Every function of eio.c is here, so whatever the standard library names
 * links; shim rules (a)-(c) are eio.c's.
 */
#include <dirent.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

/* ---- the working directory ------------------------------------------------ */

/* WASI hands a program no working directory: wasi-libc begins at `/`, so a
 * relative path is read from the root of the first directory the host opened
 * for it. A host that means one says so in PWD (tests/wasm/run.mjs passes its
 * own), and the program begins there -- `..` included, which an opened `.`
 * cannot reach. Without PWD (a page that hands in its files by absolute path)
 * the root stays. A constructor, so it runs before main and before a static
 * initializer of the program; this file is in every program, because the
 * runtime's pages come from scaly_aligned_alloc below. Rule (a). */
__attribute__((constructor)) static void scaly_wasm_adopt_pwd(void)
{
    const char* pwd = getenv("PWD");
    if (pwd && pwd[0] == '/')
        (void)chdir(pwd);
}

/* ---- the process --------------------------------------------------------- */

/* WASI has no process ids, and wasi-libc declares getpid only behind its
 * emulation (_WASI_EMULATED_GETPID). A program that names one in a file name
 * (dazzle's temporary output) gets a fixed one, as the emulation would give;
 * posixcompat_windows.c is the Windows twin. Rule (a). */
int getpid(void) { return 1; }

/* ---- the evented-I/O queue: there is none ------------------------------- */

int scaly_eio_create(void) { return -1; }
int scaly_eio_arm(int q, int fd, int for_write, void* tag) { (void)q; (void)fd; (void)for_write; (void)tag; return -1; }
int scaly_eio_watch(int q, int fd, void* tag) { (void)q; (void)fd; (void)tag; return -1; }
int scaly_eio_wait(int q, void** tags, int max) { (void)q; (void)tags; (void)max; return -1; }
int scaly_eio_wait_timeout(int q, void** tags, int max, int ms) { (void)q; (void)tags; (void)max; (void)ms; return -1; }
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns) { (void)q; (void)tags; (void)max; (void)ns; return -1; }
int scaly_eio_wake_create(int q, void* tag) { (void)q; (void)tag; return -1; }
int scaly_eio_wake(int q, int w) { (void)q; (void)w; return -1; }
int scaly_eio_wake_close(int q, int w) { (void)q; (void)w; return 0; }
int scaly_eio_submit_recv(int q, int fd, void* buf, unsigned long long count, void* tag) { (void)q; (void)fd; (void)buf; (void)count; (void)tag; return -1; }
int scaly_eio_submit_send(int q, int fd, void* buf, unsigned long long count, void* tag) { (void)q; (void)fd; (void)buf; (void)count; (void)tag; return -1; }
int scaly_eio_submit_send_recv(int q, int fd, void* sbuf, unsigned long long scount,
                               void* rbuf, unsigned long long rcount, void* tag)
{
    (void)q; (void)fd; (void)sbuf; (void)scount; (void)rbuf; (void)rcount; (void)tag;
    return -1;
}
int scaly_eio_completions(int q) { (void)q; return 0; }
int scaly_eio_backend(int q) { (void)q; return 0; }
int scaly_eio_close(int q) { return close(q); }
int scaly_eio_set_nonblocking(int fd) { (void)fd; return -1; }

/* ---- reading and writing a descriptor, blocking ------------------------- */

/* The host may hand a descriptor over non-blocking: node does with a pipe on
 * standard output, so a write past the pipe's 64 KB answers EAGAIN, and a
 * caller that takes a short count for an error (Io.write_all) lost the rest
 * of the output. The profile promises blocking calls, so a call that would
 * block waits in poll -- which WASI's poll_oneoff answers for a descriptor --
 * and is made again. */
static int scaly_wasm_would_block(int fd, short events)
{
    if (errno != EAGAIN && errno != EWOULDBLOCK)
        return 0;
    struct pollfd p = { fd, events, 0 };
    return poll(&p, 1, -1) >= 0 || errno == EINTR;
}

long long scaly_eio_read(int fd, void* buf, unsigned long long count)
{
    long long r;
    do
        r = read(fd, buf, (size_t)count);
    while (r < 0 && (errno == EINTR || scaly_wasm_would_block(fd, POLLIN)));
    return r;
}

/* (c) errno is a thread-local macro in wasi-libc too */
int scaly_eio_errno(void)
{
    return errno;
}

long long scaly_eio_write(int fd, const void* buf, unsigned long long count)
{
    long long r;
    do
        r = write(fd, buf, (size_t)count);
    while (r < 0 && (errno == EINTR || scaly_wasm_would_block(fd, POLLOUT)));
    return r;
}

/* ---- sockets: WASI preview 1 lets a program open none --------------------- */

int scaly_eio_reuseport(int fd) { (void)fd; return -1; }
long long scaly_eio_udp_recv(int fd, void* buf, unsigned long long len, void* addr, unsigned long long addrcap,
                             unsigned long long* addrlen)
{
    (void)fd; (void)buf; (void)len; (void)addr; (void)addrcap; (void)addrlen;
    return -1;
}
long long scaly_eio_udp_send(int fd, const void* buf, unsigned long long len, const void* addr, unsigned long long addrlen)
{
    (void)fd; (void)buf; (void)len; (void)addr; (void)addrlen;
    return -1;
}
long long scaly_eio_udp_send_train(int fd, const void* buf, unsigned long long len, unsigned long long segment,
                                   const void* addr, unsigned long long addrlen)
{
    (void)fd; (void)buf; (void)len; (void)segment; (void)addr; (void)addrlen;
    return -1;
}
int scaly_eio_loopback6(int port, void* addr, unsigned long long addrcap, unsigned long long* len)
{
    (void)port; (void)addr; (void)addrcap; (void)len;
    return -1;
}
int scaly_eio_sockname(int fd, void* addr, unsigned long long addrcap, unsigned long long* len)
{
    (void)fd; (void)addr; (void)addrcap; (void)len;
    return -1;
}
int scaly_eio_tcp_listen(int port) { (void)port; return -1; }
int scaly_eio_tcp_listen_host(const char* host, int port) { (void)host; (void)port; return -1; }
int scaly_eio_tcp_listen_any(int port) { (void)port; return -1; }
int scaly_eio_tcp_port(int fd) { (void)fd; return -1; }
int scaly_eio_tcp_connect(int port) { (void)port; return -1; }
int scaly_eio_tcp_connect_host(const char* host, int port) { (void)host; (void)port; return -1; }
long long scaly_eio_tcp_write(int fd, const void* buf, unsigned long long count) { (void)fd; (void)buf; (void)count; return -1; }
int scaly_eio_tcp_nodelay(int fd, int on) { (void)fd; (void)on; return -1; }
void scaly_eio_tcp_served(int fd) { (void)fd; }
void scaly_eio_tcp_unserved(int fd) { (void)fd; }
int scaly_eio_tcp_wake_served(void) { return 0; }
/* no connection to bound, so no sweeper to start and none left to sweep */
int scaly_eio_tcp_idle(int fd, long long seconds) { (void)fd; (void)seconds; return 0; }
int scaly_eio_tcp_sweep(void) { return 0; }
int scaly_eio_accept(int fd) { (void)fd; return -1; }
int scaly_eio_accept_nb(int fd) { (void)fd; return -1; }
int scaly_eio_tcp_listen_twin(int fd) { (void)fd; return -1; }
int scaly_eio_listen_end(int fd) { (void)fd; return -1; }

/* ---- signals: there are none, so nothing terminates a waiter -------------- */

void scaly_eio_block_termination(void) {}
int scaly_eio_wait_termination(void) { return -1; }

/* no guard page: wasm has no page protection a fiber stack could use, and
 * there are no fibers */
int scaly_stack_guard(void* base, unsigned long long len) { (void)base; (void)len; return -1; }
int scaly_guard_install(int (*classify)(void*)) { (void)classify; return -1; }

/* ---- the processor and the clocks ------------------------------------------ */

/* One CPU: the profile has no threads, so every parallel `for` runs inline
 * and no task pool starts a worker. SCALY_WORKERS is not asked. */
int scaly_eio_ncpu(void) { return 1; }

void scaly_eio_sleep_us(unsigned int us)
{
    struct timespec ts;
    ts.tv_sec = us / 1000000u;
    ts.tv_nsec = (long)(us % 1000000u) * 1000L;
    nanosleep(&ts, NULL);
}

long long scaly_eio_now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

long long scaly_eio_wall_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/* ---- files ------------------------------------------------------------------ */

/* as eio.c: size, mtime in ns, inode, the read block (st_blksize for a
 * regular file, 8192 otherwise). WASI's filestat carries no block size, so
 * wasi-libc answers st_blksize 0: that is "none" too, and 8192 -- a reader
 * stepping through a file by its block would not move with 0. */
int scaly_eio_stat(const char *path, int fd, long long *out)
{
    struct stat sb;
    if ((path != 0 ? stat(path, &sb) : fstat(fd, &sb)) < 0)
        return -1;
    out[0] = (long long)sb.st_size;
    out[1] = (long long)sb.st_mtim.tv_sec * 1000000000LL + sb.st_mtim.tv_nsec;
    out[2] = (long long)sb.st_ino;
    out[3] = S_ISREG(sb.st_mode) && sb.st_blksize > 0 ? (long long)sb.st_blksize : 8192;
    return 0;
}

long long scaly_eio_tell(void* stream)
{
    return (long long)ftello((FILE*)stream);
}

long long scaly_eio_seek(void* stream, long long offset, long long whence)
{
    return (long long)fseeko((FILE*)stream, (off_t)offset, (int)whence);
}

int scaly_eio_is_symlink(const char* path)
{
    struct stat st;
    return lstat(path, &st) == 0 && S_ISLNK(st.st_mode) ? 1 : 0;
}

void* scaly_eio_dir_open(const char* path)
{
    return (void*)opendir(path);
}

long long scaly_eio_dir_next(void* dir, char* buf, unsigned long long cap, int* is_dir)
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
        if (e->d_type == DT_DIR)
            kind = 1;
        else if (e->d_type == DT_REG)
            kind = 0;
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

/* ---- the stack, threads, memory ------------------------------------------- */

/* 0: cannot tell -- the linker's stack size is not something WASI reports */
long long scaly_stack_limit(void) { return 0; }

/* no threads in this profile: a spawn fails as pthread_create does where it
 * cannot start one, and the caller reports it (Thread.spawn's trap) */
int scaly_thread_spawn_sized(unsigned long long* thread, void* start, void* arg, unsigned long long stack_size)
{
    (void)thread; (void)start; (void)arg; (void)stack_size;
    return -1;
}

/* the out-of-memory fault injection, as eio.c */
static long long scaly_alloc_fail_left = -1;

void scaly_alloc_fail_after(long long n)
{
    scaly_alloc_fail_left = n;
}

void* scaly_aligned_alloc(unsigned long long alignment, unsigned long long size)
{
    if (scaly_alloc_fail_left >= 0) {
        if (scaly_alloc_fail_left == 0) {
            scaly_alloc_fail_left = -1;
            return 0;
        }
        scaly_alloc_fail_left--;
    }
    return aligned_alloc((size_t)alignment, (size_t)size);
}

void scaly_aligned_free(void* p)
{
    free(p);
}

/* ---- the rest of eio.c's questions ------------------------------------------ */

int scaly_eio_load_library(const char* path) { (void)path; return -1; }

/* (b) snprintf is variadic: the shortest %g that reads back, as eio.c */
int scaly_eio_format_double(char* buf, unsigned long long cap, double value)
{
    int precision;
    int n = 0;
    for (precision = 1; precision <= 17; precision++) {
        n = snprintf(buf, (size_t)cap, "%.*g", precision, value);
        if (n < 0 || (unsigned long long)n >= cap)
            return 0;
        if (strtod(buf, NULL) == value)
            break;
    }
    return n;
}

/* a terminal can be asked about, not put in raw mode (no termios) */
int scaly_eio_term_isatty(int fd) { return isatty(fd) ? 1 : 0; }
int scaly_eio_term_raw(int fd) { (void)fd; return -1; }
int scaly_eio_term_restore(int fd) { (void)fd; return 0; }

/* no child processes */
int scaly_proc_spawn_self(const char* mode, long long* out_pid, int* out_write_fd, int* out_read_fd)
{
    (void)mode; (void)out_pid; (void)out_write_fd; (void)out_read_fd;
    return -1;
}

int scaly_proc_worker_fds(long long argc, char** argv, const char* mode, int* in_fd, int* out_fd)
{
    (void)argc; (void)argv; (void)mode; (void)in_fd; (void)out_fd;
    return 0;
}

int scaly_proc_reap(long long pid) { (void)pid; return -1; }
int scaly_proc_wait_readable(int fd, int timeout_ms) { (void)fd; (void)timeout_ms; return -1; }
void scaly_proc_stdio_binary(void) {}
void scaly_jit_use_arena(void* builder) { (void)builder; }

/* the program's own file is not known in a WASI module */
int scaly_eio_self_path(char* buffer, unsigned long long capacity)
{
    (void)buffer; (void)capacity;
    return -1;
}
