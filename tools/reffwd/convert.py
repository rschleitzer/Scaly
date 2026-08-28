#!/usr/bin/env python3
"""Apply what `reffwd/scan.py` finds: re-declare a forwarded pointer parameter
as the `ref` its callee already takes.

Only the DECLARATION moves.  A pointer argument passes to a `ref` parameter
without a cast, so no call site of a converted routine has to change -- with
one measured exception the scan reports as `pointer[pointer[X]]`: a caller's
own cell (`var c: pointer[X] null`) yields `ref[pointer[X]]` under `&`, which
does not match `ref[ref[X]?]`, so a half-converted tree stops at the call site
rather than compiling silently.  That is a feature; the compiler names it.

The bodies need no sweep: the scan only reports parameters no body derefs.
"""
import re, sys, os, collections, importlib.util

_here = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location('reffwd_scan', _here + '/scan.py')
scan = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(scan)


def ref_form(inner):
    """`pointer[X]` -> `ref[X]`; `pointer[pointer[X]]` -> `ref[ref[X]?]`.

    The nullable spelling on the inner half is the one the refcell campaign
    settled on: a cell that holds a reference holds an Option, because null
    is what an unset cell holds.
    """
    deeper = scan.pointee(inner)
    return f'ref[ref[{deeper}]?]' if deeper else f'ref[{inner}]'


def main(paths, apply):
    routines = scan.collect(paths)
    cand, verdict, _ = scan.analyse(routines)
    per_routine = collections.defaultdict(list)
    for (ri, pi), inner in cand.items():
        if verdict[(ri, pi)] == 'CELL':
            per_routine[ri].append((routines[ri]['params'][pi][0], inner))

    edits = collections.defaultdict(dict)
    for ri, cells in per_routine.items():
        r = routines[ri]
        lines = open(r['file'], encoding='utf-8').read().split('\n')
        # the declaration may wrap; walk from its first line to the closing paren
        i = r['line'] - 1
        txt = scan.refout.strip_comment(lines[i])
        depth = txt.count('(') - txt.count(')')
        j = i
        while depth > 0 and j + 1 < len(lines):
            j += 1
            nxt = scan.refout.strip_comment(lines[j])
            depth += nxt.count('(') - nxt.count(')')
        for k in range(i, j + 1):
            new = lines[k]
            for nm, inner in cells:
                new = re.sub(r'(\b%s\s*:\s*)pointer\[%s\]' %
                             (re.escape(nm), re.escape(inner)),
                             lambda m: m.group(1) + ref_form(inner), new)
            if new != lines[k]:
                edits[r['file']][k] = new

    n = 0
    for f, per_line in sorted(edits.items()):
        lines = open(f, encoding='utf-8').read().split('\n')
        for k, new in sorted(per_line.items()):
            print(f"{f}:{k+1}\n  - {lines[k].strip()}\n  + {new.strip()}")
            lines[k] = new
            n += 1
        if apply:
            open(f, 'w', encoding='utf-8').write('\n'.join(lines))
    print(f"--- {n} declaration lines, {sum(len(v) for v in per_routine.values())} parameters"
          + ("" if apply else "  (dry run; pass --apply)"))


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    main(args or ['packages'], '--apply' in sys.argv)
