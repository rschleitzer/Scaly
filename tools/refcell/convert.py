#!/usr/bin/env python3
"""Rewrite the pointer-pointee OUT-CELLS scan.py reports as CELL.

Three edits, and they must land TOGETHER -- unlike `refout`, this conversion is
not parameter-list-only, because the caller's cell moves with the parameter:

  1. the parameter:            `out: pointer[pointer[X]]`  ->  `out: ref[ref[X]?]`
  2. the body's stores:        `set *out: v`               ->  `set out: v`
  3. the caller's cell:        `var c: pointer[X] null`    ->  `var c: ref[X]? null`

Step 3 is not optional and that is MEASURED, not assumed: `&c` on a
`var c: pointer[X] null` yields `ref[pointer[X]]`, which does NOT match a
`ref[ref[X]?]` parameter -- the call fails as `function not found`.  A
half-converted tree therefore does not silently compile; it stops at the call
site, which is the good direction for a sweep to fail in.

Step 2 is not optional either: after step 1 the parameter is a NON-optional
`ref`, so `set *out:` is a hard rc-4 from report_deref_of_ref#.

The tool cannot see types and does not pretend to -- the COMPILER is the
arbiter.  A file the build rejects is reverted whole by the driver.
"""
import re, os, sys, collections, importlib.util

def _load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec); spec.loader.exec_module(mod)
    return mod

_HERE = os.path.dirname(os.path.abspath(__file__))
# Two modules both named scan.py: refout's carries the shared parsing, this
# directory's the pointer-pointee verdicts. Loaded by PATH so neither shadows
# the other on sys.path -- the same trap a name-keyed index has.
_shared = _load('refout_scan', os.path.join(_HERE, '..', 'refout', 'scan.py'))
_cellscan = _load('refcell_scan', os.path.join(_HERE, 'scan.py'))
collect, signature, split_args = _shared.collect, _shared.signature, _shared.split_args
body_lines = _shared.body_lines
DECL = _shared.DECL
CALL, PEELED = _shared.CALL, _shared.PEELED
candidates, analyse, frozen_names = _cellscan.candidates, _cellscan.analyse, _cellscan.frozen_names

def cells(paths):
    routines = collect(paths)
    cand = candidates(routines, frozen_names())
    verdict = analyse(routines, cand)
    return routines, {k: cand[k] for k in cand if verdict[k] == 'CELL'}

def caller_locals(routines, cell_keys):
    """`var NAME: pointer[X] null` locals passed as `&NAME` into a converted slot."""
    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines): by_name[r['fn']].append(ri)
    want = collections.defaultdict(set)     # file -> {(name, X)}
    for ri, r in enumerate(routines):
        body = r['body']
        for m in CALL.finditer(body):
            callee = m.group(1)
            if callee not in by_name: continue
            args = split_args(body, m.end() - 1)
            for cri in by_name[callee]:
                params = routines[cri]['params']
                offs = [0] + ([1] if params and params[0][0] in PEELED else [])
                for off in offs:
                    for k, a in enumerate(args):
                        key = (cri, k + off)
                        if key not in cell_keys: continue
                        am = re.match(r'^\s*&(\w+)\s*$', a)
                        if am: want[r['file']].add((am.group(1), cell_keys[key]))
    return want

def main(paths, dry=False, only=None):
    routines, cell = cells(paths)
    if only:
        cell = {k: v for k, v in cell.items() if only in routines[k[0]]['file']}
    locals_by_file = caller_locals(routines, cell)

    per_file = collections.defaultdict(list)
    for (ri, pi), ty in cell.items():
        r = routines[ri]
        per_file[r['file']].append((r['line'], r['params'][pi][0], ty))

    files = set(per_file) | set(locals_by_file)
    total_p = total_s = total_l = 0
    for path in sorted(files):
        lines = open(path, encoding='utf8').read().split('\n')
        np = ns = nl = 0
        names = []
        for line, nm, ty in per_file.get(path, []):
            i = line - 1
            sig, last = signature(lines, i)
            pat = re.compile(r'(\b%s\s*:\s*)pointer\[\s*pointer\[\s*%s\s*\]\s*\]'
                             % (re.escape(nm), re.escape(ty)))
            hit = False
            for k in range(i, last + 1):
                end = len(lines[k].split(';')[0])
                new, c = pat.subn(r'\1ref[ref[%s]?]' % ty, lines[k][:end])
                if c: lines[k] = new + lines[k][end:]; hit = True; np += c
            if hit: names.append(nm)
            else: print(f'  MISS param {path}:{line} {nm}', file=sys.stderr)
        # The stores, scoped to the OWNING ROUTINE and never to the file.
        # `tools/refstar.py`'s first draft was file-scoped and swept `set *a:`
        # where `a` was a let-bound POINTER, because a different routine in the
        # same file had a parameter of that name -- and a `set` on a let-bound
        # pointer is silently dropped. A name-keyed set answers about the wrong
        # name; `set *p:` on a real pointer is still perfectly legal today, so
        # nothing downstream would have caught it.
        for line, nm, ty in per_file.get(path, []):
            if nm not in names: continue
            i = line - 1
            _, last = signature(lines, i)
            span = last + len(body_lines(lines, last, re.match(r'(\s*)', lines[i]).group(1)))
            pat = re.compile(r'(\bset\s+)\*(%s\s*:)' % re.escape(nm))
            for k in range(i, min(span + 1, len(lines))):
                end = len(lines[k].split(';')[0])
                new, c = pat.subn(r'\1\2', lines[k][:end])
                if c: lines[k] = new + lines[k][end:]; ns += c
        # The caller's cell, in BOTH spellings. The second one is why this loop
        # is not one regex: `var x null as pointer[X]` is the
        # form that hides from every pass, because the occurrence sits after an
        # `as` and reads as a cast -- 148 sites hid there once. This tool walked
        # into it too: tscaly reported ZERO caller cells for two conversions
        # whose callers all use the cast spelling.
        for nm, ty in sorted(locals_by_file.get(path, [])):
            annotated = re.compile(r'(\b(?:var|let)\s+%s\s*:\s*)pointer\[\s*%s\s*\]'
                                   % (re.escape(nm), re.escape(ty)))
            casted = re.compile(r'\b(var|let)\s+(%s)\s+null\s+as\s+pointer\[\s*%s\s*\]'
                                % (re.escape(nm), re.escape(ty)))
            for k, ln in enumerate(lines):
                end = len(ln.split(';')[0])
                code = ln[:end]
                code, c1 = annotated.subn(r'\1ref[%s]?' % ty, code)
                code, c2 = casted.subn(r'\1 \2: ref[%s]? null' % ty, code)
                if c1 or c2: lines[k] = code + ln[end:]; nl += c1 + c2
        if (np or ns or nl) and not dry:
            open(path, 'w', encoding='utf8').write('\n'.join(lines))
        print(f'{np:4} params {ns:4} stores {nl:4} locals  {path}')
        total_p += np; total_s += ns; total_l += nl
    print(f'{total_p} parameters, {total_s} stores, {total_l} caller cells'
          + (' (dry run)' if dry else ''))

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    only = next((a.split('=')[1] for a in sys.argv[1:] if a.startswith('--only=')), None)
    main(args or ['packages'], '--dry' in sys.argv, only)
