// Scaly fiber context switch — AAPCS64 (arm64/aarch64, darwin + linux).
// Modeled on Boost.Context's fcontext design (jump_arm64_aapcs): a context
// is a stack pointer to a 0xb0-byte frame holding the callee-saved state.
// Assembled by clang via tools/fcontext.sh and archived into libscaly.a;
// the Scaly-side extern declarations live in packages/scaly/0.1.0/scaly/
// fiber.scaly. One file serves both OSes: Mach-O mangles C names with a
// leading underscore, ELF does not — SYM() below absorbs the difference.
//
// Frame layout (16-byte aligned, 0xb0 = 176 bytes):
//   0x00-0x3f  d8-d15
//   0x40-0x8f  x19-x28
//   0x90       x29 (fp)
//   0x98       x30 (lr — return address if the resumed code executes `ret`)
//   0xa0       pc  (resume address; for a fresh context: the entry function)
//   0xa8       (padding)
//
// scaly_make_context(stack_top, fn, stack_base) -> ctx
//   stack_base (x2) is IGNORED here and read only by the Win64 file, which
//   needs the stack's low address for the TIB bounds it has to swap on every
//   switch. One declaration serves every target because one seed does, so the
//   argument is passed unconditionally; here it costs a register nothing
//   reads. See fcontext_x86_64_win.S.
//
//   Carves a fresh frame below stack_top. The first jump into it enters fn
//   with x0 = the jumper's context. fn must never return (lr is a trap).
//
// scaly_jump_context(to) -> ctx
//   Suspends the current context, resumes `to`. The resumed side receives
//   the jumper's fresh context: a fresh context's entry fn gets it as its
//   argument; a suspended context gets it as scaly_jump_context's return.

#if defined(__APPLE__)
#define SYM(name) _##name
#define LABEL(name) L##name
#else
#define SYM(name) name
#define LABEL(name) .L##name
#endif

.text
.balign 4

.globl SYM(scaly_make_context)
#if defined(__ELF__)
.type scaly_make_context, %function
#endif
SYM(scaly_make_context):
    // x0 = stack top (high address), x1 = entry fn
    and  x0, x0, #0xFFFFFFFFFFFFFFF0   // 16-byte align down
    sub  x0, x0, #0xb0                 // carve context frame
    str  x1, [x0, #0xa0]               // pc = entry fn
    adr  x1, LABEL(ctx_return)
    str  x1, [x0, #0x98]               // lr = trap trampoline
    str  xzr, [x0, #0x90]              // fp = 0 (frame chain terminator)
    ret

LABEL(ctx_return):
    // A fiber entry function must never return (milestone 1.1 semantics;
    // fiber completion arrives with the scheduler in milestone 1.3).
    brk  #0

.globl SYM(scaly_jump_context)
#if defined(__ELF__)
.type scaly_jump_context, %function
#endif
.balign 4
SYM(scaly_jump_context):
    // x0 = target context
    sub  sp, sp, #0xb0
    stp  d8,  d9,  [sp, #0x00]
    stp  d10, d11, [sp, #0x10]
    stp  d12, d13, [sp, #0x20]
    stp  d14, d15, [sp, #0x30]
    stp  x19, x20, [sp, #0x40]
    stp  x21, x22, [sp, #0x50]
    stp  x23, x24, [sp, #0x60]
    stp  x25, x26, [sp, #0x70]
    stp  x27, x28, [sp, #0x80]
    stp  x29, x30, [sp, #0x90]
    str  x30, [sp, #0xa0]              // resume pc = our return address
    mov  x3, sp                        // x3 = jumper's context
    mov  sp, x0                        // switch stacks
    mov  x0, x3                        // deliver jumper's context to resumed side
    ldp  d8,  d9,  [sp, #0x00]
    ldp  d10, d11, [sp, #0x10]
    ldp  d12, d13, [sp, #0x20]
    ldp  d14, d15, [sp, #0x30]
    ldp  x19, x20, [sp, #0x40]
    ldp  x21, x22, [sp, #0x50]
    ldp  x23, x24, [sp, #0x60]
    ldp  x25, x26, [sp, #0x70]
    ldp  x27, x28, [sp, #0x80]
    ldp  x29, x30, [sp, #0x90]
    ldr  x4, [sp, #0xa0]
    add  sp, sp, #0xb0
    ret  x4

#if defined(__ELF__)
// Non-executable stack marker (silences lld/GNU ld execstack warnings).
.section .note.GNU-stack,"",%progbits
#endif
