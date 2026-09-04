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
            # ★★★DER ESCAPE-CHECKER REFUESIERT DEN AUFRUF, WO DAS ZIEL EIN
            # FELD AUF EINEM PARAMETER IST.  `set this.keys: allocate_slice[..]`
            # im `init` geht, `set m.keys: ...` auf einem `ref[PtrMap]`-Parameter
            # ist ein *reference into a local page escapes via store*: der
            # Checker kann nicht durch den Aufruf hindurchsehen, dass der Slice
            # auf `m.host` liegt und nicht auf der Frame-Page, waehrend die
            # Handform ihren `allocate`-Aufruf INLINE zeigt.  Gemessen an
            # Jit.scaly:672 und Escape.scaly:346 (README, Abschnitt "Ernte").
            # Ohne diese Regel bietet jeder Lauf dieselben vier Sites erneut an
            # und der Compiler wirft sie erneut zurueck.
            line = src[src.rfind('\n', 0, m.start())+1 : m.start()]
            tgt = re.match(r'\s*set\s+([A-Za-z_][A-Za-z0-9_]*)\.', line)
            if tgt and tgt.group(1) != 'this':
                st['BLOCKIERT (escape: Feld auf Parameter)']+=1
                if not apply:
                    print(f"{p}  BLOCKIERT: {line.strip()[:72]}...")
                continue
            new=f"allocate_slice[{m.group('t')}]({m.group('page')}, {m.group('len')})"
            out=out.replace(m.group(0), new, 1)
            st['KONVERTIERT']+=1
            if not apply and shown<4:
                print(f"{p}\n  - {m.group(0)[:96]}\n  + {new}"); shown+=1
        if out!=src and apply:
            open(p,'w',encoding='utf-8').write(out); files+=1
print(('UMGESCHRIEBEN' if apply else 'KANDIDATEN')+':', dict(st), f'files={files}')
