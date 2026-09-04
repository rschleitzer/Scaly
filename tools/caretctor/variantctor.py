#!/usr/bin/env python3
"""`allocate(sizeof T,...) as pointer[T]` + `set *p: T.Variant(...)` + `p`
   ->  `let p T.Variant^host(...)` + `&p`

Nur das EXAKTE Dreizeilen-Muster, in dem der gebundene Name danach nur noch als
Ergebnis steht: alles andere entscheidet ein Leser.
"""
import re, os, sys, collections

A = re.compile(r'^(\s*)let ([A-Za-z_][A-Za-z0-9_]*) ([A-Za-z_][A-Za-z0-9_.]*)\.allocate\(sizeof ([A-Za-z_][A-Za-z0-9_]*), alignof \4\) as pointer\[\4\]\s*$')

apply = '--apply' in sys.argv
stats = collections.Counter(); shown = 0; files = 0
for root, dirs, fs in os.walk('packages'):
    for f in sorted(fs):
        if not f.endswith('.scaly'): continue
        p = os.path.join(root, f)
        lines = open(p, encoding='utf-8').read().split('\n')
        out = list(lines); touched = False; i = 0
        while i < len(lines) - 2:
            m = A.match(lines[i])
            if not m:
                i += 1; continue
            ind, name, page, ty = m.groups()
            B = re.match(r'^\s*set \*' + name + r': ' + ty + r'\.([A-Za-z_][A-Za-z0-9_]*)\((.*)\)\s*$', lines[i+1])
            C = lines[i+2].strip() == name
            if not (B and C):
                stats['kein-Muster'] += 1; i += 1; continue
            variant, args = B.groups()
            out[i]   = f'{ind}let {name} {ty}.{variant}^{page}({args})'
            out[i+1] = f'{ind}&{name}'
            out[i+2] = None
            stats['KONVERTIERT'] += 1; touched = True
            if not apply and shown < 4:
                print(f'{p}:{i+1}\n  - {lines[i].strip()}\n  - {lines[i+1].strip()}\n  - {lines[i+2].strip()}\n  + {out[i].strip()}\n  + {out[i+1].strip()}'); shown += 1
            i += 3
        if touched and apply:
            open(p,'w',encoding='utf-8').write('\n'.join(l for l in out if l is not None)); files += 1
print(('UMGESCHRIEBEN' if apply else 'KANDIDATEN') + ':', dict(stats), f'files={files}')
