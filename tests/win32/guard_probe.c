/* guard_probe.c — the fiber guard-page path, without any Scaly in it.
 * Stage 7, the instrument that measured brocken 5's handler room.
 *
 * ★LOCAL INSTRUMENT, NOT PART OF ANY CI RUNG, and deliberately so: it needs
 * ml64 (see guard_probe.asm) which the runner does not have, while the runner
 * already covers the same path end to end through the corpus test
 * `fiber__guard_overflow`. What this file adds is what a corpus test cannot: it
 * varies the RECURSION FRAME SIZE, which is what decides whether the exception
 * dispatch has room to run at all, and it can move the guard page to try a
 * layout the Scaly side does not have yet. Build and sweep lines are in
 * tests/win32/WINDOWS-BOX.md section 5.
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
 *     band         1 = keep the LOWEST page writable and guard the page above
 *                  it, so the dispatch has somewhere to spill. This is the
 *                  candidate that closes the anonymous-death class, and the
 *                  driver can try it because WHERE the guard sits is the
 *                  caller's decision — proving it out here costs no stdlib
 *                  change and no seed refresh.
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

extern void switch_stack(void *stack_top, void (*entry)(void));
extern void recurse_asm(SIZE_T frame_bytes);

/* 128 KB, FIBER_STACK_SIZE in fiber.scaly. */
#define STACK_SIZE 0x20000u

static char *g_base;   /* the mapping's low address */
static char *g_guard;  /* the guarded page — g_base, or one page up with band */
static SIZE_T g_page;
static SIZE_T g_frame;
static int g_measure;
static HANDLE g_err;

/* fiber_guard_hit, in C. */
static int classify(void *addr)
{
    return (char *)addr >= g_guard && (char *)addr < g_guard + g_page;
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
    if (!classify((void *)addr))
        return EXCEPTION_CONTINUE_SEARCH;

    g_len = 0;
    emit("  code=");
    emit_hex(ep->ExceptionRecord->ExceptionCode);
    emit(" fault=");
    emit_hex(addr);
    emit(" (");
    emit_dec(addr - (ULONG_PTR)g_guard);
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

    g_base = (char *)VirtualAlloc(NULL, STACK_SIZE, MEM_RESERVE | MEM_COMMIT,
                                  PAGE_READWRITE);
    if (g_base == NULL)
        return 2;
    g_guard = g_base + (band ? g_page : 0);

    if (scaly_stack_guard(g_guard, g_page) != 0)
        return 3;
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
    emit_hex((ULONG_PTR)g_guard);
    emit("\n");
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
