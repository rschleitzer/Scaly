#!/usr/bin/env python3
"""The INLINE form of the fixed-array walk, the half `tools/stackarray.py` cannot see:

    var digits u8[24]
    ... *((digits as pointer[u8]) + k) ...   ->   ... digits[k] ...

`stackarray.py` rewrites the walk that goes through a BINDING (`let mp modes as
pointer[int]`); where the cast sits inline in the expression there is no binding
to key on, and 90 sites of the tree are that shape.  Same rule, same reason: a
`var x T[N]` local is an `[N x T]` VALUE and indexes in place.

★★★THE DECLARATION MUST BE RESOLVED BODY-LOCALLY, AND A FILE-WIDE NAME TABLE IS
NOT AN APPROXIMATION OF THAT -- IT IS WRONG IN BOTH DIRECTIONS.  Measured on
`SgmlFOTBuilder.scaly`: `var digits u8[24]` in one function and
`let digits "0123456789ABCDEF"` in the next are two different things, and the
second is legitimate buffer arithmetic over a cstring.  The file-wide version
rewrote it (three parse errors) AND skipped a real candidate in `ELObj.scaly`,
where a body-local `var buf u8[64]` was masked by a `var buf u32[1]` elsewhere
in the file.  The compiler caught the first direction; nothing would have caught
it had the two element types agreed.

★★★THE ELEMENT TYPE MUST MATCH THE CAST EXACTLY.  `var buf u32[8]` walked as
`*((buf as pointer[u8]) + i)` is a REINTERPRETATION at a different stride, and
`buf[i]` is not the same expression.  13 such sites are skipped on purpose.

★★★THE BARE FORM IS DELIBERATELY NOT CONVERTED.  `*(x as pointer[T])` -> `x[0]`
removes no arithmetic (there is none) and emits an extra `getelementptr ... i64 0`
where the deref stored directly -- harmless, folded by `opt -O2`, but it moves
`seed/scaly.ll` for a purely cosmetic gain.  19 sites, left as they are.

★The write is `set base[i]: v` here, the OPPOSITE of the container rule: on a
fixed array the subscript is a GEP and the store is real, while on an
Array/Vector/Slice `set a[i]: v` is a hard rc-4 and the write is `put`.

Cost: 90 sites over 13 files, ALL 22 package roots BYTE-IDENTICAL, no seed owed.

Usage: tools/stackinline.py [--apply]
"""
import re, os, sys, collections

FN    = re.compile(r'^\s*(function|procedure)\s')
ARRAY = re.compile(r'^\s*var\s+([A-Za-z_][A-Za-z0-9_]*)\s+([A-Za-z_][A-Za-z0-9_]*)\[\s*\d+\s*\]\s*$')
OTHER = re.compile(r'^\s*(?:var|let)\s+([A-Za-z_][A-Za-z0-9_]*)\b')
PAT   = re.compile(r'\*\(\(\s*([A-Za-z_][A-Za-z0-9_]*)\s+as\s+pointer\[([A-Za-z_][A-Za-z0-9_]*)\]\s*\)\s*\+\s*')
BARE  = re.compile(r'\*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s+as\s+pointer\[([A-Za-z_][A-Za-z0-9_]*)\]\s*\)')

def match_paren(s, i):
    d = 0
    while i < len(s):
        if s[i] == '(': d += 1
        elif s[i] == ')':
            d -= 1
            if d == 0: return i + 1
        i += 1
    return -1

def spans(lines):
    """(start, end) je Funktionsrumpf, an der Deklarationszeile getrennt."""
    starts = [i for i, l in enumerate(lines) if FN.match(l)]
    for k, s in enumerate(starts):
        yield s, (starts[k+1] if k+1 < len(starts) else len(lines))

apply = '--apply' in sys.argv
stats = collections.Counter(); shown = 0; changed_files = 0
for root, dirs, files in os.walk('packages'):
    for f in sorted(files):
        if not f.endswith('.scaly'): continue
        p = os.path.join(root, f)
        lines = open(p, encoding='utf-8').read().split('\n')
        touched = False
        for s, e in spans(lines):
            arrays, shadowed = {}, set()
            for i in range(s, e):
                m = ARRAY.match(lines[i])
                if m:
                    if m.group(1) in arrays and arrays[m.group(1)] != m.group(2):
                        shadowed.add(m.group(1))
                    arrays[m.group(1)] = m.group(2)
                    continue
                m2 = OTHER.match(lines[i])
                if m2 and m2.group(1) in arrays:
                    shadowed.add(m2.group(1))       # im selben Rumpf neu gebunden
            for i in range(s, e):
                line, new = lines[i], lines[i]
                for rx, kind in ((PAT, "OFFSET"),):
                    while True:
                        m = rx.search(new)
                        if not m: break
                        name, ty = m.group(1), m.group(2)
                        if name in shadowed or arrays.get(name) != ty:
                            stats['SKIP'] += 1; break
                        if kind == 'OFFSET':
                            end = match_paren(new, m.start() + 1)
                            if end < 0: stats['SKIP'] += 1; break
                            idx = new[m.end():end-1].strip()
                            new = new[:m.start()] + f'{name}[{idx}]' + new[end:]
                        else:
                            new = new[:m.start()] + f'{name}[0]' + new[m.end():]
                        stats[kind] += 1
                if new != line:
                    if not apply and shown < 6:
                        print(f'{p}:{i+1}\n  - {line.strip()}\n  + {new.strip()}'); shown += 1
                    lines[i] = new; touched = True
        if touched and apply:
            open(p, 'w', encoding='utf-8').write('\n'.join(lines)); changed_files += 1
print(('UMGESCHRIEBEN' if apply else 'KANDIDATEN') + ':', dict(stats), f'files={changed_files}')
