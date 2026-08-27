#!/usr/bin/env python3
"""Sweep the pointer deref off a REFERENCE: `set *r: v` becomes `set r: v`.

A `ref[T]` now reads and writes as its pointee, so `*` on one is the pointer
idiom it replaces.

★ SCOPED TO PARAMETERS, and the first draft was not.  A file-wide "names
declared `ref[` somewhere" set swept `set *a:` where `a` was a let-bound
POINTER from `host.allocate(...) as pointer[ElementType]`, because a
*different* routine in the same file has a parameter named `a: ref[...]`.  A
`set` on a let-bound pointer is silently dropped, so 811 lines went out of
opensp's root and a whole caller frame out of tscaly's -- caught only by the
byte-identity checksum.  A name-keyed set answers about the wrong name.

★ Only a non-optional `ref[T]` PARAMETER of the enclosing routine, and only
when the body does not rebind that name with its own `let`/`var`.

★ The STORE is the shape swept here.  An untyped binding (`let q *r` COPIES
where `let q r` ALIASES), an `as` cast (a reference cast to an integer is how
this tree takes an address) and an arithmetic operand (report_ref_arithmetic#
owns that) are deliberately left alone.

★ Checksum: BYTE-IDENTICAL emission of every root.  A swept `r` plans the very
node the `*r` collapse builds, so any root whose IR moves marks a site read
wrong.
"""
import re, os, sys, collections
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'refout'))
from scan import DECL, strip_comment, signature, split_params, body_lines

STORE = re.compile(r'^(\s*set\s+)\*(\w+)(\s*:)')
REFPARAM = re.compile(r'^\s*(\w+)\s*:\s*ref\[.*\]\s*$')

def sweep(path, dry=False):
    src = open(path, encoding='utf8').read()
    lines = src.split('\n')
    changed = 0
    i = 0
    while i < len(lines):
        m = DECL.match(strip_comment(lines[i]))
        if not m:
            i += 1; continue
        sig, end = signature(lines, i)
        if re.search(r'\bextern\b', sig):
            i = end + 1; continue
        names = set()
        for prm in split_params(sig):
            pm = REFPARAM.match(prm)
            if pm and not prm.rstrip().endswith('?'):
                names.add(pm.group(1))
        if not names:
            i = end + 1; continue
        body = body_lines(lines, end, m.group(1))
        span = len(body)
        # a body that rebinds the name owns it from there on -- do not guess
        text = '\n'.join(strip_comment(l) for l in body)
        for nm in list(names):
            if re.search(r'^\s*(let|var)\s+%s\b' % re.escape(nm), text, re.M):
                names.discard(nm)
        for k in range(end + 1, end + 1 + span):
            cut = len(lines[k].split(';')[0])
            head, tail = lines[k][:cut], lines[k][cut:]
            sm = STORE.match(head)
            if sm and sm.group(2) in names:
                lines[k] = STORE.sub(r'\1\2\3', head, count=1) + tail
                changed += 1
        i = end + 1
    if changed and not dry:
        open(path, 'w', encoding='utf8').write('\n'.join(lines))
    return changed

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry' in sys.argv
    per = collections.Counter()
    for root in args or ['packages']:
        for d, dirs, files in os.walk(root):
            dirs[:] = [x for x in dirs if x not in ('tests', 'out')]
            for f in sorted(files):
                if f.endswith('.scaly'):
                    p = os.path.join(d, f)
                    c = sweep(p, dry)
                    if c: per[p] = c
    for p, c in sorted(per.items()): print(f'{c:5}  {p}')
    print(sum(per.values()), 'derefs removed' + (' (dry run)' if dry else ''))
