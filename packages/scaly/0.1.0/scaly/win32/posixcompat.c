/* POSIX compatibility shim for Win64 (stage 7, brocken 3).
 *
 * Provides the fourteen POSIX symbols the Scaly runtime and its test corpus
 * reference that Windows does not have. Compiled ONLY on Windows
 * (tools/win32compat.sh); everywhere else the platform provides these itself
 * and this file is not built.
 *
 * ★It was twelve until 2026-08-10, and the two that were missing say something
 * about the instrument rather than about Windows: `setenv` and `socketpair` are
 * declared `extern` in TEST sources, not in `packages/`, so a completeness
 * check scoped to the runtime root could not see them. `tools/win-undef.sh`
 * now unions the runtime with every program the Windows corpus links.
 *
 * Why it provides the POSIX NAMES rather than changing the Scaly side: the
 * committed seed ships ONE scaly.ll for every target, so the runtime's extern
 * declarations cannot be target-conditional. Renaming them for Windows would
 * mean a second seed. Providing the names here keeps the Scaly side ignorant
 * of Windows, which is the same reasoning that puts eio.c on the C side.
 *
 * The list is not a guess — `tools/win-undef.sh` measures it by cross-emitting
 * the runtime for the Windows triple and classifying every undefined symbol.
 * Run it; class B is exactly what this file owes. It is 12 rather than 13
 * because `aligned_alloc` could not be faked: Windows has `_aligned_malloc`,
 * whose memory must be released with `_aligned_free`, and plain `free()` on it
 * is undefined — so alloc and free had to move together, as the
 * scaly_aligned_alloc/scaly_aligned_free pair on the Scaly side.
 *
 * WHAT THIS FILE MAY NOT DO: guess. Where a POSIX contract has no faithful
 * Win32 equivalent, it fails loudly (or is documented as narrowed) rather than
 * approximating — a silently wrong syscall shim is the worst of the three
 * outcomes, and the hardest to find later.
 */

#ifdef _WIN32

#include <winsock2.h>   /* before windows.h — it defines the socket API */
#include <ws2tcpip.h>   /* IPPROTO_TCP/TCP_NODELAY for the socketpair emulation */
#include <windows.h>
#include <io.h>         /* _pipe */
#include <fcntl.h>      /* _O_BINARY — NOT in io.h, despite _pipe living there */
#include <errno.h>      /* setenv's EINVAL */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Virtual memory ---------------------------------------------------
 *
 * The only caller is the fiber stack pool (scaly/fiber.scaly): one anonymous
 * private RW mapping per stack, with its first page turned PROT_NONE as a
 * guard. `addr` is always NULL, `fd` is always -1 and `offset` always 0, so
 * the file-mapping half of mmap's contract is never exercised — the shim
 * REFUSES it rather than pretending, because a file mapping through
 * VirtualAlloc would silently return anonymous memory.
 *
 * munmap maps to MEM_RELEASE, which requires the exact base VirtualAlloc
 * returned and a size of 0; both callers pass the original base, so that
 * holds. The `length` argument is therefore ignored, as MEM_RELEASE demands.
 */
#define SC_PROT_NONE  0
#define SC_PROT_READ  1
#define SC_PROT_WRITE 2

static DWORD sc_prot_to_win(int prot)
{
    if (prot == SC_PROT_NONE)
        return PAGE_NOACCESS;
    if (prot & SC_PROT_WRITE)
        return PAGE_READWRITE;
    return PAGE_READONLY;
}

void* mmap(void* addr, size_t length, int prot, int flags, int fd, long long offset)
{
    (void)flags;
    (void)offset;
    if (fd != -1 || addr != NULL)
        return (void*)-1;   /* MAP_FAILED — see the refusal note above */
    return VirtualAlloc(NULL, length, MEM_RESERVE | MEM_COMMIT, sc_prot_to_win(prot));
}

int munmap(void* addr, size_t length)
{
    (void)length;   /* MEM_RELEASE requires 0 */
    return VirtualFree(addr, 0, MEM_RELEASE) ? 0 : -1;
}

int mprotect(void* addr, size_t len, int prot)
{
    DWORD old;
    return VirtualProtect(addr, len, sc_prot_to_win(prot), &old) ? 0 : -1;
}

int getpagesize(void)
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwPageSize;
}

/* ---- Threads ----------------------------------------------------------
 *
 * pthread_t is carried as size_t on the Scaly side (it is opaque and
 * pointer-sized on every target we serve), so the HANDLE fits it exactly.
 *
 * The entry signatures differ — POSIX `void*(*)(void*)` against Win32
 * `DWORD(*)(LPVOID)` — and the cast is safe HERE for one reason worth
 * recording: x64 has a single calling convention, and the caller
 * (scaly/fiber.scaly, Thread.join) discards the result, so the only
 * difference is that the thread's 64-bit return is read back as 32 bits by
 * machinery nobody consults. Do not copy this cast to a site that USES the
 * value.
 */
int pthread_create(size_t* thread, void* attr, void* start, void* arg)
{
    HANDLE h;
    (void)attr;
    h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)start, arg, 0, NULL);
    if (h == NULL)
        return -1;
    *thread = (size_t)h;
    return 0;
}

int pthread_join(size_t thread, void* retval)
{
    HANDLE h = (HANDLE)thread;
    (void)retval;   /* the caller passes NULL; POSIX would store the result */
    if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0)
        return -1;
    CloseHandle(h);
    return 0;
}

/* ---- Directories ------------------------------------------------------
 *
 * opendir/closedir are used for exactly ONE thing in this tree
 * (scaly/io/Directory.scaly, dir_isdir_worker): open it, and if that worked,
 * close it — an is-this-a-directory test. There is no readdir anywhere, which
 * `tools/win-undef.sh` confirms by its absence from the undefined set. So the
 * whole dirent machinery reduces to one attribute query, and the handle is a
 * non-NULL sentinel rather than a real directory stream.
 *
 * If a readdir ever appears, this must become FindFirstFile/FindNextFile and
 * the sentinel becomes a real object — hence the loud name.
 */
static char sc_dir_sentinel;

void* opendir(const char* name)
{
    DWORD a = GetFileAttributesA(name);
    if (a == INVALID_FILE_ATTRIBUTES)
        return NULL;
    if (!(a & FILE_ATTRIBUTE_DIRECTORY))
        return NULL;
    return &sc_dir_sentinel;
}

int closedir(void* dirp)
{
    return (dirp == &sc_dir_sentinel) ? 0 : -1;
}

/* ---- Path splitting ---------------------------------------------------
 *
 * POSIX semantics, faithfully: dirname MODIFIES its argument in place and
 * returns it; basename returns a pointer INTO it. The one caller
 * (scaly/io/Path.scaly) passes a fresh `to_c_string()` copy, so in-place
 * modification is safe, and it compares the result against "." — which is what
 * POSIX dirname answers for a path with no separator, so that answer is a
 * contract, not a detail.
 *
 * The one deliberate difference from POSIX: BOTH '/' and '\\' count as
 * separators. Windows accepts either in every API, our own path joining emits
 * '/', and a user-supplied path will carry '\\' — treating only '/' would make
 * `dirname("C:\\a\\b")` answer "." and silently collapse a real directory.
 */
static int sc_is_sep(char c) { return c == '/' || c == '\\'; }

char* dirname(char* path)
{
    size_t n;
    char* p;
    if (path == NULL || *path == '\0')
        return ".";
    n = strlen(path);
    while (n > 1 && sc_is_sep(path[n - 1]))   /* strip trailing separators */
        n--;
    p = NULL;
    {
        size_t i = n;
        while (i > 0) {
            if (sc_is_sep(path[i - 1])) { p = path + (i - 1); break; }
            i--;
        }
    }
    if (p == NULL)
        return ".";
    if (p == path) {          /* root: "/a" -> "/" */
        path[1] = '\0';
        return path;
    }
    while (p > path && sc_is_sep(p[-1]))   /* collapse a run of separators */
        p--;
    *p = '\0';
    return path;
}

char* basename(char* path)
{
    size_t n;
    if (path == NULL || *path == '\0')
        return ".";
    n = strlen(path);
    while (n > 1 && sc_is_sep(path[n - 1]))
        path[--n] = '\0';
    {
        size_t i = n;
        while (i > 0) {
            if (sc_is_sep(path[i - 1]))
                return path + i;
            i--;
        }
    }
    return path;
}

/* ---- Pipes and polling ------------------------------------------------
 *
 * `pipe` is the CRT's _pipe with a default buffer, in binary mode: text mode
 * would translate '\n' to "\r\n" inside the pipe, which would corrupt every
 * framed protocol that crosses it.
 *
 * `poll` is WSAPoll, and the narrowing is REAL and must not be papered over:
 * WSAPoll works on SOCKETS only, so a poll on a pipe or a file descriptor
 * fails with WSAENOTSOCK rather than answering. That is deliberate — the
 * alternative (a WaitForMultipleObjects emulation keyed on handle type) is a
 * different mechanism with different semantics, and guessing between them is
 * what this file must not do. The one structural mismatch is the fd width:
 * POSIX `struct pollfd.fd` is a 32-bit int, WSAPOLLFD's is a 64-bit SOCKET, so
 * the arrays are NOT layout-compatible and the entries are translated one by
 * one rather than cast.
 */
int pipe(int fds[2])
{
    return _pipe(fds, 65536, _O_BINARY);
}

int poll(void* fds, unsigned long long nfds, int timeout)
{
    /* The Scaly-side struct: { int fd; short events; short revents; } */
    struct sc_pollfd { int fd; short events; short revents; };
    struct sc_pollfd* in = (struct sc_pollfd*)fds;
    WSAPOLLFD stackbuf[16];
    WSAPOLLFD* w = stackbuf;
    int rc;
    unsigned long long i;

    if (nfds > sizeof stackbuf / sizeof stackbuf[0]) {
        w = (WSAPOLLFD*)malloc((size_t)nfds * sizeof(WSAPOLLFD));
        if (w == NULL)
            return -1;
    }
    for (i = 0; i < nfds; i++) {
        w[i].fd = (SOCKET)(size_t)(unsigned int)in[i].fd;
        w[i].events = in[i].events;
        w[i].revents = 0;
    }
    rc = WSAPoll(w, (ULONG)nfds, timeout);
    for (i = 0; i < nfds; i++)
        in[i].revents = w[i].revents;
    if (w != stackbuf)
        free(w);
    return rc;
}

/* ---- Environment ------------------------------------------------------
 *
 * `_putenv_s` is the CRT's writer; the only thing it does not carry is
 * `overwrite`, which POSIX defines as "leave an existing value alone when 0" —
 * and the callers rely on it (tests/fiber/{taskpool_default,trace_balance}
 * arm SCALY_TRACE_HEAP), so it is implemented rather than dropped.
 *
 * TWO documented narrowings, neither of them a guess:
 *
 *  - `getenv` reads the CRT's copy of the environment, which is the same copy
 *    `_putenv_s` writes and the same one the Scaly side reads back, so the
 *    overwrite test is consistent with itself. It is NOT consistent with a
 *    value set through `SetEnvironmentVariableA` behind the CRT's back; no
 *    caller in this tree does that.
 *  - an EMPTY value DELETES the variable on Windows (that is `_putenv_s`'s
 *    documented contract) where POSIX would define it as empty. There is no
 *    CRT call that sets an empty value, so this cannot be fixed here — it is
 *    recorded rather than approximated. Every caller passes a non-empty value.
 */
int setenv(const char* name, const char* value, int overwrite)
{
    if (name == NULL || *name == '\0' || strchr(name, '=') != NULL || value == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (!overwrite && getenv(name) != NULL)
        return 0;
    return _putenv_s(name, value) == 0 ? 0 : -1;
}

/* ---- Socket pairs -----------------------------------------------------
 *
 * Windows has no `socketpair` and no usable AF_UNIX stream socket, so the pair
 * is manufactured over the loopback interface: listen on 127.0.0.1:0, connect
 * to the port the kernel picked, accept, drop the listener. Both callers
 * (tests/fiber/echo.scaly, echo_main.scaly) ask for AF_UNIX/SOCK_STREAM, i.e.
 * (1, 1, 0), and what they need from it is a connected byte-stream pair — which
 * loopback TCP is. Anything else is REFUSED rather than approximated: a
 * datagram or raw request would get a stream socket and misbehave later, far
 * from here.
 *
 * The descriptors are raw SOCKET handles carried in an int, which is the
 * convention the whole Windows I/O side already uses — `eio_win.c` decides
 * socket-vs-CRT-descriptor with SO_TYPE on exactly this representation, and
 * `scaly_eio_tcp_connect` hands its sockets back the same way. Winsock is
 * started here as well as there; WSAStartup is refcounted, so a second start
 * is free and neither file may assume the other ran first.
 *
 * SECURITY: an ephemeral loopback listener is connectable by any process on
 * the machine for the instant it exists, so the accepted peer is VERIFIED to
 * be our own client — its remote address and port must equal the client's
 * local address and port — and the pair is torn down if it is not. Without
 * that check the "pair" could quietly be a stranger's socket.
 *
 * TCP_NODELAY is set on both ends because the contract being emulated is a
 * local stream socket, which has no write-coalescing delay; leaving Nagle on
 * would make a small-message ping-pong depend on the ack timing of a loopback
 * connection. That is fidelity to the emulated contract, not a tuning knob.
 *
 * ★KNOWN HAZARD at the far end, recorded because it is not fixable HERE: POSIX
 * code closes a socketpair descriptor with `close`, and on Windows `close` is
 * the CRT's, which knows nothing of socket handles — it validates the number
 * against its own descriptor table and terminates the process through the
 * invalid-parameter handler when it does not match. If echo/echo_main now fail
 * at their `close` calls rather than at the link, that is this, and the fix is
 * a `close` in THIS file that decides with SO_TYPE exactly as eio_win.c's
 * read/write do — a separate rung, because it changes a symbol every caller
 * uses and not just these two.
 */
static INIT_ONCE sc_pc_ws_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK sc_pc_ws_init(PINIT_ONCE o, PVOID p, PVOID* c)
{
    WSADATA d;
    (void)o; (void)p; (void)c;
    return WSAStartup(MAKEWORD(2, 2), &d) == 0;
}

static void sc_pc_nodelay(SOCKET s)
{
    int on = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, (int)sizeof on);
}

int socketpair(int domain, int sotype, int protocol, int sv[2])
{
    SOCKET listener = INVALID_SOCKET;
    SOCKET client   = INVALID_SOCKET;
    SOCKET accepted = INVALID_SOCKET;
    struct sockaddr_in addr, bound, mine, theirs;
    int len;

    /* AF_UNIX (1) and AF_INET (2) are both served by the loopback pair; the
     * stream type is not negotiable, see the refusal note above. */
    if ((domain != 1 && domain != AF_INET) || sotype != SOCK_STREAM || protocol != 0)
        return -1;
    if (sv == NULL)
        return -1;

    InitOnceExecuteOnce(&sc_pc_ws_once, sc_pc_ws_init, NULL, NULL);

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;                      /* let the kernel pick */

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET)
        goto fail;
    if (bind(listener, (struct sockaddr*)&addr, (int)sizeof addr) == SOCKET_ERROR)
        goto fail;
    len = (int)sizeof bound;
    if (getsockname(listener, (struct sockaddr*)&bound, &len) == SOCKET_ERROR)
        goto fail;
    if (listen(listener, 1) == SOCKET_ERROR)
        goto fail;

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == INVALID_SOCKET)
        goto fail;
    if (connect(client, (struct sockaddr*)&bound, (int)sizeof bound) == SOCKET_ERROR)
        goto fail;

    len = (int)sizeof theirs;
    accepted = accept(listener, (struct sockaddr*)&theirs, &len);
    if (accepted == INVALID_SOCKET)
        goto fail;

    /* Is the connection we accepted the one we made? */
    len = (int)sizeof mine;
    if (getsockname(client, (struct sockaddr*)&mine, &len) == SOCKET_ERROR)
        goto fail;
    if (mine.sin_port != theirs.sin_port
        || mine.sin_addr.s_addr != theirs.sin_addr.s_addr)
        goto fail;

    closesocket(listener);
    listener = INVALID_SOCKET;
    sc_pc_nodelay(client);
    sc_pc_nodelay(accepted);

    sv[0] = (int)accepted;
    sv[1] = (int)client;
    return 0;

fail:
    if (listener != INVALID_SOCKET) closesocket(listener);
    if (client   != INVALID_SOCKET) closesocket(client);
    if (accepted != INVALID_SOCKET) closesocket(accepted);
    return -1;
}

#else
/* Not Windows: an empty translation unit is not valid C, and every build
 * script that might compile this file unconditionally needs something here. */
typedef int scaly_posixcompat_not_needed_on_this_target;
#endif /* _WIN32 */
