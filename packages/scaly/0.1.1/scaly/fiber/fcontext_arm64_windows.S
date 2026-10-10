// Scaly fiber context switch — arm64 Windows (AAPCS64 as Windows uses it).
//
// ★Written 2026-10-02 on a Mac, run the same day on the first Windows-on-arm64
// machine: a fiber switches, all eighteen callee-saved registers survive
// (tests/win32/d8_check_arm64.S), the stack overflow report arrives in 96 of
// 96 landing points, and a catch point on a fiber unwinds since the headroom
// in scaly_make_context below.
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
    // 16 bytes stay free above the entry function's frame. Without them its
    // stack pointer at entry IS the TEB's StackBase, and that value is the
    // target frame of a catch point in the entry function (`_setjmpex` takes
    // llvm.sponentry): RtlUnwindEx refuses a frame that is not BELOW the
    // base and raises STATUS_BAD_STACK (measured 2026-10-02,
    // tests/fiber/panic_chain: the fiber's own `try` died 0xC0000028).
    sub  x0, x0, #0x10
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

// ---- the catch point: scaly_setjmp / scaly_longjmp ----------------------
//
// The panic shim's catch point on Windows (scaly/memory/panic.c).
// The CRT's longjmp UNWINDS through SEH (RtlUnwindEx) on arm64 -- a
// `Frame` of 0 does not opt out there, measured -- and that needs registered
// unwind data for every frame between the throw and the catch, which code the
// JIT generated does not have. These two do what longjmp does on POSIX: save
// the callee-saved state at the catch point, restore it at the throw, and
// return a second time from scaly_setjmp. Nothing between is unwound, which
// is all Scaly needs: its own unwind (the region stack) runs from the catch
// frame's record, not from the stack.
//
// Buffer (256 bytes, from scaly_catch_push): x19..x28 at 0..72, fp/lr at
// 80/88, sp at 96, d8..d15 at 104..160, fpcr at 168. x18 is the TEB and is
// never touched. Both are leaf routines with no frame of their own.

.p2align 2
.globl scaly_setjmp
scaly_setjmp:
    stp   x19, x20, [x0, #0x00]
    stp   x21, x22, [x0, #0x10]
    stp   x23, x24, [x0, #0x20]
    stp   x25, x26, [x0, #0x30]
    stp   x27, x28, [x0, #0x40]
    stp   x29, x30, [x0, #0x50]
    mov   x2, sp
    str   x2, [x0, #0x60]
    stp   d8,  d9,  [x0, #0x68]
    stp   d10, d11, [x0, #0x78]
    stp   d12, d13, [x0, #0x88]
    stp   d14, d15, [x0, #0x98]
    mrs   x2, fpcr
    str   x2, [x0, #0xa8]
    mov   w0, #0
    ret

.p2align 2
.globl scaly_longjmp
scaly_longjmp:
    ldp   x19, x20, [x0, #0x00]
    ldp   x21, x22, [x0, #0x10]
    ldp   x23, x24, [x0, #0x20]
    ldp   x25, x26, [x0, #0x30]
    ldp   x27, x28, [x0, #0x40]
    ldp   x29, x30, [x0, #0x50]
    ldr   x2, [x0, #0x60]
    mov   sp, x2
    ldp   d8,  d9,  [x0, #0x68]
    ldp   d10, d11, [x0, #0x78]
    ldp   d12, d13, [x0, #0x88]
    ldp   d14, d15, [x0, #0x98]
    ldr   x2, [x0, #0xa8]
    msr   fpcr, x2
    cmp   w1, #0                    // longjmp(buf, 0) returns 1, as in C
    csinc w0, w1, wzr, ne
    ret                             // to scaly_setjmp's caller, a second time
