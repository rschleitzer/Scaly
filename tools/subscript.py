#!/usr/bin/env python3
"""Rewrite `*(X.get_buffer() + I)` as `X[I]`.

The ports walk container buffers by hand: `*(list.get_buffer() + i)` is
`list[i]` spelled as pointer arithmetic. Since 2026-08-27 `operator []`
answers the element BY VALUE and traps on a bad index, so the two forms mean
the same thing and the subscript is the one a reader of Clark's C++ or of
typescript-go recognises.

NOT emission-neutral: the subscript carries a bounds check the hand-walk does
not. The check is free once the trap call is `noreturn` (measured), but the
checksum for this conversion is the SUITES plus a timing A/B, never an opcode
comparison.

★★★BLOCKED as of 2026-08-27, and the blocker is a COMPILER gap, not a tool
one: **the subscript does not dispatch through an OPTION receiver.** A member
access unwraps the pointer/ref/Option cascade (s99); `find_subscript_operator`
does not, so on the ordinary port field shape `ref[Array[T]]?` the operand is
left standing and `[i]` becomes an orphan --

    function pick(a: ref[Array[int]]?, i: size_t) returns int   a[i]
    -> error: unexpected operand - missing operator, ... before this statement
    -> error: the returned value does not conform ... Option[ref[Array[int]]]
       where int is declared

while `*(a.get_buffer() + i)` on the SAME receiver is fine. That is the
standing bug pattern of this compiler one more time: a name test that does not
know about the s113 Option wrapper. Fix the dispatch before running this tool
over the ports -- a dry run on tscaly/parser.scaly converted 72 sites and three
of them could not compile for exactly this reason.

Usage: tools/subscript.py [--apply] <file.scaly>...
"""
import re, sys

CALL = ".get_buffer()"

def balanced_index(s, start):
    """s[start:] is the index expression up to the matching `)` of the `*(`.
    Returns (index_text, position_after_the_closing_paren) or None."""
    depth = 1
    i = start
    while i < len(s):
        c = s[i]
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
            if depth == 0:
                return s[start:i], i + 1
        i += 1
    return None

RECV = re.compile(r"[A-Za-z_][A-Za-z0-9_.']*$")

def convert_line(line):
    out = line
    changed = 0
    while True:
        m = out.find("*(")
        found = False
        pos = 0
        while True:
            m = out.find("*(", pos)
            if m < 0:
                break
            inner = balanced_index(out, m + 2)
            if inner is None:
                pos = m + 2
                continue
            expr, after = inner
            # expr must read `<receiver>.get_buffer() + <index>`
            cut = expr.find(CALL)
            if cut < 0:
                pos = m + 2
                continue
            recv = expr[:cut]
            if not RECV.fullmatch(recv.strip()):
                pos = m + 2
                continue
            rest = expr[cut + len(CALL):].lstrip()
            if not rest.startswith("+"):
                pos = m + 2
                continue
            idx = rest[1:].strip()
            if not idx:
                pos = m + 2
                continue
            out = out[:m] + recv.strip() + "[" + idx + "]" + out[after:]
            changed += 1
            found = True
            break
        if not found:
            return out, changed

def main():
    apply = "--apply" in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith("--")]
    total = 0
    for p in files:
        lines = open(p, encoding="utf8").read().split("\n")
        n = 0
        for k, l in enumerate(lines):
            code_end = len(l)
            semi = l.find(";")
            if semi >= 0 and CALL not in l[:semi]:
                continue                      # comment-only occurrence
            new, c = convert_line(l)
            if c:
                lines[k] = new
                n += c
        if n and apply:
            open(p, "w", encoding="utf8").write("\n".join(lines))
        if n:
            print("%5d  %s" % (n, p))
        total += n
    print("SUMME:", total, "(angewendet)" if apply else "(nur gezählt)")

main()
