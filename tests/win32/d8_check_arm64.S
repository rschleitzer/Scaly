// Callee-saved register preservation across the arm64 Windows context switch.
// The arm64 twin of xmm_check.S, whose header has the reasoning: why assembly
// on both sides, and why the fiber must DESTROY the registers under test.
//
// The symbol names are xmm_check.S's on purpose, although there is no XMM
// here: fcontext_smoke.c is one driver for both, and it links whichever file
// the triple selects.
//
// void* scaly_xmm_check(void* ctx, unsigned long long* out)
//   Loads a distinct pattern into each register under test, jumps to `ctx`
//   (a context made over scaly_xmm_fiber), and on return writes the eighteen
//   observed values into out[]:
//       out[0..9]    x19 .. x28
//       out[10..17]  d8 .. d15   (AAPCS64 preserves only the low 64 bits of
//                                 v8..v15, so this is all of the obligation)
//   Returns the context the fiber handed back.
//
// x29 and x30 are not under test: a switch that lost either does not come
// back here at all. x18 is the TEB and is nobody's to load.
//
// Frame (0xb0, 16-byte aligned):
//   0x00  x29, x30        0x10..0x5f  x19..x28      0x60..0x9f  d8..d15
//   0xa0  the out pointer 0xa8        the returned ctx

.text

// 0xHHHHHHHH0000000n with HHHH twice: GPR 0x1111, FP 0x2222, anti 0xEEEE.
.macro LOADPAT reg, hi, n
    movz \reg, #\n
    movk \reg, #\hi, lsl #32
    movk \reg, #\hi, lsl #48
.endm

.globl scaly_xmm_check
.balign 4
scaly_xmm_check:
    sub  sp, sp, #0xb0
    // What we owe our own caller, before trampling it.
    stp  x29, x30, [sp, #0x00]
    stp  x19, x20, [sp, #0x10]
    stp  x21, x22, [sp, #0x20]
    stp  x23, x24, [sp, #0x30]
    stp  x25, x26, [sp, #0x40]
    stp  x27, x28, [sp, #0x50]
    stp  d8,  d9,  [sp, #0x60]
    stp  d10, d11, [sp, #0x70]
    stp  d12, d13, [sp, #0x80]
    stp  d14, d15, [sp, #0x90]
    str  x1, [sp, #0xa0]                // out

    // Load the patterns. x0 (ctx) stays untouched; x9 is volatile.
    LOADPAT x19, 0x1111, 0
    LOADPAT x20, 0x1111, 1
    LOADPAT x21, 0x1111, 2
    LOADPAT x22, 0x1111, 3
    LOADPAT x23, 0x1111, 4
    LOADPAT x24, 0x1111, 5
    LOADPAT x25, 0x1111, 6
    LOADPAT x26, 0x1111, 7
    LOADPAT x27, 0x1111, 8
    LOADPAT x28, 0x1111, 9
    LOADPAT x9, 0x2222, 0
    fmov d8, x9
    LOADPAT x9, 0x2222, 1
    fmov d9, x9
    LOADPAT x9, 0x2222, 2
    fmov d10, x9
    LOADPAT x9, 0x2222, 3
    fmov d11, x9
    LOADPAT x9, 0x2222, 4
    fmov d12, x9
    LOADPAT x9, 0x2222, 5
    fmov d13, x9
    LOADPAT x9, 0x2222, 6
    fmov d14, x9
    LOADPAT x9, 0x2222, 7
    fmov d15, x9

    bl   scaly_jump_context
    str  x0, [sp, #0xa8]                // park the returned ctx

    // Read the registers out.
    ldr  x9, [sp, #0xa0]
    stp  x19, x20, [x9, #0x00]
    stp  x21, x22, [x9, #0x10]
    stp  x23, x24, [x9, #0x20]
    stp  x25, x26, [x9, #0x30]
    stp  x27, x28, [x9, #0x40]
    stp  d8,  d9,  [x9, #0x50]
    stp  d10, d11, [x9, #0x60]
    stp  d12, d13, [x9, #0x70]
    stp  d14, d15, [x9, #0x80]

    // Give our caller its registers back.
    ldp  x29, x30, [sp, #0x00]
    ldp  x19, x20, [sp, #0x10]
    ldp  x21, x22, [sp, #0x20]
    ldp  x23, x24, [sp, #0x30]
    ldp  x25, x26, [sp, #0x40]
    ldp  x27, x28, [sp, #0x50]
    ldp  d8,  d9,  [sp, #0x60]
    ldp  d10, d11, [sp, #0x70]
    ldp  d12, d13, [sp, #0x80]
    ldp  d14, d15, [sp, #0x90]
    ldr  x0, [sp, #0xa8]
    add  sp, sp, #0xb0
    ret

// The fiber: overwrite every register under test with an anti-pattern and
// jump straight back. Entry state, from scaly_jump_context's `ret x4` into a
// fresh context: x0 = the jumper's context, sp 16-byte aligned.
.globl scaly_xmm_fiber
.balign 4
scaly_xmm_fiber:
    LOADPAT x19, 0xEEEE, 0
    LOADPAT x20, 0xEEEE, 1
    LOADPAT x21, 0xEEEE, 2
    LOADPAT x22, 0xEEEE, 3
    LOADPAT x23, 0xEEEE, 4
    LOADPAT x24, 0xEEEE, 5
    LOADPAT x25, 0xEEEE, 6
    LOADPAT x26, 0xEEEE, 7
    LOADPAT x27, 0xEEEE, 8
    LOADPAT x28, 0xEEEE, 9
    LOADPAT x9, 0xEEEE, 10
    fmov d8,  x9
    fmov d9,  x9
    fmov d10, x9
    fmov d11, x9
    fmov d12, x9
    fmov d13, x9
    fmov d14, x9
    fmov d15, x9
    bl   scaly_jump_context
    // One round trip only: a second entry is a bug in the harness.
    brk  #0xf001
