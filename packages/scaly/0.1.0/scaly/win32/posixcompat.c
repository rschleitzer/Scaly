/* POSIX compatibility shim for Win64 (stage 7, brocken 3).
 *
 * Provides the twelve POSIX symbols the Scaly runtime references that Windows
 * does not have. Compiled ONLY on Windows (tools/win32compat.sh); everywhere
 * else the platform provides these itself and this file is not built.
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
#include <windows.h>
#include <io.h>         /* _pipe */
#include <fcntl.h>      /* _O_BINARY — NOT in io.h, despite _pipe living there */
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

#else
/* Not Windows: an empty translation unit is not valid C, and every build
 * script that might compile this file unconditionally needs something here. */
typedef int scaly_posixcompat_not_needed_on_this_target;
#endif /* _WIN32 */
