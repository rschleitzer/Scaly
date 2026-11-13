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
 *
 * THE OTHER PROFILE: wasm32-wasip1-threads (wasi-threads: a shared memory, a
 * thread a new instance of the module in a worker of the host). Compiled for
 * it -- clang defines __wasm_atomics__ there -- this file answers five of
 * the questions differently, in the section `threads` at its end: the CPUs
 * are the host's (SCALY_CPUS, which the host sets; SCALY_WORKERS caps them as
 * everywhere), a thread starts (wasi-libc's pthreads), a pipe is one in this
 * program's memory -- WASI has none, and the task pool's workers wait on
 * one --, a TCP socket is the host's (a node of a cluster in a page: the
 * import module scaly_net), and the evented-I/O queue is one of wakes and
 * readiness that a thread's main context waits on (a channel, a socket).
 * Its close goes through __wrap_close (the link wraps `close` for this
 * target). Everything else is the one-thread profile's: a thread does no
 * file I/O, and a parallel loop's body performs none; no task ever parks.
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

/* (The threads profile has one, for what a cluster's node waits on: the
 * section `threads` at the end.) */
#ifndef __wasm_atomics__
int scaly_eio_create(void) { return -1; }
int scaly_eio_arm(int q, int fd, int for_write, void* tag) { (void)q; (void)fd; (void)for_write; (void)tag; return -1; }
int scaly_eio_wait(int q, void** tags, int max) { (void)q; (void)tags; (void)max; return -1; }
int scaly_eio_wait_timeout(int q, void** tags, int max, int ms) { (void)q; (void)tags; (void)max; (void)ms; return -1; }
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns) { (void)q; (void)tags; (void)max; (void)ns; return -1; }
int scaly_eio_wake_create(int q, void* tag) { (void)q; (void)tag; return -1; }
int scaly_eio_wake(int q, int w) { (void)q; (void)w; return -1; }
int scaly_eio_wake_close(int q, int w) { (void)q; (void)w; return 0; }
#endif
int scaly_eio_watch(int q, int fd, void* tag) { (void)q; (void)fd; (void)tag; return -1; }
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
#ifndef __wasm_atomics__
int scaly_eio_close(int q) { return close(q); }
#endif
#ifndef __wasm_atomics__
int scaly_eio_set_nonblocking(int fd) { (void)fd; return -1; }
#endif

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

#ifdef __wasm_atomics__
static int scaly_wasm_is_pipe(int fd);
static long long scaly_wasm_pipe_read(int fd, void* buf, unsigned long long count);
static long long scaly_wasm_pipe_write(int fd, const void* buf, unsigned long long count);
static int scaly_wasm_is_net(int fd);
static long long scaly_wasm_net_read(int fd, void* buf, unsigned long long count);
static long long scaly_wasm_net_write(int fd, const void* buf, unsigned long long count);
#endif

long long scaly_eio_read(int fd, void* buf, unsigned long long count)
{
#ifdef __wasm_atomics__
    if (scaly_wasm_is_pipe(fd))
        return scaly_wasm_pipe_read(fd, buf, count);
    if (scaly_wasm_is_net(fd))
        return scaly_wasm_net_read(fd, buf, count);
#endif
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
#ifdef __wasm_atomics__
    if (scaly_wasm_is_pipe(fd))
        return scaly_wasm_pipe_write(fd, buf, count);
    if (scaly_wasm_is_net(fd))
        return scaly_wasm_net_write(fd, buf, count);
#endif
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
/* (The threads profile has TCP through its host: the section `threads` at
 * the end.) */
/* (both profiles) */
/* a cluster's helpers: every peer of the page's network is on this
 * machine; finding each other goes through the page (not yet: -1) */
int scaly_eio_peer_is_local(int fd) { (void)fd; return 1; }
int scaly_eio_random(void* buf, unsigned long long len)
{
    unsigned char* p = (unsigned char*)buf;
    while (len > 0) {
        size_t k = len < 256 ? (size_t)len : 256;
        if (getentropy(p, k) != 0)
            return -1;
        p += k;
        len -= k;
    }
    return 0;
}
int scaly_eio_mcast_listen(int port, int local_only) { (void)port; (void)local_only; return -1; }
long long scaly_eio_mcast_send(const void* buf, unsigned long long len, int port, int local_only) { (void)buf; (void)len; (void)port; (void)local_only; return -1; }
long long scaly_eio_mcast_recv(int fd, void* buf, unsigned long long cap, int ms, char* from, unsigned long long from_cap) { (void)fd; (void)buf; (void)cap; (void)ms; (void)from; (void)from_cap; return -1; }
#ifndef __wasm_atomics__
int scaly_eio_tcp_listen(int port) { (void)port; return -1; }
int scaly_eio_tcp_listen_host(const char* host, int port) { (void)host; (void)port; return -1; }
int scaly_eio_tcp_listen_any(int port) { (void)port; return -1; }
int scaly_eio_tcp_port(int fd) { (void)fd; return -1; }
int scaly_eio_tcp_connect(int port) { (void)port; return -1; }
int scaly_eio_tcp_connect_host(const char* host, int port) { (void)host; (void)port; return -1; }
long long scaly_eio_tcp_write(int fd, const void* buf, unsigned long long count) { (void)fd; (void)buf; (void)count; return -1; }
int scaly_eio_tcp_nodelay(int fd, int on) { (void)fd; (void)on; return -1; }
int scaly_eio_accept(int fd) { (void)fd; return -1; }
int scaly_eio_accept_nb(int fd) { (void)fd; return -1; }
#endif
void scaly_eio_tcp_served(int fd) { (void)fd; }
void scaly_eio_tcp_unserved(int fd) { (void)fd; }
int scaly_eio_tcp_wake_served(void) { return 0; }
/* no connection to bound, so no sweeper to start and none left to sweep */
int scaly_eio_tcp_idle(int fd, long long seconds) { (void)fd; (void)seconds; return 0; }
int scaly_eio_tcp_sweep(void) { return 0; }
void scaly_eio_tcp_sweeper(void *task) { (void)task; }
void *scaly_eio_tcp_idle_done(void) { return 0; }
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
 * and no task pool starts a worker. SCALY_WORKERS is not asked. (With
 * threads: the section `threads`.) */
#ifndef __wasm_atomics__
int scaly_eio_ncpu(void) { return 1; }
#endif

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

/* not macOS: no version to write into a triple (see eio.c) */
unsigned long long scaly_macos_version(char* buf, unsigned long long cap)
{
    (void)buf; (void)cap;
    return 0;
}

/* no threads in this profile: a spawn fails as pthread_create does where it
 * cannot start one, and the caller reports it (Thread.spawn's trap). (With
 * threads: the section `threads`.) */
#ifndef __wasm_atomics__
int scaly_thread_spawn_sized(unsigned long long* thread, void* start, void* arg, unsigned long long stack_size)
{
    (void)thread; (void)start; (void)arg; (void)stack_size;
    return -1;
}
#endif

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
    /* a whole number below 10^16 is written out: 10, not 1e+01 -- %g takes
       the exponent form as soon as the exponent reaches the precision, and
       the shortest precision of 10 is one digit. A larger or a tiny value
       keeps the exponent (1e+21, 1e-05), as a reader expects. */
    {
        int i;
        for (i = 0; i < n; i++) {
            if (buf[i] == 'e') {
                long x = strtol(buf + i + 1, NULL, 10);
                if (x >= precision && x < 16) {
                    n = snprintf(buf, (size_t)cap, "%.*g", (int)x + 1, value);
                    if (n < 0 || (unsigned long long)n >= cap)
                        return 0;
                }
                break;
            }
        }
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

/* The program's arguments, as in eio.c: WASI hands them to the entry as
 * UTF-8, which passes them on here. */
static int scaly_argc_;
static char** scaly_argv_;

/* the gate of a helper (eio.c): in a page there are no helpers yet */
void scaly_helper_gate(void (*helper)(void)) { (void)helper; }

void scaly_set_arguments(long long argc, char** argv)
{
    scaly_argc_ = (int)argc;
    scaly_argv_ = argv;
}

unsigned long long scaly_argument_count(void)
{
    return scaly_argc_ > 0 && scaly_argv_ ? (unsigned long long)scaly_argc_ : 0;
}

const char* scaly_argument(unsigned long long index)
{
    return index < scaly_argument_count() ? scaly_argv_[index] : 0;
}

/* ---- threads (wasm32-wasip1-threads) --------------------------------------- */
#ifdef __wasm_atomics__
#include <pthread.h>
#include <stdint.h>

/* The CPUs the host gives this program: SCALY_CPUS, which a host that runs
 * threads sets (a page from navigator.hardwareConcurrency, node from its
 * os.availableParallelism) -- WASI has no question for it. SCALY_WORKERS caps
 * it, as eio.c's does. Without either: one, and every loop runs inline. */
int scaly_eio_ncpu(void)
{
    const char* names[2] = { "SCALY_WORKERS", "SCALY_CPUS" };
    for (int i = 0; i < 2; i++) {
        const char* w = getenv(names[i]);
        if (w != NULL && *w != '\0') {
            long k = strtol(w, NULL, 10);
            if (k >= 1)
                return (int)k;
        }
    }
    return 1;
}

/* A thread is wasi-libc's pthread: the host starts it as a new instance of
 * this module over the same memory (wasi.thread-spawn). Its stack is taken
 * from the heap -- 8 MB unless the caller asks for a size, the main thread's
 * stack (the link's stack-size). Rule (a). */
int scaly_thread_spawn_sized(unsigned long long* thread, void* start, void* arg, unsigned long long stack_size)
{
    pthread_attr_t attr;
    pthread_t t;
    if (pthread_attr_init(&attr) != 0)
        return -1;
    pthread_attr_setstacksize(&attr, (size_t)(stack_size ? stack_size : 8u << 20));
    int rc = pthread_create(&t, &attr, (void* (*)(void*))start, arg);
    pthread_attr_destroy(&attr);
    if (rc != 0)
        return -1;
    *thread = (unsigned long long)(unsigned long)t;
    return 0;
}

/* A pipe in this program's memory: what the task pool and a task group wait
 * on (fiber.scaly), which WASI does not have. Its two descriptors are
 * numbers no WASI descriptor reaches; read and write go through
 * scaly_eio_read/_write, close through __wrap_close. As POSIX's: a read
 * blocks until there is a byte and answers 0 once the writing end is closed
 * and nothing is left; a write blocks while the buffer is full. Rule (a). */
#define SCALY_PIPES 1024
#define SCALY_PIPE_FD 0x30000000
#define SCALY_PIPE_BYTES 4096

struct scaly_wasm_pipe
{
    int used, closed_read, closed_write;
    unsigned head, count;
    pthread_mutex_t lock;
    pthread_cond_t changed;
    unsigned char bytes[SCALY_PIPE_BYTES];
};

static struct scaly_wasm_pipe* scaly_wasm_pipes[SCALY_PIPES];
static pthread_mutex_t scaly_wasm_pipes_lock = PTHREAD_MUTEX_INITIALIZER;

static int scaly_wasm_is_pipe(int fd)
{
    return fd >= SCALY_PIPE_FD && fd < SCALY_PIPE_FD + 2 * SCALY_PIPES;
}

static struct scaly_wasm_pipe* scaly_wasm_pipe_of(int fd)
{
    return scaly_wasm_pipes[(fd - SCALY_PIPE_FD) / 2];
}

int pipe(int fds[2])
{
    pthread_mutex_lock(&scaly_wasm_pipes_lock);
    int i = 0;
    while (i < SCALY_PIPES && scaly_wasm_pipes[i] && scaly_wasm_pipes[i]->used)
        i++;
    if (i == SCALY_PIPES) {
        pthread_mutex_unlock(&scaly_wasm_pipes_lock);
        errno = EMFILE;
        return -1;
    }
    struct scaly_wasm_pipe* p = scaly_wasm_pipes[i];
    if (!p) {
        p = (struct scaly_wasm_pipe*)malloc(sizeof *p);
        if (!p) {
            pthread_mutex_unlock(&scaly_wasm_pipes_lock);
            errno = ENOMEM;
            return -1;
        }
        pthread_mutex_init(&p->lock, NULL);
        pthread_cond_init(&p->changed, NULL);
        scaly_wasm_pipes[i] = p;
    }
    p->used = 1;
    p->closed_read = 0;
    p->closed_write = 0;
    p->head = 0;
    p->count = 0;
    pthread_mutex_unlock(&scaly_wasm_pipes_lock);
    fds[0] = SCALY_PIPE_FD + 2 * i;
    fds[1] = SCALY_PIPE_FD + 2 * i + 1;
    return 0;
}

static long long scaly_wasm_pipe_read(int fd, void* buf, unsigned long long count)
{
    struct scaly_wasm_pipe* p = scaly_wasm_pipe_of(fd);
    if (!p || !p->used || (fd & 1) != 0) {
        errno = EBADF;
        return -1;
    }
    if (count == 0)
        return 0;
    pthread_mutex_lock(&p->lock);
    while (p->count == 0 && !p->closed_write)
        pthread_cond_wait(&p->changed, &p->lock);
    unsigned long long n = 0;
    while (n < count && p->count > 0) {
        ((unsigned char*)buf)[n++] = p->bytes[p->head];
        p->head = (p->head + 1) % SCALY_PIPE_BYTES;
        p->count--;
    }
    pthread_cond_broadcast(&p->changed);
    pthread_mutex_unlock(&p->lock);
    return (long long)n;
}

static long long scaly_wasm_pipe_write(int fd, const void* buf, unsigned long long count)
{
    struct scaly_wasm_pipe* p = scaly_wasm_pipe_of(fd);
    if (!p || !p->used || (fd & 1) != 1) {
        errno = EBADF;
        return -1;
    }
    unsigned long long n = 0;
    pthread_mutex_lock(&p->lock);
    while (n < count) {
        while (p->count == SCALY_PIPE_BYTES && !p->closed_read)
            pthread_cond_wait(&p->changed, &p->lock);
        if (p->closed_read) {
            pthread_mutex_unlock(&p->lock);
            errno = EPIPE;
            return n ? (long long)n : -1;
        }
        p->bytes[(p->head + p->count) % SCALY_PIPE_BYTES] = ((const unsigned char*)buf)[n++];
        p->count++;
        pthread_cond_broadcast(&p->changed);
    }
    pthread_mutex_unlock(&p->lock);
    return (long long)n;
}

/* close: a pipe's end here, any other descriptor WASI's. The link wraps
 * `close` for this target (-Wl,--wrap=close, cli.scaly), so every caller of
 * it -- the standard library's and wasi-libc's own -- comes through here. */
int __real_close(int fd);
static int scaly_wasm_net_close(int fd);
int __wrap_close(int fd)
{
    if (scaly_wasm_is_net(fd))
        return scaly_wasm_net_close(fd);
    if (!scaly_wasm_is_pipe(fd))
        return __real_close(fd);
    struct scaly_wasm_pipe* p = scaly_wasm_pipe_of(fd);
    if (!p || !p->used) {
        errno = EBADF;
        return -1;
    }
    pthread_mutex_lock(&p->lock);
    if (fd & 1)
        p->closed_write = 1;
    else
        p->closed_read = 1;
    int both = p->closed_read && p->closed_write;
    pthread_cond_broadcast(&p->changed);
    pthread_mutex_unlock(&p->lock);
    if (both) {
        pthread_mutex_lock(&scaly_wasm_pipes_lock);
        p->used = 0;
        pthread_mutex_unlock(&scaly_wasm_pipes_lock);
    }
    return 0;
}

/* ---- sockets, through the host (a node of a cluster) --------------------- */

/* WASI preview 1 opens no socket, so this profile asks its host: the import
 * module `scaly_net` (packages/scaly/<version>/web/net.mjs), where the nodes
 * of a cluster in one page -- or in one node process -- listen and connect,
 * one port space for all of them, whatever host a connect names. A host
 * without a network refuses every call (wasi.mjs, NO_NET). Its descriptors
 * are numbers no WASI descriptor and no pipe of this file reaches; read and
 * write go through scaly_eio_read/_write, close through __wrap_close.
 *
 * Blocking as POSIX's sockets are, until Io.set_nonblocking: then a call
 * that would wait answers -2 -- after waiting a little in the host, for the
 * host's poll_oneoff answers a descriptor it does not know at once, and a
 * caller that polls before it calls again (the cluster's reader, keeping its
 * heartbeat clock) would spin. Rule (a). */
#define SCALY_NET_FD 0x40000000
#define SCALY_NET_LISTEN_FD (SCALY_NET_FD + 0x100000)
#define SCALY_NET_NAP_MS 50
#define SCALY_NET_IMPORT(name) __attribute__((import_module("scaly_net"), import_name(name)))

SCALY_NET_IMPORT("listen") int scaly_net_listen(int port);
SCALY_NET_IMPORT("port") int scaly_net_port(int fd);
SCALY_NET_IMPORT("connect") int scaly_net_connect(int port);
SCALY_NET_IMPORT("accept") int scaly_net_accept(int fd, int ms);
SCALY_NET_IMPORT("read") int scaly_net_read(int fd, void* buf, unsigned n, int ms);
SCALY_NET_IMPORT("write") int scaly_net_write(int fd, const void* buf, unsigned n, int ms);
SCALY_NET_IMPORT("ready") int scaly_net_ready(int fd, int for_write);
SCALY_NET_IMPORT("close") int scaly_net_close(int fd);

/* which descriptors were made non-blocking: the connections' and the
 * listeners' (their numbers as net.mjs makes them) */
static unsigned char scaly_wasm_net_nb[256 + 64];

static int scaly_wasm_is_net(int fd)
{
    return fd >= SCALY_NET_FD;
}

static unsigned char* scaly_wasm_net_flag(int fd)
{
    if (fd >= SCALY_NET_LISTEN_FD && fd < SCALY_NET_LISTEN_FD + 64)
        return &scaly_wasm_net_nb[256 + (fd - SCALY_NET_LISTEN_FD)];
    if (fd >= SCALY_NET_FD && fd < SCALY_NET_FD + 256)
        return &scaly_wasm_net_nb[fd - SCALY_NET_FD];
    return 0;
}

static int scaly_wasm_net_wait_ms(int fd)
{
    unsigned char* nb = scaly_wasm_net_flag(fd);
    return nb && *nb ? SCALY_NET_NAP_MS : -1;
}

int scaly_eio_set_nonblocking(int fd)
{
    unsigned char* nb = scaly_wasm_net_flag(fd);
    if (!nb)
        return -1;
    *nb = 1;
    return 0;
}

static long long scaly_wasm_net_read(int fd, void* buf, unsigned long long count)
{
    unsigned n = count > 0x40000000ULL ? 0x40000000U : (unsigned)count;
    return scaly_net_read(fd, buf, n, scaly_wasm_net_wait_ms(fd));
}

static long long scaly_wasm_net_write(int fd, const void* buf, unsigned long long count)
{
    unsigned n = count > 0x40000000ULL ? 0x40000000U : (unsigned)count;
    int r = scaly_net_write(fd, buf, n, scaly_wasm_net_wait_ms(fd));
    if (r == -1)
        errno = EPIPE;
    return r;
}

static int scaly_wasm_net_close(int fd)
{
    unsigned char* nb = scaly_wasm_net_flag(fd);
    if (nb)
        *nb = 0;
    if (scaly_net_close(fd) < 0) {
        errno = EBADF;
        return -1;
    }
    return 0;
}

static int scaly_wasm_net_accept(int fd)
{
    int c = scaly_net_accept(fd, scaly_wasm_net_wait_ms(fd));
    if (c >= 0) {
        unsigned char* nb = scaly_wasm_net_flag(c);
        if (nb)
            *nb = 0;
    }
    return c;
}

int scaly_eio_tcp_listen(int port) { return scaly_net_listen(port); }
int scaly_eio_tcp_listen_host(const char* host, int port) { (void)host; return scaly_net_listen(port); }
int scaly_eio_tcp_listen_any(int port) { return scaly_net_listen(port); }
int scaly_eio_tcp_port(int fd) { return scaly_net_port(fd); }
int scaly_eio_tcp_connect(int port)
{
    int c = scaly_net_connect(port);
    unsigned char* nb = c >= 0 ? scaly_wasm_net_flag(c) : 0;
    if (nb)
        *nb = 0;
    return c;
}
int scaly_eio_tcp_connect_host(const char* host, int port) { (void)host; return scaly_eio_tcp_connect(port); }
long long scaly_eio_tcp_write(int fd, const void* buf, unsigned long long count)
{
    return scaly_wasm_is_net(fd) ? scaly_wasm_net_write(fd, buf, count) : -1;
}
/* the host sends at once: there is no Nagle to switch off */
int scaly_eio_tcp_nodelay(int fd, int on) { (void)on; return scaly_wasm_is_net(fd) ? 0 : -1; }
int scaly_eio_accept(int fd) { return scaly_wasm_is_net(fd) ? scaly_wasm_net_accept(fd) : -1; }
int scaly_eio_accept_nb(int fd) { (void)fd; return -1; }

/* ---- the evented-I/O queue: wakes and readiness, in this program's memory -- */

/* What a scheduler waits on where there are no fibers: a task never parks
 * here, but the MAIN context of a thread does -- Channel.receive with a
 * scheduler made (a cluster's node: its connection's reader thread delivers
 * a message and wakes it), and Io.park of a descriptor. So a queue holds one
 * wake handle (scaly_eio_wake from any thread) and the descriptors armed on
 * it (each once, as kqueue's EV_ONESHOT), and a wait answers their tags as
 * they come ready: a wake at once, a descriptor of the network or a pipe
 * when the host or the pipe says so -- looked at every few milliseconds, as
 * nothing here can wake on both --, any other descriptor at once. Rule (a). */
#define SCALY_QUEUES 64
#define SCALY_ARMED 64

struct scaly_wasm_queue
{
    int used, wake_pending, has_wake, armed;
    void* wake_tag;
    int fd[SCALY_ARMED], for_write[SCALY_ARMED];
    void* tag[SCALY_ARMED];
    pthread_mutex_t lock;
    pthread_cond_t changed;
};

static struct scaly_wasm_queue* scaly_wasm_queues[SCALY_QUEUES];
static pthread_mutex_t scaly_wasm_queues_lock = PTHREAD_MUTEX_INITIALIZER;

static struct scaly_wasm_queue* scaly_wasm_queue_of(int q)
{
    return q >= 0 && q < SCALY_QUEUES ? scaly_wasm_queues[q] : 0;
}

int scaly_eio_create(void)
{
    pthread_mutex_lock(&scaly_wasm_queues_lock);
    int i = 0;
    while (i < SCALY_QUEUES && scaly_wasm_queues[i] && scaly_wasm_queues[i]->used)
        i++;
    if (i == SCALY_QUEUES) {
        pthread_mutex_unlock(&scaly_wasm_queues_lock);
        return -1;
    }
    struct scaly_wasm_queue* q = scaly_wasm_queues[i];
    if (!q) {
        q = (struct scaly_wasm_queue*)calloc(1, sizeof *q);
        if (!q) {
            pthread_mutex_unlock(&scaly_wasm_queues_lock);
            return -1;
        }
        pthread_mutex_init(&q->lock, 0);
        pthread_cond_init(&q->changed, 0);
        scaly_wasm_queues[i] = q;
    }
    q->used = 1;
    q->wake_pending = q->has_wake = q->armed = 0;
    q->wake_tag = 0;
    pthread_mutex_unlock(&scaly_wasm_queues_lock);
    return i;
}

int scaly_eio_arm(int qi, int fd, int for_write, void* tag)
{
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return -1;
    pthread_mutex_lock(&q->lock);
    int k = 0;
    while (k < q->armed && !(q->fd[k] == fd && q->for_write[k] == (for_write != 0)))
        k++;
    if (k == q->armed) {
        if (q->armed == SCALY_ARMED) {
            pthread_mutex_unlock(&q->lock);
            return -1;
        }
        q->armed++;
    }
    q->fd[k] = fd;
    q->for_write[k] = for_write != 0;
    q->tag[k] = tag;
    pthread_cond_broadcast(&q->changed);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int scaly_eio_wake_create(int qi, void* tag)
{
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return -1;
    pthread_mutex_lock(&q->lock);
    q->has_wake = 1;
    q->wake_tag = tag;
    pthread_mutex_unlock(&q->lock);
    return 1;
}

int scaly_eio_wake(int qi, int w)
{
    (void)w;
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return -1;
    pthread_mutex_lock(&q->lock);
    q->wake_pending = 1;
    pthread_cond_broadcast(&q->changed);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int scaly_eio_wake_close(int qi, int w)
{
    (void)w;
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return 0;
    pthread_mutex_lock(&q->lock);
    q->has_wake = 0;
    q->wake_pending = 0;
    pthread_mutex_unlock(&q->lock);
    return 0;
}

/* whether an armed descriptor is ready: the host's, a pipe's, or at once */
static int scaly_wasm_fd_ready(int fd, int for_write)
{
    if (scaly_wasm_is_net(fd))
        return scaly_net_ready(fd, for_write);
    if (scaly_wasm_is_pipe(fd)) {
        struct scaly_wasm_pipe* p = scaly_wasm_pipe_of(fd);
        if (!p || !p->used)
            return 1;
        pthread_mutex_lock(&p->lock);
        int r = for_write ? (p->closed_read || p->count < SCALY_PIPE_BYTES) : (p->closed_write || p->count > 0);
        pthread_mutex_unlock(&p->lock);
        return r;
    }
    return 1;
}

/* (under q's lock) the tags that are ready, each armed descriptor once */
static int scaly_wasm_queue_take(struct scaly_wasm_queue* q, void** tags, int max)
{
    int n = 0;
    if (q->wake_pending && q->has_wake && n < max) {
        tags[n++] = q->wake_tag;
        q->wake_pending = 0;
    }
    int k = 0;
    while (k < q->armed && n < max) {
        if (scaly_wasm_fd_ready(q->fd[k], q->for_write[k])) {
            tags[n++] = q->tag[k];
            q->armed--;
            q->fd[k] = q->fd[q->armed];
            q->for_write[k] = q->for_write[q->armed];
            q->tag[k] = q->tag[q->armed];
        } else
            k++;
    }
    return n;
}

/* ns below 0: no bound */
static int scaly_wasm_queue_wait(int qi, void** tags, int max, long long ns)
{
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return -1;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    long long until = ns < 0 ? -1 : (long long)now.tv_sec * 1000000000LL + now.tv_nsec + ns;
    pthread_mutex_lock(&q->lock);
    for (;;) {
        int n = scaly_wasm_queue_take(q, tags, max);
        clock_gettime(CLOCK_REALTIME, &now);
        long long t = (long long)now.tv_sec * 1000000000LL + now.tv_nsec;
        if (n > 0 || (until >= 0 && t >= until)) {
            pthread_mutex_unlock(&q->lock);
            return n;
        }
        /* an armed descriptor can come ready without a word to this queue:
         * look again in a few milliseconds */
        long long next = q->armed > 0 ? t + 5000000LL : until;
        if (until >= 0 && next > until)
            next = until;
        if (next < 0)
            pthread_cond_wait(&q->changed, &q->lock);
        else {
            struct timespec at = { (time_t)(next / 1000000000LL), (long)(next % 1000000000LL) };
            pthread_cond_timedwait(&q->changed, &q->lock, &at);
        }
    }
}

/* a queue is a number of this file's, no descriptor: it is given back */
int scaly_eio_close(int qi)
{
    struct scaly_wasm_queue* q = scaly_wasm_queue_of(qi);
    if (!q)
        return -1;
    pthread_mutex_lock(&scaly_wasm_queues_lock);
    pthread_mutex_lock(&q->lock);
    q->used = q->armed = q->has_wake = q->wake_pending = 0;
    pthread_mutex_unlock(&q->lock);
    pthread_mutex_unlock(&scaly_wasm_queues_lock);
    return 0;
}

int scaly_eio_wait(int q, void** tags, int max) { return scaly_wasm_queue_wait(q, tags, max, -1); }
int scaly_eio_wait_timeout(int q, void** tags, int max, int ms) { return scaly_wasm_queue_wait(q, tags, max, ms < 0 ? -1 : (long long)ms * 1000000LL); }
int scaly_eio_wait_timeout_ns(int q, void** tags, int max, long long ns) { return scaly_wasm_queue_wait(q, tags, max, ns); }

/* The break, which wasi-libc's malloc grows its heap by. Its own sbrk takes
 * the break to be the END OF THE MEMORY: each call grows the memory and hands
 * out what was added. That is right for a memory the program was given new --
 * but a host that runs a program again and again keeps ONE shared memory and
 * wipes it (threads.mjs, runMemory: a browser gives the address space of a
 * memory back late), and that memory is as large as the last run made it. The
 * next run's malloc began at its end, past everything the runs before had
 * grown, and a page that ran the program some dozens of times had used up
 * its gigabyte. So the break is this program's own: it begins where the heap
 * the link set aside ends (__heap_end, where malloc's first region ends) and
 * the memory grows only where the break goes past it. As wasi-libc's: whole
 * pages, never back; malloc calls it under its lock. Rule (a). */
extern char __heap_end;
static uintptr_t scaly_wasm_break;
void* sbrk(intptr_t increment)
{
    if (scaly_wasm_break == 0)
        scaly_wasm_break = (uintptr_t)&__heap_end;
    uintptr_t old = scaly_wasm_break;
    if (increment == 0)
        return (void*)old;
    if (increment < 0 || increment % 65536 != 0) {
        errno = EINVAL;
        return (void*)-1;
    }
    uintptr_t want = old + (uintptr_t)increment;
    if (want < old) {
        errno = ENOMEM;
        return (void*)-1;
    }
    uintptr_t have = (uintptr_t)__builtin_wasm_memory_size(0) * 65536;
    if (want > have && __builtin_wasm_memory_grow(0, (want - have + 65535) / 65536) == (size_t)-1) {
        errno = ENOMEM;
        return (void*)-1;
    }
    scaly_wasm_break = want;
    return (void*)old;
}
#endif
