/* Callee-saved register preservation across the Win64 context switch.
 * Stage 7, brocken 1 — the half pingpong cannot see.
 *
 * pingpong proves the switch works. It cannot prove it is correct: it holds no
 * live float or vector state across a jump, so a switch that had forgotten
 * XMM6-15 entirely would still print the right thing. Those ten registers plus
 * RSI and RDI are exactly what Win64 adds to the SysV callee-saved set — the
 * likeliest place for a mistake, and the one whose failure is a wrong NUMBER
 * instead of a crash.
 *
 * The mechanics live in xmm_check.S, in assembly, for two reasons written out
 * there: a C driver cannot guarantee the compiler keeps anything in a
 * callee-saved register, and the fiber has to DESTROY those registers or a
 * missing save and a missing restore would cancel out into a green test.
 *
 * This file only builds a context, runs one round trip, and reads the verdict.
 */

#include <stdio.h>

void*     scaly_make_context(void* stack_top, void* fn, void* stack_base);
void*     scaly_xmm_check(void* ctx, unsigned long long* out);
void      scaly_xmm_fiber(void);
void*     scaly_aligned_alloc(long long alignment, long long size);
void      scaly_aligned_free(void* p);

#define GPR_PAT(n)  (0x1111111100000000ULL + (n))
#define XMM_PAT(n)  (0x2222222200000000ULL + (n))
#define ANTI_PAT(n) (0xEEEEEEEE00000000ULL + (n))

/* Eighteen registers on both processors, split differently: x64 has eight
 * integer and ten vector ones (xmm_check.S), arm64 ten and eight
 * (d8_check_arm64.S). */
#if defined(_M_ARM64) || defined(__aarch64__)
#define N_GPR 10
#define VEC_NAME "d%d"
#define VEC_FIRST 8
static const char* const gpr_name[N_GPR] = {
    "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
};
#else
#define N_GPR 8
#define VEC_NAME "xmm%d"
#define VEC_FIRST 6
static const char* const gpr_name[N_GPR] = {
    "rbx", "rbp", "rsi", "rdi", "r12", "r13", "r14", "r15"
};
#endif

static int failures;

static void expect(unsigned long long got, unsigned long long want,
                   const char* what)
{
    if (got == want) {
        printf("  %-6s preserved\n", what);
        return;
    }
    failures++;
    /* The anti-pattern is a DIFFERENT diagnosis from garbage: it means the
     * fiber's own value came through, i.e. the switch neither saved nor
     * restored this register. Garbage means it restored from the wrong place. */
    if ((got & 0xFFFFFFFF00000000ULL) == 0xEEEEEEEE00000000ULL)
        printf("  %-6s NOT SAVED/RESTORED — the fiber's value came through"
               " (%016llx)\n", what, got);
    else
        printf("  %-6s WRONG — want %016llx, got %016llx\n", what, want, got);
}

int main(void)
{
    const long long size = 1 << 16;
    unsigned long long out[18];
    void* base;
    void* top;
    void* ctx;
    void* back;
    int i;

    printf("fcontext_smoke (Win64 callee-saved preservation)\n");

    base = scaly_aligned_alloc(4096, size);
    if (base == NULL) {
        printf("  could not allocate a fiber stack\nfcontext_smoke: 1 FAIL\n");
        return 1;
    }
    top = (char*)base + size;

    ctx = scaly_make_context(top, (void*)scaly_xmm_fiber, base);
    if (ctx == NULL) {
        printf("  make_context returned null\nfcontext_smoke: 1 FAIL\n");
        return 1;
    }

    for (i = 0; i < 18; i++)
        out[i] = 0;

    back = scaly_xmm_check(ctx, out);

    /* A completed round trip hands back the fiber's suspended context. Null
     * would mean the jump went somewhere else and we are reading noise, so it
     * is checked BEFORE the registers are believed. */
    if (back == NULL) {
        printf("  the round trip did not complete (null context back)\n");
        failures++;
    } else {
        printf("  round trip completed\n");
    }

    for (i = 0; i < N_GPR; i++)
        expect(out[i], GPR_PAT(i), gpr_name[i]);
    for (i = 0; i < 18 - N_GPR; i++) {
        char nm[8];
        snprintf(nm, sizeof nm, VEC_NAME, VEC_FIRST + i);
        expect(out[N_GPR + i], XMM_PAT(i), nm);
    }

    scaly_aligned_free(base);
    printf(failures ? "fcontext_smoke: %d FAIL\n" : "fcontext_smoke: PASS\n",
           failures);
    return failures ? 1 : 0;
}
