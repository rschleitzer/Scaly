// Scaly fiber context switch — x86-64 Windows (MS x64 ABI). Stage 7, brocken 1.
//
// A SEPARATE file from fcontext_x86_64.S on purpose: Win64 is a different ABI,
// not a different OS. Recycling the SysV assembly would be wrong in four
// independent ways at once, which is why the roadmap says do not try.
//
//   1. ARGUMENTS. RCX/RDX/R8, not RDI/RSI/RDX.
//   2. CALLEE-SAVED SET. SysV keeps 6 GPRs; Win64 keeps 8 (adds RSI and RDI)
//      AND the ten SSE registers XMM6-XMM15. Dropping those silently corrupts
//      any fiber that crosses a switch with live float/vector state — the kind
//      of bug that shows up as a wrong number, not a crash.
//   3. SHADOW SPACE. Every call site must leave 32 bytes below its own frame
//      for the callee to spill its first four arguments into. The trampoline
//      has to allocate that before entering the fiber's entry function.
//   4. THE TIB. This is the one with no SysV counterpart at all: Windows keeps
//      the current stack's bounds in thread-local fields that the OS itself
//      reads (stack-overflow dispatch, guard-page growth, RtlUnwind's frame
//      validation). They describe the THREAD's stack, so a fiber running on a
//      different stack must swap them in and out with everything else.
//
// (4) is why this file changed the contract rather than just the register
// names: the bounds cannot be derived from the stack TOP alone, so
// scaly_make_context now takes the stack BASE as a third argument. Boost's
// make_fcontext takes a size for exactly this reason. Every caller already had
// it to hand (scaly/fiber.scaly computes `top` as base + size); the SysV file
// ignores the extra argument, which costs it nothing — a register it never
// reads.
//
// FRAME LAYOUT (0x120 bytes, 16-byte aligned — the XMM block needs it):
//   0x000  mxcsr             0x004  x87 control word
//   0x008  TIB fiber data    (gs:0x20)
//   0x010  TIB stack limit   (gs:0x10, the LOW address)
//   0x018  TIB stack base    (gs:0x08, the HIGH address)
//   0x020  TIB dealloc stack (gs:0x1478)
//   0x028  rbx   (fresh context: the entry function, consumed by the trampoline)
//   0x030  rbp   (fresh context: address of the never-return trap)
//   0x038  rsi     0x040  rdi     0x048  r12
//   0x050  r13     0x058  r14     0x060  r15     0x068  (pad)
//   0x070  xmm6 .. 0x100  xmm15   (10 x 16, 16-byte aligned by construction)
//   0x110  (pad)
//   0x118  pc  (resume address; for a fresh context: the trampoline)
//
// The 0x118 offset for `pc` is not cosmetic: jump_context allocates 0x118 and
// lets the CALLER'S OWN return address land in that slot, exactly as the SysV
// file does. It also lands the context 16-byte aligned — at entry RSP % 16 == 8
// and 0x118 % 16 == 8, so the subtraction cancels it — which is what makes the
// `movaps` at 0x070 legal. Change either number and the XMM saves fault.
//
// scaly_make_context(stack_top, fn, stack_base) -> ctx
// scaly_jump_context(to) -> ctx
//   The resumed side receives the jumper's context: a fresh context's entry
//   function gets it as its first argument (RCX), a suspended one as the
//   return value (RAX). The epilogue sets both, as in the SysV file.
//
// Windows x64 PE does NOT prefix C symbols with an underscore (that is 32-bit
// x86), so the names are written plain.

.text

.globl scaly_make_context
.balign 16
scaly_make_context:
.seh_proc scaly_make_context
.seh_endprologue
    // RCX = stack top (high), RDX = entry fn, R8 = stack base (low)
    movq  %rcx, %rax
    andq  $-16, %rax                    // 16-byte align down
    leaq  -0x120(%rax), %rax            // carve the context frame
    movq  %rdx, 0x28(%rax)              // rbx slot = entry fn
    stmxcsr 0x00(%rax)                  // seed SSE + x87 control from the caller
    fnstcw  0x04(%rax)
    movq  $0, 0x08(%rax)                // fiber data: none
    movq  %r8,  0x10(%rax)              // stack limit = base (low address)
    movq  %rcx, 0x18(%rax)              // stack base  = top  (high address)
    movq  %r8,  0x20(%rax)              // dealloc stack = the mapping's base
    leaq  ctx_trampoline(%rip), %r9
    movq  %r9, 0x118(%rax)              // pc = trampoline
    leaq  ctx_return(%rip), %r9
    movq  %r9, 0x30(%rax)               // rbp slot = never-return trap
    // Zero the XMM slots rather than resuming whatever bytes the stack held.
    // Harmless either way (the entry function treats them as callee-saved and
    // simply preserves them), but it keeps a fresh fiber's register state a
    // fact instead of a leftover.
    xorps %xmm0, %xmm0
    movaps %xmm0, 0x70(%rax)
    movaps %xmm0, 0x80(%rax)
    movaps %xmm0, 0x90(%rax)
    movaps %xmm0, 0xa0(%rax)
    movaps %xmm0, 0xb0(%rax)
    movaps %xmm0, 0xc0(%rax)
    movaps %xmm0, 0xd0(%rax)
    movaps %xmm0, 0xe0(%rax)
    movaps %xmm0, 0xf0(%rax)
    movaps %xmm0, 0x100(%rax)
    ret
.seh_endproc

.balign 16
ctx_trampoline:
    // RSP = the aligned stack top, 16-byte aligned; RBX = entry fn,
    // RBP = trap, RCX = the jumper's context.
    //
    // Win64 order matters here: the 32 bytes of shadow space must sit ABOVE
    // the return address, which is what a real `call` produces — reserve the
    // shadow first, then push the return address below it. At the entry
    // function's first instruction that gives [RSP] = return address and
    // [RSP+8 .. RSP+0x27] = its shadow space, with RSP % 16 == 8, which is
    // exactly the state the ABI promises.
    subq  $0x20, %rsp                   // shadow space for the entry function
    pushq %rbp                          // its return address = the trap
    jmp   *%rbx

.balign 16
ctx_return:
    // A fiber entry function must never return.
    ud2

.globl scaly_jump_context
.balign 16
scaly_jump_context:
.seh_proc scaly_jump_context
    leaq  -0x118(%rsp), %rsp            // the caller's return address becomes pc
.seh_stackalloc 0x118
.seh_endprologue
    // RCX = target context.
    stmxcsr 0x00(%rsp)
    fnstcw  0x04(%rsp)
    movq  %gs:0x20,   %rax              // TIB fiber data
    movq  %rax, 0x08(%rsp)
    movq  %gs:0x10,   %rax              // TIB stack limit
    movq  %rax, 0x10(%rsp)
    movq  %gs:0x08,   %rax              // TIB stack base
    movq  %rax, 0x18(%rsp)
    movq  %gs:0x1478, %rax              // TIB deallocation stack
    movq  %rax, 0x20(%rsp)
    movq  %rbx, 0x28(%rsp)
    movq  %rbp, 0x30(%rsp)
    movq  %rsi, 0x38(%rsp)
    movq  %rdi, 0x40(%rsp)
    movq  %r12, 0x48(%rsp)
    movq  %r13, 0x50(%rsp)
    movq  %r14, 0x58(%rsp)
    movq  %r15, 0x60(%rsp)
    movaps %xmm6,  0x70(%rsp)
    movaps %xmm7,  0x80(%rsp)
    movaps %xmm8,  0x90(%rsp)
    movaps %xmm9,  0xa0(%rsp)
    movaps %xmm10, 0xb0(%rsp)
    movaps %xmm11, 0xc0(%rsp)
    movaps %xmm12, 0xd0(%rsp)
    movaps %xmm13, 0xe0(%rsp)
    movaps %xmm14, 0xf0(%rsp)
    movaps %xmm15, 0x100(%rsp)

    movq  %rsp, %rax                    // RAX = the jumper's context
    movq  %rcx, %rsp                    // switch stacks

    movq  0x118(%rsp), %r10             // resume pc (R10 is volatile: free scratch)
    ldmxcsr 0x00(%rsp)
    fldcw   0x04(%rsp)
    movq  0x08(%rsp), %r11              // TIB fiber data
    movq  %r11, %gs:0x20
    movq  0x10(%rsp), %r11              // TIB stack limit
    movq  %r11, %gs:0x10
    movq  0x18(%rsp), %r11              // TIB stack base
    movq  %r11, %gs:0x08
    movq  0x20(%rsp), %r11              // TIB deallocation stack
    movq  %r11, %gs:0x1478
    movq  0x28(%rsp), %rbx
    movq  0x30(%rsp), %rbp
    movq  0x38(%rsp), %rsi
    movq  0x40(%rsp), %rdi
    movq  0x48(%rsp), %r12
    movq  0x50(%rsp), %r13
    movq  0x58(%rsp), %r14
    movq  0x60(%rsp), %r15
    movaps 0x70(%rsp),  %xmm6
    movaps 0x80(%rsp),  %xmm7
    movaps 0x90(%rsp),  %xmm8
    movaps 0xa0(%rsp),  %xmm9
    movaps 0xb0(%rsp),  %xmm10
    movaps 0xc0(%rsp),  %xmm11
    movaps 0xd0(%rsp),  %xmm12
    movaps 0xe0(%rsp),  %xmm13
    movaps 0xf0(%rsp),  %xmm14
    movaps 0x100(%rsp), %xmm15
    leaq  0x120(%rsp), %rsp
    movq  %rax, %rcx                    // deliver the jumper's context
    jmp   *%r10
.seh_endproc
