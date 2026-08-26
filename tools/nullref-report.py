#!/usr/bin/env python3
"""Turn `report_null_test_on_ref#` findings into a per-decision worklist.

The gate answers "this declaration lies"; it does not answer "what is the
truth", and that second question is not mechanical -- sometimes the fix is the
nullable spelling, sometimes the null test is dead and goes away, and for a
port the authority is the C++ reference, not the Scaly source.  So this is a
REPORT, never a rewriter.  ★It deliberately does NOT offer to insert the
`as ref[T]` unwraps that adding a `?` provokes: that cast asserts "proven
non-null here", and a tool writing it 983 times would be fabricating the proof
the exercise exists to establish -- worse than the original lie, because the
result would look audited.

Usage:  tools/nullref-report.py <compiler-log>...
where each log is the stderr of a `-c` compile of a package root built with a
compiler carrying the gate.  Re-derive the logs rather than trusting a stale
copy; the gate's list is authoritative and free.

The four kinds differ in fix and in RISK, which is the point of separating them:

  param    add `?`.  Callers are unaffected (an Option accepts what a plain ref
           accepted), but uses inside the body may need the explicit unwrap.
  local    add `?`.  Usually the initializer's RETURN type is the real culprit
           -- fix that first and the local often follows for free.
  return   add `?` to `returns`.  Highest blast radius: every caller's local
           inherits the Option, so expect this to create new findings.
  field    add `?` -- and CHECK THE GENERIC HOLD FIRST.

★★★THE GENERIC HOLD.  A `?` on a field of a generic concept whose own type
nests generics MISCOMPILES SILENTLY.  Measured 2026-08-26 on the stdlib's
`ArrayIterator[T].array`: with `ref[Array[T]]?` every instantiation of `next`
emitted `load %_Z5ArrayIiE` -- Array[int] regardless of T -- reading `length`
out of the wrong layout.  It passed all 217 regress tests because field 0 lines
up; the ONLY instrument that saw it was a byte-diff of the emitted IR.  The
cause is the `instantiate_generic` s116 cycle guard, which short-circuits
without binding the concept's generic parameters, and an `X?` field is exactly
the one extra generic level that pushes the walk into re-entry.  Such a slot
stays a `pointer[...]` until that defect is fixed.  Rows flagged GENERIC are
that hazard; do not add a `?` to one without an emission diff.
"""
import re, sys, os, collections

FIND = re.compile(r'^(packages/\S+?):(\d+):(\d+): error: '
                  r'null test on a non-optional reference: (\S+)')
NULLTEST = re.compile(r'(?:(?P<l>[A-Za-z_][\w.]*(?:\s*\([^()]*\))?)\s*(?:<>|=)\s*null'
                      r'|null\s*(?:<>|=)\s*(?P<r>[A-Za-z_][\w.]*(?:\s*\([^()]*\))?))')

def load(path):
    return open(path, encoding='utf-8').read().split('\n')

def enclosing(lines, ln):
    """(routine signature line index, define-header line index) above ln."""
    sig = rec = None
    for k in range(ln - 1, -1, -1):
        s = lines[k]
        if sig is None and re.match(r'\s*(function|procedure|operator|init)\b', s):
            sig = k
        if re.match(r'\s*define\s', s):
            rec = k
            break
    return sig, rec

def classify(lines, ln, name):
    """param / local / field / return / ? plus the declaration line, if found."""
    sig, rec = enclosing(lines, ln)
    if name.endswith(')'):
        return 'return', None, name.split('(')[0].split('.')[-1]
    if '.' in name:
        return 'field', None, name.split('.')[-1]
    if sig is not None:
        # the signature may span lines; read to the closing paren
        txt, d, k = '', 0, sig
        while k < len(lines):
            txt += lines[k]
            d += lines[k].count('(') - lines[k].count(')')
            if d <= 0 and '(' in txt:
                break
            k += 1
        if re.search(r'(?<![.\w])' + re.escape(name) + r'\s*:', txt):
            return 'param', sig, name
        for j in range(sig, ln):
            if re.match(r'\s*(let|var)\s+' + re.escape(name) + r'\b', lines[j]):
                return 'local', j, name
    return '?', None, name

def generic_owner(lines, rec):
    if rec is None:
        return False
    return bool(re.match(r'\s*define\s+[A-Za-z_]\w*\s*\[', lines[rec]))

def main(logs):
    rows, seen = [], set()
    for log in logs:
        for line in open(log, encoding='utf-8'):
            m = FIND.match(line)
            if not m:
                continue
            path, ln = m.group(1), int(m.group(2))
            if (path, ln) in seen:
                continue
            seen.add((path, ln))
            src = load(path)
            text = src[ln - 1] if ln - 1 < len(src) else ''
            code = text.split(';')[0]
            mm = NULLTEST.search(code)
            nm = (mm.group('l') or mm.group('r')).strip() if mm else '?'
            kind, decl, base = classify(src, ln - 1, nm)
            _, rec = enclosing(src, ln - 1)
            rows.append((path, ln, kind, base, m.group(4),
                         generic_owner(src, rec), text.strip()))

    kinds = collections.Counter(r[2] for r in rows)
    gen = sum(1 for r in rows if r[5])
    print(f'{len(rows)} findings')
    for k, v in kinds.most_common():
        print(f'  {v:5d}  {k}')
    print(f'  {gen:5d}  of them inside a GENERIC concept (check the hold)')
    print()
    byfile = collections.Counter(r[0] for r in rows)
    print('by file, heaviest first:')
    for path, v in byfile.most_common():
        ks = collections.Counter(r[2] for r in rows if r[0] == path)
        print(f'  {v:4d}  {path.split("/0.1.0/")[-1]:42s} '
              + ' '.join(f'{k}={n}' for k, n in sorted(ks.items())))
    out = os.environ.get('NULLREF_OUT', '/tmp/nullref-worklist.txt')
    with open(out, 'w') as fh:
        for path, ln, kind, base, ty, g, text in sorted(rows):
            fh.write(f'{path}:{ln}\t{kind}\t{base}\t{ty}'
                     f'\t{"GENERIC" if g else ""}\t{text}\n')
    print(f'\nworklist -> {out}')

main(sys.argv[1:])
