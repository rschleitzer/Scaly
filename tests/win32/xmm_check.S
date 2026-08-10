// Callee-saved register preservation across a Win64 context switch.
// Stage 7, brocken 1 — the half pingpong cannot see.
//
// WHAT THIS EXISTS FOR. pingpong proves the switch WORKS: six jumps, correct
// output. It cannot prove the switch is CORRECT, because it holds no live
// float or vector state across a jump — if fcontext_x86_64_win.S had forgotten
// XMM6-15 entirely, pingpong would still be green. Those ten registers plus
// RSI and RDI are exactly what Win64 adds to the SysV callee-saved set, so they
// are the likeliest place for a mistake AND the one whose failure is a wrong
// NUMBER rather than a crash.
//
// WHY ASSEMBLY, both halves. Two reasons, and each on its own is sufficient.
//
//   1. A C driver cannot GUARANTEE it tests anything. Whether the compiler
//      keeps a value in a callee-saved register or spills it to the stack is
//      its choice; a version that spills would pass while exercising nothing.
//      A test whose effectiveness cannot be shown is precisely the failure this
//      file was written to rule out.
//   2. THE FIBER MUST DESTROY THE REGISTERS. A missing save and a missing
//      restore cancel out: the value would simply still be in the register when
//      control came back, and the test would pass having proven nothing. So the
//      fiber overwrites every one of them with a distinct anti-pattern, and
//      only a genuine save-and-restore can bring the originals back.
//
// int/void* scaly_xmm_check(void* ctx, unsigned long long* out)
//   Loads a distinct pattern into each register under test, jumps to `ctx`
//   (which must be a context made over scaly_xmm_fiber), and on return writes
//   the eighteen observed values into out[] in this order:
//       out[0..7]   rbx, rbp, rsi, rdi, r12, r13, r14, r15
//       out[8..17]  low 64 bits of xmm6 .. xmm15
//   Returns the context the fiber handed back, so the caller can tell a
//   completed round trip from a wild jump.
//
//   Only the LOW half of each XMM is reported. That catches every plausible
//   failure — a register not saved, not restored, or saved to the wrong offset,
//   since the patterns are distinct — but not a 64-bit save where a 128-bit one
//   was meant. The switch uses movaps uniformly, so that shape is not on the
//   table; if it ever is, widen out[] and read the high halves with movhlps.
//
// Frame (0x118, which lands RSP 16-byte aligned: entry has RSP%16 == 8 and
// 0x118%16 == 8, so the subtraction cancels it — the movaps below depend on it):
//   0x00..0x1f  shadow space for the call to scaly_jump_context
//   0x20..0x5f  saved rbx, rbp, rsi, rdi, r12, r13, r14, r15 (this function's
//               own obligation to ITS caller)
//   0x60..0xff  saved xmm6..xmm15   (0x60 is 16-aligned by construction)
//   0x100       the out pointer
//   0x108       the ctx argument
//   0x110       the returned ctx, parked while registers are read out

.text

// Patterns. Distinct per register so a wrong OFFSET shows up as a swap rather
// than as a pass.
#define GPR_PAT(n) (0x1111111100000000 + (n))
#define XMM_PAT(n) (0x2222222200000000 + (n))
#define ANTI_PAT(n) (0xEEEEEEEE00000000 + (n))

.globl scaly_xmm_check
.balign 16
scaly_xmm_check:
.seh_proc scaly_xmm_check
    subq  $0x118, %rsp
.seh_stackalloc 0x118
.seh_endprologue
    // Preserve what we owe our own caller before trampling it.
    movq  %rbx, 0x20(%rsp)
    movq  %rbp, 0x28(%rsp)
    movq  %rsi, 0x30(%rsp)
    movq  %rdi, 0x38(%rsp)
    movq  %r12, 0x40(%rsp)
    movq  %r13, 0x48(%rsp)
    movq  %r14, 0x50(%rsp)
    movq  %r15, 0x58(%rsp)
    movaps %xmm6,  0x60(%rsp)
    movaps %xmm7,  0x70(%rsp)
    movaps %xmm8,  0x80(%rsp)
    movaps %xmm9,  0x90(%rsp)
    movaps %xmm10, 0xa0(%rsp)
    movaps %xmm11, 0xb0(%rsp)
    movaps %xmm12, 0xc0(%rsp)
    movaps %xmm13, 0xd0(%rsp)
    movaps %xmm14, 0xe0(%rsp)
    movaps %xmm15, 0xf0(%rsp)
    movq  %rdx, 0x100(%rsp)          // out
    movq  %rcx, 0x108(%rsp)          // ctx

    // Load the patterns.
    movabsq $GPR_PAT(0), %rbx
    movabsq $GPR_PAT(1), %rbp
    movabsq $GPR_PAT(2), %rsi
    movabsq $GPR_PAT(3), %rdi
    movabsq $GPR_PAT(4), %r12
    movabsq $GPR_PAT(5), %r13
    movabsq $GPR_PAT(6), %r14
    movabsq $GPR_PAT(7), %r15
    movabsq $XMM_PAT(0), %rax
    movq    %rax, %xmm6
    movabsq $XMM_PAT(1), %rax
    movq    %rax, %xmm7
    movabsq $XMM_PAT(2), %rax
    movq    %rax, %xmm8
    movabsq $XMM_PAT(3), %rax
    movq    %rax, %xmm9
    movabsq $XMM_PAT(4), %rax
    movq    %rax, %xmm10
    movabsq $XMM_PAT(5), %rax
    movq    %rax, %xmm11
    movabsq $XMM_PAT(6), %rax
    movq    %rax, %xmm12
    movabsq $XMM_PAT(7), %rax
    movq    %rax, %xmm13
    movabsq $XMM_PAT(8), %rax
    movq    %rax, %xmm14
    movabsq $XMM_PAT(9), %rax
    movq    %rax, %xmm15

    // Round trip. RSP is 16-aligned and the 32 bytes at RSP are the callee's
    // shadow space, which is what the ABI asks of a call site.
    movq  0x108(%rsp), %rcx
    call  scaly_jump_context
    movq  %rax, 0x110(%rsp)          // park the returned ctx

    // Read the registers out. R10/R11 are volatile, so they are free here —
    // and deliberately not any register under test.
    movq  0x100(%rsp), %r10
    movq  %rbx,  0x00(%r10)
    movq  %rbp,  0x08(%r10)
    movq  %rsi,  0x10(%r10)
    movq  %rdi,  0x18(%r10)
    movq  %r12,  0x20(%r10)
    movq  %r13,  0x28(%r10)
    movq  %r14,  0x30(%r10)
    movq  %r15,  0x38(%r10)
    movq  %xmm6,  %r11
    movq  %r11,  0x40(%r10)
    movq  %xmm7,  %r11
    movq  %r11,  0x48(%r10)
    movq  %xmm8,  %r11
    movq  %r11,  0x50(%r10)
    movq  %xmm9,  %r11
    movq  %r11,  0x58(%r10)
    movq  %xmm10, %r11
    movq  %r11,  0x60(%r10)
    movq  %xmm11, %r11
    movq  %r11,  0x68(%r10)
    movq  %xmm12, %r11
    movq  %r11,  0x70(%r10)
    movq  %xmm13, %r11
    movq  %r11,  0x78(%r10)
    movq  %xmm14, %r11
    movq  %r11,  0x80(%r10)
    movq  %xmm15, %r11
    movq  %r11,  0x88(%r10)

    // Give our caller its registers back.
    movq  0x20(%rsp), %rbx
    movq  0x28(%rsp), %rbp
    movq  0x30(%rsp), %rsi
    movq  0x38(%rsp), %rdi
    movq  0x40(%rsp), %r12
    movq  0x48(%rsp), %r13
    movq  0x50(%rsp), %r14
    movq  0x58(%rsp), %r15
    movaps 0x60(%rsp), %xmm6
    movaps 0x70(%rsp), %xmm7
    movaps 0x80(%rsp), %xmm8
    movaps 0x90(%rsp), %xmm9
    movaps 0xa0(%rsp), %xmm10
    movaps 0xb0(%rsp), %xmm11
    movaps 0xc0(%rsp), %xmm12
    movaps 0xd0(%rsp), %xmm13
    movaps 0xe0(%rsp), %xmm14
    movaps 0xf0(%rsp), %xmm15
    movq  0x110(%rsp), %rax
    addq  $0x118, %rsp
    ret
.seh_endproc

// The fiber. Its ONLY job is to make a missing save-and-restore pair
// detectable: it overwrites every register under test with an anti-pattern and
// jumps straight back. Written here rather than in C because a C function
// would preserve exactly the registers we need it to destroy.
//
// Entry state, from the trampoline in fcontext_x86_64_win.S: RCX = the
// jumper's context, RSP%16 == 8 with 32 bytes of shadow space above the
// return address. `sub 0x38` lands RSP 16-aligned again and reserves this
// function's own shadow space for the call below.
.globl scaly_xmm_fiber
.balign 16
scaly_xmm_fiber:
.seh_proc scaly_xmm_fiber
    subq  $0x38, %rsp
.seh_stackalloc 0x38
.seh_endprologue
    movq  %rcx, 0x30(%rsp)           // the context to jump back to
    movabsq $ANTI_PAT(0), %rbx
    movabsq $ANTI_PAT(1), %rbp
    movabsq $ANTI_PAT(2), %rsi
    movabsq $ANTI_PAT(3), %rdi
    movabsq $ANTI_PAT(4), %r12
    movabsq $ANTI_PAT(5), %r13
    movabsq $ANTI_PAT(6), %r14
    movabsq $ANTI_PAT(7), %r15
    movabsq $ANTI_PAT(8), %rax
    movq    %rax, %xmm6
    movq    %rax, %xmm7
    movq    %rax, %xmm8
    movq    %rax, %xmm9
    movq    %rax, %xmm10
    movq    %rax, %xmm11
    movq    %rax, %xmm12
    movq    %rax, %xmm13
    movq    %rax, %xmm14
    movq    %rax, %xmm15
    movq  0x30(%rsp), %rcx
    call  scaly_jump_context
    // A second entry would mean the caller jumped in again; this test does
    // exactly one round trip, so getting here at all is a bug in the harness
    // rather than in the switch. Fall into the trap.
    ud2
.seh_endproc
