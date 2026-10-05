/* guard_probe.c — the fiber guard-page path, without any Scaly in it.
 * Stage 7, the instrument that measured brocken 5's handler room.
 *
 * ★LOCAL INSTRUMENT, NOT PART OF ANY CI RUNG, and deliberately so: it needs
 * ml64 (see guard_probe.asm) which the runner does not have, while the runner
 * already covers the same path end to end through the corpus test
 * `fiber__guard_overflow`. What this file adds is what a corpus test cannot: it
 * varies the RECURSION FRAME SIZE, which is what decides whether the exception
 * dispatch has room to run at all, and it can move the guard page to try a
 * layout the Scaly side does not have yet.
 *
 * WHAT IS UNDER TEST IS THE COMMITTED SHIM. scaly_stack_guard,
 * scaly_guard_install and the vectored handler come from eio_win.obj; this file
 * only supplies what fiber.scaly supplies on the Scaly side — a hand-made stack
 * carrying the TIB fields scaly_make_context writes, and fiber_guard_hit's
 * predicate. A copy of the handler would measure the copy.
 *
 *   guard_probe.exe [frame_bytes] [band] [measure]
 *
 *     frame_bytes  0 (default) = ~512-byte C frames, what tests/fiber's
 *                  guard_overflow has; otherwise an exact frame step.
 *     band         1 = ask for the guard one page HIGHER than the mapping's
 *                  bottom, keeping the lowest page writable as an emergency band
 *                  for the exception dispatch. ★This is the knob that PROVED the
 *                  band before anyone built it — the sweep goes from 41/64 to
 *                  64/64 — and it is still the checksum for the rung that will
 *                  land it. It cannot be done in the shim: `fiber.scaly` keeps
 *                  the stack pool's free-list node right above the guard page,
 *                  so both halves have to move together (see scaly_stack_guard).
 *     measure      1 = install a second handler AHEAD of the shim's that prints
 *                  the geometry (fault rsp, handler rsp, what the dispatch
 *                  spent) and then falls through to it. ★It PERTURBS: its own
 *                  frame and its WriteFile come out of the same few hundred
 *                  bytes, so a run that reports 108 without it can die with it.
 *                  Measure with it, judge without it.
 *
 * Exit codes: 108 = the shim reported and ended the process, which is the pass.
 * 0xC0000005 / 0xC00000FD = the process died with no handler run at all.
 */

#include <windows.h>

int scaly_stack_guard(void *base, size_t len);
int scaly_guard_install(int (*classify)(void *));
/* ★The stack comes from the SHIM's mmap, not from VirtualAlloc directly, because
 * on Windows that shim IS the fiber stack allocator — it is where the emergency
 * band below the guard page is bought. Allocating here would test a layout no
 * fiber ever has. */
void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           long long offset);
int munmap(void *addr, size_t length);

extern void switch_stack(void *stack_top, void (*entry)(void));
extern void recurse_asm(SIZE_T frame_bytes);

/* 128 KB, FIBER_STACK_SIZE in fiber.scaly. */
#define STACK_SIZE 0x20000u

static char *g_base;   /* the mapping's low address */
static char *g_guard;  /* what we ASK the shim to guard */
static char *g_real;   /* where the guard actually ended up — see find_guard */
static SIZE_T g_page;
static SIZE_T g_frame;
static int g_measure;
static HANDLE g_err;

/* fiber_guard_hit, in C: the range the SCALY side believes in — the page it
 * asked the shim to guard. The shim may have put the real guard elsewhere and
 * then owes the translation; that is exactly what this predicate must NOT know
 * about, or the test would stop testing it. */
static int classify(void *addr)
{
    return (char *)addr >= g_guard && (char *)addr < g_guard + g_page;
}

/* ★Where the guard REALLY is, read back from the OS rather than assumed. The
 * shim is free to place it away from the page it was handed (Win64 keeps a
 * writable band below it for the exception dispatch), so an instrument that
 * assumed `g_guard` would mislabel every number it prints — and, worse, would
 * silently stop noticing if the band disappeared. Scanning for PAGE_GUARD makes
 * the placement a MEASUREMENT: `guard=` in the banner is the shim's answer. */
static char *find_guard(void)
{
    MEMORY_BASIC_INFORMATION mbi;
    char *p;
    for (p = g_base; p < g_base + 8 * g_page; p += g_page) {
        if (VirtualQuery(p, &mbi, sizeof mbi) == sizeof mbi
            && (mbi.Protect & PAGE_GUARD) != 0)
            return p;
    }
    return NULL;
}

/* ---- writing without a stack buffer or the CRT ------------------------ */

static char g_buf[512];
static unsigned g_len;

static void emit(const char *s)
{
    while (*s && g_len < sizeof g_buf - 1)
        g_buf[g_len++] = *s++;
}

static void emit_dec(ULONG_PTR v)
{
    char t[24];
    int n = 0;
    if (v == 0) {
        emit("0");
        return;
    }
    while (v && n < (int)sizeof t) {
        t[n++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (n-- > 0 && g_len < sizeof g_buf - 1)
        g_buf[g_len++] = t[n];
}

static void emit_hex(ULONG_PTR v)
{
    static const char d[] = "0123456789abcdef";
    int i;
    emit("0x");
    for (i = 60; i >= 0; i -= 4)
        if (g_len < sizeof g_buf - 1)
            g_buf[g_len++] = d[(v >> i) & 0xf];
}

static void flush(void)
{
    DWORD written = 0;
    WriteFile(g_err, g_buf, g_len, &written, NULL);
    g_len = 0;
}

/* ---- the measuring handler, ahead of the shim's ----------------------- */

static LONG CALLBACK measure_veh(EXCEPTION_POINTERS *ep)
{
    ULONG_PTR here = (ULONG_PTR)&ep; /* ~rsp of this frame */
    ULONG_PTR addr;

    if (ep->ExceptionRecord->NumberParameters < 2)
        return EXCEPTION_CONTINUE_SEARCH;
    addr = ep->ExceptionRecord->ExceptionInformation[1];
    /* Against the REAL guard, not the requested one: this handler measures the
     * machine, while `classify` above plays the Scaly side. */
    if (g_real == NULL || addr < (ULONG_PTR)g_real
        || addr >= (ULONG_PTR)g_real + g_page)
        return EXCEPTION_CONTINUE_SEARCH;

    g_len = 0;
    emit("  code=");
    emit_hex(ep->ExceptionRecord->ExceptionCode);
    emit(" fault=");
    emit_hex(addr);
    emit(" (");
    emit_dec(addr - (ULONG_PTR)g_real);
    emit(" into the guarded page)\n  rsp at fault=");
    emit_dec(ep->ContextRecord->Rsp - (ULONG_PTR)g_base);
    emit(" above the mapping, rsp in handler=");
    emit_dec(here - (ULONG_PTR)g_base);
    emit(", dispatch spent ");
    emit_dec(ep->ContextRecord->Rsp - here);
    emit("\n");
    flush();
    return EXCEPTION_CONTINUE_SEARCH; /* on to the shim's handler */
}

/* ---- the recursion --------------------------------------------------- */

static volatile char g_sink;

static void recurse(unsigned depth)
{
    volatile char frame[512];
    /* Unreachable, and there for the compiler rather than the reader: without a
     * way out, "recursive on all control paths" is an error under /WX — which is
     * a fair warning about a function whose whole job is to overflow. */
    if (depth == 0xFFFFFFFFu)
        return;
    frame[0] = (char)depth;
    frame[511] = (char)depth;
    recurse(depth + 1);
    g_sink = frame[0];
}

static void fiber_entry(void)
{
    if (g_frame)
        recurse_asm(g_frame);
    recurse(0);
}

/* ---- setup ----------------------------------------------------------- */

static SIZE_T arg_num(int argc, char **argv, int i)
{
    const char *p;
    SIZE_T v = 0;
    if (argc <= i)
        return 0;
    p = argv[i];
    while (*p >= '0' && *p <= '9')
        v = v * 10 + (SIZE_T)(*p++ - '0');
    return v;
}

int main(int argc, char **argv)
{
    SYSTEM_INFO si;
    char *teb;
    int band;

    g_err = GetStdHandle(STD_ERROR_HANDLE);
    GetSystemInfo(&si);
    g_page = si.dwPageSize;

    g_frame = arg_num(argc, argv, 1) & ~(SIZE_T)15;
    band = arg_num(argc, argv, 2) != 0;
    g_measure = arg_num(argc, argv, 3) != 0;

    /* PROT_READ|PROT_WRITE and MAP_ANON|MAP_PRIVATE as fiber.scaly spells them. */
    g_base = (char *)mmap(NULL, STACK_SIZE, 3, 0x1022, -1, 0);
    if (g_base == (char *)-1 || g_base == NULL)
        return 2;
    g_guard = g_base + (band ? g_page : 0);

    if (scaly_stack_guard(g_guard, g_page) != 0)
        return 3;
    g_real = find_guard();
    if (g_real == NULL)
        return 6;
    if (scaly_guard_install(classify) != 0)
        return 4;
    /* First in the chain, so it runs BEFORE the shim's and can fall through. */
    if (g_measure && AddVectoredExceptionHandler(1, measure_veh) == NULL)
        return 5;

    g_len = 0;
    emit("guard_probe: frame=");
    emit_dec(g_frame ? g_frame : 512);
    emit(g_frame ? " exact" : " (C frames)");
    emit(" band=");
    emit_dec((ULONG_PTR)band);
    emit(" mapping=");
    emit_hex((ULONG_PTR)g_base);
    emit(" guard=");
    emit_hex((ULONG_PTR)g_real);
    /* The band the ALLOCATOR bought, read back from the OS: everything between
     * the true allocation base and the guard page is writable spill room. An
     * assumed number here would be the one thing this file must not print. */
    {
        MEMORY_BASIC_INFORMATION mbi;
        emit(" band=");
        if (VirtualQuery(g_base, &mbi, sizeof mbi) == sizeof mbi)
            emit_dec((ULONG_PTR)(g_real - (char *)mbi.AllocationBase));
        else
            emit("?");
        emit(" bytes\n");
    }
    flush();

    /* The TIB fields scaly_make_context writes, so the kernel sees this exactly
     * as it sees a fiber stack: the whole mapping is the thread's stack and
     * DeallocationStack == StackLimit, i.e. there is nothing below to grow into.
     * That is why the fault arrives as STATUS_STACK_OVERFLOW rather than as a
     * guard-page violation. */
    teb = (char *)NtCurrentTeb();
    *(char **)(teb + 0x08) = g_base + STACK_SIZE; /* StackBase (high) */
    *(char **)(teb + 0x10) = g_base;              /* StackLimit (low) */
    *(char **)(teb + 0x1478) = g_base;            /* DeallocationStack */

    switch_stack(g_base + STACK_SIZE, fiber_entry);
    return 0;
}
