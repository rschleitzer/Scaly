// Scaly fiber context switch — SysV x86-64 (darwin + linux).
// Modeled on Boost.Context's fcontext design (jump_x86_64_sysv): a context
// is a stack pointer to a 0x40-byte frame holding the callee-saved state.
// Assembled by clang via tools/fcontext.sh and archived into libscaly.a;
// the Scaly-side extern declarations live in packages/scaly/0.1.1/scaly/
// fiber.scaly. One file serves both OSes: Mach-O mangles C names with a
// leading underscore, ELF does not — SYM() below absorbs the difference.
//
// Frame layout (16-byte aligned, 0x40 = 64 bytes):
//   0x00  mxcsr (SSE control/status)     0x04  x87 control word
//   0x08  r12    0x10  r13    0x18  r14    0x20  r15
//   0x28  rbx  (fresh context: the entry function, consumed by trampoline)
//   0x30  rbp  (fresh context: address of the never-return trap)
//   0x38  pc   (resume address; fresh context: the trampoline)
//
// scaly_make_context(stack_top, fn, stack_base) -> ctx
//   stack_base (rdx) is IGNORED here and read only by the Win64 file, which
//   needs the stack's low address for the TIB bounds it has to swap on every
//   switch. One declaration serves every target because one seed does, so the
//   argument is passed unconditionally; on SysV that costs a register nothing
//   reads. See fcontext_x86_64_windows.S.
//
//   Carves a fresh frame below stack_top. The first jump lands on the
//   trampoline, which pushes the trap address as fn's return address (also
//   restoring the ABI's entry alignment, rsp % 16 == 8) and enters fn with
//   rdi = the jumper's context. fn must never return.
//
// scaly_jump_context(to) -> ctx
//   Suspends the current context, resumes `to`. The resumed side receives
//   the jumper's fresh context: a fresh context's entry fn gets it as its
//   argument (rdi); a suspended context gets it as the return value (rax) —
//   the epilogue sets both.

#if defined(__APPLE__)
#define SYM(name) _##name
#define LABEL(name) L##name
#else
#define SYM(name) name
#define LABEL(name) .L##name
#endif

.text

.globl SYM(scaly_make_context)
#if defined(__ELF__)
.type scaly_make_context, @function
#endif
.balign 16
SYM(scaly_make_context):
    // rdi = stack top (high address), rsi = entry fn
    movq  %rdi, %rax
    andq  $-16, %rax                   // 16-byte align down
    leaq  -0x40(%rax), %rax            // carve context frame
    movq  %rsi, 0x28(%rax)             // rbx slot = entry fn (trampoline jumps to it)
    stmxcsr 0x00(%rax)                 // seed SSE + x87 control from the caller
    fnstcw  0x04(%rax)
    leaq  LABEL(ctx_trampoline)(%rip), %rcx
    movq  %rcx, 0x38(%rax)             // pc = trampoline
    leaq  LABEL(ctx_return)(%rip), %rcx
    movq  %rcx, 0x30(%rax)             // rbp slot = never-return trap
    ret

LABEL(ctx_trampoline):
    // rsp = aligned stack top, rbp = trap, rbx = entry fn, rdi = jumper ctx.
    pushq %rbp                         // fn's return address; entry rsp % 16 == 8
    jmp   *%rbx

LABEL(ctx_return):
    // A fiber entry function must never return (milestone 1.1 semantics;
    // fiber completion arrives with the scheduler in milestone 1.3).
    ud2

.globl SYM(scaly_jump_context)
#if defined(__ELF__)
.type scaly_jump_context, @function
#endif
.balign 16
SYM(scaly_jump_context):
    // rdi = target context; the caller's ret address sits at 0x38 after this.
    leaq  -0x38(%rsp), %rsp
    stmxcsr 0x00(%rsp)
    fnstcw  0x04(%rsp)
    movq  %r12, 0x08(%rsp)
    movq  %r13, 0x10(%rsp)
    movq  %r14, 0x18(%rsp)
    movq  %r15, 0x20(%rsp)
    movq  %rbx, 0x28(%rsp)
    movq  %rbp, 0x30(%rsp)
    movq  %rsp, %rax                   // rax = jumper's context
    movq  %rdi, %rsp                   // switch stacks
    movq  0x38(%rsp), %r8              // resume pc
    ldmxcsr 0x00(%rsp)
    fldcw   0x04(%rsp)
    movq  0x08(%rsp), %r12
    movq  0x10(%rsp), %r13
    movq  0x18(%rsp), %r14
    movq  0x20(%rsp), %r15
    movq  0x28(%rsp), %rbx
    movq  0x30(%rsp), %rbp
    leaq  0x40(%rsp), %rsp
    movq  %rax, %rdi                   // deliver jumper's context to resumed side
    jmp   *%r8

#if defined(__ELF__)
// Non-executable stack marker (silences lld/GNU ld execstack warnings).
.section .note.GNU-stack,"",%progbits
#endif
