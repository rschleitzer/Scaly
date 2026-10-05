#!/usr/bin/env python3
"""Turn a hoisted ELEMENT ADDRESS into a borrowed element reference:

    let node a.get_buffer() + id     ->   let node a.at(id)
    ... node.field ...                    ... node.field ...        (unchanged)
    ... set node.field: v ...             ... set node.field: v ... (unchanged)

This is the class `tools/derefcensus/scan.py` and the `get_buffer` census both
report as FOUNDATION, and the verdict was wrong: the sites bind a RECORD
POINTER and then use it as a REFERENCE, which is what `ref[T]` is.  Only the
BINDING moves -- every use below it already reads the way a `ref` reads,
because a member access unwraps the pointer/ref/Option cascade.

★★★WHY `at` AND NOT THE `get` THAT WAS ALREADY THERE: `a.get(i)` answers
`ref[T]?`, the lookup that may legitimately MISS, so the site needs
`a.get(i) as ref[T]` -- and that emits the call (with its own bounds check)
PLUS an `unwrap.isnull` PLUS `exit 21`: two branches for one question, and a
trap naming a mangled symbol where the fault is a bounds violation.  `at` is
the trapping twin, the same relation `[]` has to `get` for the value half.

★★★THE COMPILER IS THE ARBITER AND IT IS NOW A LOUD ONE.  The tool cannot see
types, and a receiver without an `at` (`String`, `Vector`, a raw pointer) is a
hard error rather than a silent miscompile.  Better still, a use this tool
should not have converted is loud too, by the five ref gates that landed in
2026-08: `*node` on a non-optional ref is rc-4, `node + 1` is rc-4
("arithmetic on a reference"), `node = null` is rc-4.  Run
`tools/subscript_driver.py` afterwards to revert by LINE what the compiler
rejects.

★The guards below are therefore about keeping the diff honest, not about
safety: a use that is not a member access is left alone rather than handed to
the compiler to reject.

★★★THE TWO-STEP SHAPE IS THE BIGGER HALF, and it needs one guard the one-step
shape does not.  Written out, the ports hoist the buffer once and take several
element addresses off it:

    let buf a.get_buffer()            (deleted once nothing uses it)
    let node buf + id           ->    let node a.at(id)

Here the CAPTURE POINT MOVES: `buf` froze the buffer address at the hoist,
`a.at(id)` reads whatever buffer `a` holds at the element line, and an Array
that GREW in between has reallocated.  The two therefore differ exactly where
the old code was already holding a dangling pointer -- so the rewrite would
silently repair a bug, or paper over one, and neither is a mechanical tool's
call.  Any routine that calls a mutator on the receiver (or re-seats it) is
skipped whole, which is `tools/subscript_local.py`'s rule for the same reason.

Usage: tools/refat.py [--apply] <file.scaly>...
"""
import re, sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from subscript_local import routine_spans, strip_comment, REBIND
from subscript import pointer_names

HOIST = re.compile(r"^(\s*)(?:let|var)\s+([A-Za-z_][A-Za-z0-9_']*)\s+"
                   r"([A-Za-z_][A-Za-z0-9_.']*)\.get_buffer\s*\(\s*\)\s*$")
ELEM = re.compile(r"^(\s*)((?:let|var)\s+([A-Za-z_][A-Za-z0-9_']*)\s+)"
                  r"([A-Za-z_][A-Za-z0-9_']*)\s*\+\s*(\S.*?)\s*$")
MUTATOR = r"(add|add_all|remove|reallocate|clear|resize|reserve)"
DEAD = object()          # a hoist marked for removal; filtered once, at the end 

BIND = re.compile(r"^(\s*)((?:let|var)\s+([A-Za-z_][A-Za-z0-9_']*)\s+)"
                  r"([A-Za-z_][A-Za-z0-9_.']*)\.get_buffer\s*\(\s*\)\s*\+\s*(\S.*?)\s*$")


def uses_are_member_only(codes, k, end, name):
    """Every occurrence of `name` below the binding is `name.` or `set name.`.

    A bare occurrence -- passed on as an argument, compared, address-taken --
    is left alone. It would very likely be correct (a `ref` and a `pointer`
    are the same address at LLVM level, so forwarding one is bit-identical),
    but "very likely" is not what a mechanical rewrite gets to assume.
    """
    seen = False
    for j in range(k + 1, end):
        code = codes[j]
        for m in re.finditer(rf'\b{re.escape(name)}\b', code):
            after = code[m.end():]
            if not after.startswith('.'):
                return False
            # ★The deref/address sigil is the character IMMEDIATELY before the
            # name -- `*node`, `&node`. Asking the RSTRIPPED prefix instead
            # reads the MULTIPLICATION in `node.rows * node.cols` as a deref of
            # the second occurrence, and that false positive silently held back
            # three `tensor.scaly` sites whose uses were member-only throughout.
            if m.start() > 0 and code[m.start() - 1] in '*&':
                return False
            seen = True
    return seen


def convert(lines, skip):
    changed = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k in range(len(body)):
            m = BIND.match(codes[k])
            if not m: continue
            indent, decl, local, recv, idx = m.groups()
            if any(seg in skip for seg in recv.split('.')): continue
            end = len(body)
            for j in range(k + 1, len(body)):
                if REBIND(local).match(codes[j]): end = j; break
            if not uses_are_member_only(codes, k, end, local): continue
            body[k] = f'{indent}{decl}{recv}.at({idx})'
            codes[k] = strip_comment(body[k])
            changed += 1
        lines[a:b] = body
    return lines, changed



def receiver_is_stable(codes, recv):
    """No mutator call on the receiver, and nothing re-seats it or its head."""
    head = recv.split('.')[0]
    for c in codes:
        if re.search(rf'\b{re.escape(recv)}\s*\.\s*{MUTATOR}\b', c): return False
        if re.search(rf'\bset\s+{re.escape(recv)}\b', c): return False
        if head != recv and re.search(rf'\bset\s+{re.escape(head)}\b', c): return False
    return True


def convert_hoisted(lines, skip):
    """`let buf a.get_buffer()` + `let node buf + id` -> `let node a.at(id)`."""
    changed = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k in range(len(body)):
            m = HOIST.match(codes[k])
            if not m: continue
            indent, buf, recv = m.groups()
            if any(seg in skip for seg in recv.split('.')): continue
            if not receiver_is_stable(codes, recv): continue
            end = len(body)
            for j in range(k + 1, len(body)):
                if REBIND(buf).match(codes[j]): end = j; break
            plan, other = [], False
            for j in range(k + 1, end):
                if not re.search(rf'\b{re.escape(buf)}\b', codes[j]): continue
                em = ELEM.match(codes[j])
                if not em or em.group(4) != buf:
                    other = True; continue
                e_end = end
                for j2 in range(j + 1, end):
                    if REBIND(em.group(3)).match(codes[j2]): e_end = j2; break
                if not uses_are_member_only(codes, j, e_end, em.group(3)):
                    other = True; continue
                plan.append((j, em))
            if not plan: continue
            for j, em in plan:
                e_indent, e_decl, _, _, idx = em.groups()
                body[j] = f'{e_indent}{e_decl}{recv}.at({idx})'
                changed += 1
            if not other:
                body[k] = DEAD                     # the hoist has no user left
            codes = [strip_comment(l) if l is not DEAD else '' for l in body]
        lines[a:b] = body
    # ★★★THE DELETION HAPPENS ONCE, AT THE END, AND THAT IS LOAD-BEARING.
    # `routine_spans` is computed ONCE over the whole file, so removing a line
    # inside this loop shifts every LATER span by one, and the scan then reads
    # one routine's bindings against another routine's text. Measured on the
    # first version: the tool stopped converging at all -- 3, then 8, then 8
    # sites per pass, each pass finding "new" work -- which is what a
    # misaligned span looks like from outside. The whole pass was reverted and
    # redone rather than inspected site by site.
    return [l for l in lines if l is not DEAD], changed


def main():
    apply = '--apply' in sys.argv
    total = 0
    for p in [x for x in sys.argv[1:] if not x.startswith('--')]:
        text = open(p, encoding='utf8').read()
        lines = text.split('\n')
        skip = pointer_names(text)
        new, c = convert(list(lines), skip)
        new, c2 = convert_hoisted(new, skip)
        c += c2
        if c:
            print('%5d  %s' % (c, p))
            if apply: open(p, 'w', encoding='utf8').write('\n'.join(new))
        total += c
    print('TOTAL:', total, '(applied)' if apply else '(counted only)')


if __name__ == '__main__':
    main()
