#!/usr/bin/env python3
"""`Slice[T](n, host.allocate(n * sizeof T, alignof T) as pointer[T])`
   ->  `allocate_slice[T](host, n)`

Nur wo Elementtyp und Laenge in beiden Haelften WOERTLICH uebereinstimmen —
eine Laenge, die etwas anderes misst, ist eine andere Aussage.
"""
import re, os, sys, collections

# Slice[T](LEN, PAGE.allocate(LEN * sizeof T, alignof T) as pointer[T])
PAT = re.compile(
    r'Slice\[(?P<t>[A-Za-z_][A-Za-z0-9_]*)\]\('
    r'(?P<len>[A-Za-z_][A-Za-z0-9_.]*|\d+)\s*,\s*'
    r'(?P<page>[A-Za-z_][A-Za-z0-9_.]*)\.allocate\('
    r'(?P<len2>[A-Za-z_][A-Za-z0-9_.]*|\d+)\s*\*\s*\(?sizeof (?P=t)\)?\s*,\s*'
    r'alignof (?P=t)\s*\)\s*as pointer\[(?P=t)\]\)')

apply='--apply' in sys.argv
st=collections.Counter(); shown=0; files=0
for root,dirs,fs in os.walk('packages'):
    for f in sorted(fs):
        if not f.endswith('.scaly'): continue
        p=os.path.join(root,f)
        src=open(p,encoding='utf-8').read(); out=src
        # ★Die eigene DEFINITION nicht anfassen: ein Rewriter, der sein Ziel
        # in seiner Quelle findet, schreibt sie zu einer Endlosrekursion um.
        if 'function allocate_slice[T]' in src:
            st['eigene Definition uebersprungen']+=1; continue
        for m in list(PAT.finditer(src)):
            if m.group('len') != m.group('len2'):
                st['Laenge weicht ab']+=1; continue
            new=f"allocate_slice[{m.group('t')}]({m.group('page')}, {m.group('len')})"
            out=out.replace(m.group(0), new, 1)
            st['KONVERTIERT']+=1
            if not apply and shown<4:
                print(f"{p}\n  - {m.group(0)[:96]}\n  + {new}"); shown+=1
        if out!=src and apply:
            open(p,'w',encoding='utf-8').write(out); files+=1
print(('UMGESCHRIEBEN' if apply else 'KANDIDATEN')+':', dict(st), f'files={files}')
