#!/usr/bin/env python3
"""Die Varianten-Sites mit BELIEBIGER Folgezeile, in der zweizeiligen Form:

    let p host.allocate(sizeof T, alignof T) as pointer[T]     let pv T.Variant^host(args)
    set *p: T.Variant(args)                              ->    let p &pv
    <Rest unveraendert>                                        <Rest unveraendert>

Die DIREKTE Form (`let p &T.Variant^host(args)`) traegt hier nicht: der
Escape-Checker refuest sie beim STORE und beim RETURN, weil `region_of_operand`
ueber die VARIABLE laeuft und nicht ueber die Konstruktion. Gemessen; ein Arm
fuer die Konstruktion allein aendert daran nichts (probiert, verworfen).
"""
import re, os, sys, collections

A = re.compile(r'^(\s*)let ([A-Za-z_][A-Za-z0-9_]*) ([A-Za-z_][A-Za-z0-9_.]*)\.allocate\(sizeof ([A-Za-z_][A-Za-z0-9_]*), alignof \4\) as pointer\[\4\]\s*$')

apply='--apply' in sys.argv
stats=collections.Counter(); shown=0; files=0
for root, dirs, fs in os.walk('packages'):
    for f in sorted(fs):
        if not f.endswith('.scaly'): continue
        p=os.path.join(root,f)
        src=open(p,encoding='utf-8').read(); lines=src.split('\n')
        out=list(lines); touched=False
        for i in range(len(lines)-1):
            m=A.match(lines[i])
            if not m: continue
            ind,name,page,ty=m.groups()
            B=re.match(r'^\s*set \*'+re.escape(name)+r': '+re.escape(ty)+r'\.([A-Z][A-Za-z0-9_]*)\((.*)\)\s*$', lines[i+1])
            if not B: continue
            variant,args=B.groups()
            vname=name+'_v'
            if re.search(r'\b'+re.escape(vname)+r'\b', src):   # Name schon vergeben
                stats['Name belegt']+=1; continue
            out[i]  =f'{ind}let {vname} {ty}.{variant}^{page}({args})'
            out[i+1]=f'{ind}let {name} &{vname}'
            stats['KONVERTIERT']+=1; touched=True
            if not apply and shown<3:
                print(f'{p}:{i+1}\n  - {lines[i].strip()}\n  - {lines[i+1].strip()}\n  + {out[i].strip()}\n  + {out[i+1].strip()}'); shown+=1
        if touched and apply:
            open(p,'w',encoding='utf-8').write('\n'.join(out)); files+=1
print(('UMGESCHRIEBEN' if apply else 'KANDIDATEN')+':', dict(stats), f'files={files}')
