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

★★★THE 2026-08-27 BLOCKER IS LIFTED (measured 2026-08-30). It read: "the
subscript does not dispatch through an OPTION receiver", so on the ordinary
port field shape `ref[Array[T]]?` the `[i]` was left standing as an orphan
operand. It no longer is -- probed in all four shapes, each compiled and RUN
against `scalyc/build/scalyc`:

    function pick(a: ref[Array[int]]?, i: size_t) returns int   a[i]     -> 20
    function poke(a: ref[Array[int]]?, i: size_t, v: int)  a.put(i, v)   -> 77
    function pick2(a: ref[Vector[int]]?, i: size_t) returns int  a[i]    ->  5

so the Option receiver takes the READ and the WRITE alike, and `Vector` as
well as `Array`. Nothing in this file was changed to make that true; one of
the ref/Option sweeps of the intervening days fixed the dispatch, and the note
had simply outlived it. ★A blocker recorded against a COMPILER gap owes a
re-probe before it is believed -- reading it was three days of not running
this tool.

★★★WHAT THE TOOL STILL CANNOT SEE IS THE RECEIVER'S TYPE, and that is what
decides whether `X[i]` EXISTS at all: `Array`, `Vector` and `Slice` carry an
`operator []`; `String` and the ports' own `StringC` carry a `get_buffer()`
and NO subscript. So the rewrite is PROPOSED here and JUDGED by the compiler
-- `tools/subscript_driver.py` emits every package root, reverts exactly the
lines the compiler rejects, and repeats. 11 of 78 proposals were reverted that
way on 2026-08-30 (`prog[pi]`, `lo.name[t]`, `t4[3]` -- all String receivers,
plus one `Array[u16]` copy the planner would not subscript). ★The revert is at
LINE granularity, not by file: reverting a whole file for one bad site throws
away the good sites in it. ★And a CONFORMANCE error is reported at the
FUNCTION's line, not the expression's, so the driver falls back to the nearest
changed line at or after the one the compiler named.

★★★AND IT MUST NEVER TOUCH A `set` TARGET. `operator []` answers the element
BY VALUE, so there is no slot behind `a[i]` to store into: `set a[i]: v` on a
container compiles, runs, and is SILENTLY DROPPED -- rc 0, no diagnostic. The
first run of this tool (2026-08-27, commit 25614a02) rewrote FOUR `set`
targets out of 74 sites, and every one of them was a live miscompile that
stood for a day: dazzle's `#!optional`/`#!key` DEFAULTS were never stored
(`SchemeParser.parse_formals`), so every default read back as `#f`, and the
root-rule specificity sort never swapped (`ProcessingMode`). Five backend
suites plus `tests/dazzle/engine` went red, and the engine red was written
down as "not ours". `set NAME[i]: v` on a POINTER is a real store (pointer
arithmetic) and stays legal -- which is exactly why the wrong ones looked
right. The rule below refuses the target half of any `set`, whatever the
receiver; the write on a container is `put`.

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

# ★A POINTER receiver must be left alone, and this is not a nicety: on a
# `pointer[Array[T]]` the subscript IS pointer arithmetic with an Array-sized
# stride, so `a[i]` answers a whole Array[T] where `*(a.get_buffer() + i)`
# answered an element. The member access auto-derefs the pointer; the
# subscript does not. Found the hard way in tscaly's SymbolDump.sorted_order,
# where `a` is `host.allocate(...) as pointer[Array[int]]` -- shape 3 of the
# pointer doctrine, still in use. Conservative BY FILE: a name declared a
# pointer anywhere in the file is skipped everywhere in it, over-skipping
# rather than guessing at scope.
PTR_DECL = re.compile(r"(?:\b(?:let|var)\s+([A-Za-z_][A-Za-z0-9_']*)\b[^;\n]*\bas\s+pointer\[)"
                      r"|(?:\b([A-Za-z_][A-Za-z0-9_']*)\s*:\s*pointer\[)")

def pointer_names(text):
    out = set()
    for line in text.split("\n"):
        code = line.split(";")[0]
        for m in PTR_DECL.finditer(code):
            out.add(m.group(1) or m.group(2))
    return out

def set_source_start(line):
    """For a `set <target>: <source>` line, the offset where the SOURCE begins.

    0 for any other line. The split is the first `:` at bracket depth 0 after
    `set ` -- a target may itself be subscripted (`set a[i].f: v`) or a member
    chain, and only the source half may be rewritten. See the header: a
    subscript in the TARGET half is a store the language drops in silence.
    """
    stripped = line.lstrip()
    if not stripped.startswith("set "):
        return 0
    depth = 0
    i = len(line) - len(stripped) + 4
    while i < len(line):
        c = line[i]
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
        elif c == ":" and depth == 0:
            return i + 1
        i += 1
    return len(line)                       # no source half: convert nothing


def convert_line(line, skip=frozenset()):
    head_len = set_source_start(line)
    head, line = line[:head_len], line[head_len:]
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
            # ★Check EVERY segment, not just the head: the receiver is usually
            # `this.vec`, and a pointer FIELD is declared under its own name --
            # testing only `this` let `pointer[Array[ref[Attribute]?]]` through
            # and the emitter trapped with `member not found: Array.specified`
            # one stage later, where no line number points back here.
            if any(seg in skip for seg in recv.strip().split(".")):
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
            return head + out, changed

def main():
    apply = "--apply" in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith("--")]
    total = 0
    for p in files:
        text = open(p, encoding="utf8").read()
        skip = pointer_names(text)
        lines = text.split("\n")
        n = 0
        for k, l in enumerate(lines):
            code_end = len(l)
            semi = l.find(";")
            if semi >= 0 and CALL not in l[:semi]:
                continue                      # comment-only occurrence
            new, c = convert_line(l, skip)
            if c:
                lines[k] = new
                n += c
        if n and apply:
            open(p, "w", encoding="utf8").write("\n".join(lines))
        if n:
            print("%5d  %s" % (n, p))
        total += n
    print("SUMME:", total, "(angewendet)" if apply else "(nur gezählt)")

# ★Under `if __name__`, and that is not style: `tools/subscript_local.py`
# imports the guards from this file, and a bare `main()` at module scope ran
# THIS tool -- with the importer's `--apply` and the importer's file list --
# every time that one started. It re-applied sites the driver had just
# reverted, silently, and the only reason the tree survived it is that the
# driver runs again afterwards.
if __name__ == '__main__':
    main()
