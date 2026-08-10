; guard_probe.asm — the two things C cannot express on x64 MSVC.
;
; ★LOCAL INSTRUMENT, MASM (ml64), NOT PART OF ANY CI RUNG. Every other assembly
; file in this tree is GAS syntax and built by clang, because that is what CI
; has. This one is MASM because the Windows box has the VS Build Tools and no
; clang, and the measurement it enables was worth more than the uniformity. If
; you wire it into the workflow, port it to GAS first — ml64 is not installed on
; the runner. tests/win32/WINDOWS-BOX.md section 5 has the build line.
;
; switch_stack(rcx = stack top, rdx = entry) — move onto the hand-made stack and
; call the entry. Never returns: the process dies on that stack, which is the
; whole point of the instrument.
;
; recurse_asm(rcx = frame bytes) — a recursion with an EXACT frame step whose
; touched byte is the LOWEST one, so the faulting address IS rsp. A C function
; cannot give that: a large `volatile char[]` gets _chkstk probing, which walks
; the guard page while rsp is still high and would flatter every number.

.code

switch_stack PROC
    mov     rsp, rcx
    xor     rbp, rbp
    sub     rsp, 32                 ; shadow space, keeps rsp 16-aligned
    call    rdx
    hlt
switch_stack ENDP

recurse_asm PROC
    sub     rsp, rcx                ; carve a frame of rcx bytes
    mov     byte ptr [rsp], 1       ; touch its LOWEST byte: the faulting store
    sub     rsp, 32                 ; shadow space for the call
    call    recurse_asm             ; rcx is untouched, so it carries through
    add     rsp, 32
    add     rsp, rcx
    ret
recurse_asm ENDP

END
