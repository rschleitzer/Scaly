/* Does a catch point survive on Win64, and does it survive a stack the program
 * switched itself? (2026-09-19)
 *
 * The Windows corpus reported six failures, all of them catchers and all of
 * them fiber or parfor programs, each ending in the harness' `exit 127` with
 * an empty stderr. Two diagnoses fit that evidence equally well — "longjmp is
 * broken on this target" and "longjmp is broken across a stack fcontext
 * switched" — and the corpus cannot tell them apart, because it holds no
 * catcher that stays on the thread's original stack. This probe does, at the C
 * level, with no Scaly and no runtime in the way, and it asks the candidate
 * FIX in the same run:
 *
 *   A  setjmp/longjmp inside one function, original stack       (the floor)
 *   B  setjmp here, jump to a fiber stack, longjmp back         (our shape)
 *   C  the same as B through the NON-UNWINDING spelling         (the candidate)
 *
 * ★Every probe prints its result and FLUSHES before the next one starts: the
 * interesting outcome is a probe that kills the process, and a buffered line
 * would take the evidence with it. How far the output got IS the measurement.
 *
 * ★On x64 the Windows CRT's `setjmp` records a frame pointer and `longjmp`
 * then unwinds through SEH (RtlUnwindEx), which needs unwind data for every
 * frame in between; a stack that fcontext switched has none, and the CRT ends
 * the process rather than returning an error. `_setjmp(buf, NULL)` asks for
 * the jump WITHOUT that unwind, which is what probe C tries. If B dies and C
 * survives, the fix for panic.c (and for the emitter's own `setjmp` call per
 * `try` arm) is exactly that spelling; if C dies too, the cause is elsewhere
 * and nobody has to spend a round guessing.
 *
 * ★Probe C is behind SJ_PROBE_C because its spelling is the very thing in
 * question: MSVC declares `_setjmp` with the frame argument on x64, clang may
 * declare it with one. The step compiles this file WITH the macro first and,
 * if that fails, again WITHOUT it — so a compile error about probe C is itself
 * a finding and still leaves A and B measured in the same round. A probe that
 * takes its neighbours down with it answers nothing.
 */
#include <setjmp.h>
#include <stdio.h>

void* scaly_make_context(void* stack_top, void* fn, void* stack_base);
void* scaly_jump_context(void* to);
void* scaly_aligned_alloc(long long alignment, long long size);
void  scaly_aligned_free(void* p);

static jmp_buf home;
static int     mode;               /* 1 = probe B, 2 = probe C */

static void say(const char* line)
{
    printf("%s", line);
    fflush(stdout);
}

/* The fiber's entry. The switcher hands the jumper's context in the first
 * argument register (fiber.scaly's API note), and an entry function must never
 * return — this one leaves through the catch point, which is the whole probe. */
static void fiber_entry(void* from)
{
    (void)from;
    say("  fiber: running on the switched stack\n");
    longjmp(home, mode);
    say("  fiber: longjmp RETURNED (impossible)\n");
    for (;;) { }
}

static void cross_stack(int which)
{
    const long long size = 1 << 16;
    void* base;
    void* top;
    void* ctx;

    base = scaly_aligned_alloc(4096, size);
    if (base == NULL) {
        say("  could not allocate a fiber stack\n");
        return;
    }
    top = (char*)base + size;
    ctx = scaly_make_context(top, (void*)fiber_entry, base);
    if (ctx == NULL) {
        scaly_aligned_free(base);
        say("  make_context returned null\n");
        return;
    }
    mode = which;
    (void)scaly_jump_context(ctx);     /* control leaves through longjmp */
    say("  the switch RETURNED without a jump (unexpected)\n");
    scaly_aligned_free(base);
}

int main(void)
{
    say("setjmp_smoke (Win64 catch points)\n");

    /* --- A: the floor ------------------------------------------------- */
    if (setjmp(home) == 0) {
        say("  A: catch point installed, jumping\n");
        longjmp(home, 7);
        say("  A: longjmp RETURNED (impossible)\n");
        return 1;
    }
    say("  A: PASS — a jump on the original stack arrives\n");

    /* --- B: our shape -------------------------------------------------- */
    say("  B: about to jump out of a switched stack\n");
    if (setjmp(home) == 0) {
        cross_stack(1);
        say("  B: FAIL — no jump happened\n");
        return 1;
    }
    say("  B: PASS — a jump out of a fiber stack arrives\n");

    /* --- C: the candidate fix ------------------------------------------ */
#ifdef SJ_PROBE_C
    say("  C: about to jump out of a switched stack, non-unwinding\n");
    if (_setjmp(home, NULL) == 0) {
        cross_stack(2);
        say("  C: FAIL — no jump happened\n");
        return 1;
    }
    say("  C: PASS — the non-unwinding jump arrives\n");
#else
    say("  C: not compiled in\n");
#endif

    say("setjmp_smoke: PASS\n");
    return 0;
}
