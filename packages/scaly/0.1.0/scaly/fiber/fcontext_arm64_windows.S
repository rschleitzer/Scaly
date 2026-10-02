// Scaly fiber context switch — arm64 Windows (AAPCS64 as Windows uses it).
//
// ★WRITTEN 2026-10-02 ON A MAC AND NOT YET RUN. It assembles for
// aarch64-pc-windows-msvc; whether a fiber switches, survives a stack
// overflow report and unwinds on it is for the first Windows-on-arm64 machine
// to say. tests/win32/WINDOWS-BOX.md has the plan.
//
// A separate file from fcontext_arm64.S for ONE of the four reasons the x64
// Windows file has (fcontext_x86_64_windows.S): the register conventions are
// AAPCS64's on both systems — arguments in x0..x2, x19..x28, fp, lr and
// d8..d15 callee-saved, no shadow space — but Windows keeps the current
// stack's bounds in the thread's TEB, which the OS itself reads (stack
// overflow dispatch, guard-page growth, the unwinder's frame validation). A
// fiber on another stack must swap them with everything else.
//
// The TEB is reached through x18, which Windows RESERVES for it (never a
// scratch register there, unlike Linux); the fields are those of the x64 TIB
// at the same offsets:
//   0x08  StackBase  (the HIGH address)     0x10  StackLimit (the LOW address)
//   0x20  FiberData                         0x1478  DeallocationStack
//
// FRAME LAYOUT (0xd0 bytes, 16-byte aligned):
//   0x00-0x3f  d8-d15
//   0x40-0x8f  x19-x28
//   0x90       x29 (fp)
//   0x98       x30 (lr — for a fresh context: the never-return trap)
//   0xa0       pc  (resume address; for a fresh context: the entry function)
//   0xa8       TEB fiber data
//   0xb0       TEB stack limit
//   0xb8       TEB stack base
//   0xc0       TEB deallocation stack
//   0xc8       (padding)
//
// scaly_make_context(stack_top, fn, stack_base) -> ctx
// scaly_jump_context(to) -> ctx
//   The resumed side receives the jumper's context in x0: a fresh context's
//   entry function as its argument, a suspended one as the return value.
//
// COFF does not prefix C symbols on arm64, so the names are written plain.

.text
.balign 4

.globl scaly_make_context
scaly_make_context:
    // x0 = stack top (high), x1 = entry fn, x2 = stack base (low)
    mov  x3, x0                         // keep the unaligned top for the TEB
    and  x0, x0, #0xFFFFFFFFFFFFFFF0    // 16-byte align down
    sub  x0, x0, #0xd0                  // carve the context frame
    str  x1, [x0, #0xa0]                // pc = entry fn
    adr  x1, ctx_return
    str  x1, [x0, #0x98]                // lr = never-return trap
    str  xzr, [x0, #0x90]               // fp = 0 (frame chain terminator)
    str  xzr, [x0, #0xa8]               // fiber data: none
    str  x2, [x0, #0xb0]                // stack limit = base (low address)
    str  x3, [x0, #0xb8]                // stack base  = top  (high address)
    str  x2, [x0, #0xc0]                // dealloc stack = the mapping's base
    // d8-d15 and x19-x28 zeroed: a fresh fiber's register state is a fact,
    // not what the stack happened to hold
    stp  xzr, xzr, [x0, #0x00]
    stp  xzr, xzr, [x0, #0x10]
    stp  xzr, xzr, [x0, #0x20]
    stp  xzr, xzr, [x0, #0x30]
    stp  xzr, xzr, [x0, #0x40]
    stp  xzr, xzr, [x0, #0x50]
    stp  xzr, xzr, [x0, #0x60]
    stp  xzr, xzr, [x0, #0x70]
    stp  xzr, xzr, [x0, #0x80]
    ret

ctx_return:
    // A fiber entry function must never return.
    brk  #0xf000

.globl scaly_jump_context
.balign 4
scaly_jump_context:
.seh_proc scaly_jump_context
    sub  sp, sp, #0xd0
.seh_stackalloc 0xd0
.seh_endprologue
    // x0 = target context
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
    str  x30, [sp, #0xa0]               // resume pc = our return address
    ldr  x5, [x18, #0x20]               // TEB fiber data
    str  x5, [sp, #0xa8]
    ldr  x5, [x18, #0x10]               // TEB stack limit
    str  x5, [sp, #0xb0]
    ldr  x5, [x18, #0x08]               // TEB stack base
    str  x5, [sp, #0xb8]
    ldr  x5, [x18, #0x1478]             // TEB deallocation stack
    str  x5, [sp, #0xc0]

    mov  x3, sp                         // x3 = the jumper's context
    mov  sp, x0                         // switch stacks
    mov  x0, x3                         // deliver it to the resumed side

    ldr  x5, [sp, #0xa8]
    str  x5, [x18, #0x20]
    ldr  x5, [sp, #0xb0]
    str  x5, [x18, #0x10]
    ldr  x5, [sp, #0xb8]
    str  x5, [x18, #0x08]
    ldr  x5, [sp, #0xc0]
    str  x5, [x18, #0x1478]
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
.seh_startepilogue
    add  sp, sp, #0xd0
.seh_stackalloc 0xd0
.seh_endepilogue
    ret  x4
.seh_endproc
