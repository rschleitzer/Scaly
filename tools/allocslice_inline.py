#!/usr/bin/env python3
"""Die HANDGESCHRIEBENE Schreibweise von `allocate_slice[T]` einsammeln.

    Slice[T](L, R.allocate(B, A) as pointer[T])   ->   allocate_slice[T](R, L)

`allocate_slice` (2026-09-04, `scaly/containers/Slice.scaly`) IST diese
Konstruktion -- ihr eigener Kommentar nennt die Sites, die sie von Hand
schreiben.  Neun wurden in der ersten Runde geerntet, ALLE mit skalarem
Elementtyp; die hier gefundenen sind der Rest, und darunter liegen die beiden
Elementklassen, die noch nie durch diese Funktion gelaufen sind: `ref[X]?`
(NPO-Option, opensp' Id/Lpd/Partition/Syntax) und ein 16-Byte-STRUCT
(`Slice[StringC]` in Syntax.scaly).  Beide sind mit einer Sonde belegt, BEVOR
hier etwas umgeschrieben wurde -- put/get-Rundlauf ueber `allocate_slice`.

★★★DAS EIGENTLICHE ARGUMENT ist kein Zaehlerstand, sondern eine ASYMMETRIE:
`Escape.scaly` und `Jit.scaly` rufen `allocate_slice` im `init` (Zeile 275/597)
und schreiben im REHASH derselben Datei (346/672) dieselbe Allokation von Hand.
CLAUDE.md nennt genau diese Form den Beweis und nicht den Hinweis: wenn eine
Schwesterroutine die Fassung schon hat und diese nicht, ist das der Fund.

★★★WAS DER GEWINN IST: jede dieser Zeilen MINTET ein `as pointer[T]`.
`Page.allocate` antwortet rohen Speicher, also muss ihm irgendwer einen Typ
geben -- am Aufrufort getan, ist das genau der Zeiger, den die Kampagne sucht.
In `allocate_slice` steht der Cast EINMAL.

★★★WAS DIESES SKRIPT NICHT ENTSCHEIDET: ob `B` wirklich `L * sizeof T` ist.
Es PRUEFT das textuell (unten) und LEHNT AB, was es nicht beweisen kann -- die
Gleichheit `alignof pointer[Id]` == `alignof ref[Id]?` etwa steht hier als
Tabelle und nicht als Vermutung.  Ein Rest, den die Tabelle nicht traegt, wird
gemeldet und bleibt liegen; der Leser entscheidet, nie das Werkzeug.

Aufruf: tools/allocslice_inline.py [--apply]
"""
import re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'derefcensus'))
import scan

# Elementgroesse in Bytes, soweit sie hier BEWEISBAR ist.  Ein Typ, der nicht
# in dieser Tabelle steht, wird nur akzeptiert, wenn `B` ihn SYMBOLISCH nennt
# (`n * sizeof StringC`) -- dann ist die Gleichheit textuell und braucht keine
# Zahl.
SIZE = {'u8': 1, 'i8': 1, 'char': 1, 'u16': 2, 'i16': 2, 'u32': 4, 'i32': 4,
        'u64': 8, 'i64': 8, 'int': 8, 'size_t': 8}
# Ein `ref[X]?` ist Option[ref[X]] und damit NPO -- zeigergross.  Der Baum
# schreibt die Allokation dafuer als `sizeof pointer[X]`, was dieselbe Zahl ist.
PTRLIKE = 8

def norm(s):
    return re.sub(r'\s+', ' ', s).strip()

def elem_size(t):
    t = t.strip()
    if t in SIZE: return SIZE[t]
    if re.match(r'^ref\[.*\]\?$', t) or t.startswith('pointer['): return PTRLIKE
    return None

def bytes_match(L, B, T):
    """Ist `B` beweisbar `L * sizeof T`?"""
    L, B = norm(L), norm(B)
    sz = elem_size(T)
    # (a) symbolisch: B ist L * sizeof T  (Klammerung egal)
    for form in (f'{L} * sizeof {T}', f'({L}) * sizeof {T}',
                 f'{L} * (sizeof {T})', f'({L}) * (sizeof {T})'):
        if norm(form) == B: return True
    # (b) der Baum schreibt fuer ref[X]?/Zeiger `sizeof pointer[X]`
    if sz == PTRLIKE:
        inner = re.match(r'^ref\[(.*)\]\?$', T)
        names = ['void'] + ([inner.group(1)] if inner else [])
        for nm in names:
            for form in (f'{L} * (sizeof pointer[{nm}])', f'{L} * sizeof pointer[{nm}]',
                         f'({L}) * (sizeof pointer[{nm}])', f'({L}) * sizeof pointer[{nm}]'):
                if norm(form) == B: return True
    if sz is None: return False
    # (c) numerisch: B ist L mit angehaengtem `* <sz>`, oder sz == 1 und B == L
    if sz == 1 and B == L: return True
    # L kann ein `X as size_t` sein, B dieselbe Basis ohne den Cast
    Lb = re.sub(r'\s+as\s+size_t\b', '', L).strip()
    Lb = Lb[1:-1].strip() if Lb.startswith('(') and Lb.endswith(')') else Lb
    Bb = B
    for cand in (L, Lb):
        if sz == 1 and norm(cand) == Bb: return True
        for form in (f'{cand} * {sz}', f'({cand}) * {sz}'):
            if norm(form) == Bb: return True
    return False

def align_match(A, T):
    A = norm(A)
    sz = elem_size(T)
    if A == f'alignof {T}': return True
    if sz == PTRLIKE:
        inner = re.match(r'^ref\[(.*)\]\?$', T)
        for nm in ['void'] + ([inner.group(1)] if inner else []):
            if A == f'alignof pointer[{nm}]': return True
    return sz is not None and A == str(sz)

def split_args(s):
    out, depth, cur = [], 0, []
    for c in s:
        if c in '([': depth += 1
        elif c in ')]': depth -= 1
        if c == ',' and depth == 0:
            out.append(''.join(cur)); cur = []
        else: cur.append(c)
    out.append(''.join(cur))
    return [x.strip() for x in out]

def balanced_end(text, open_at):
    """Index NACH der zu text[open_at]=='(' gehoerenden ')'."""
    depth = 0
    for i in range(open_at, len(text)):
        if text[i] in '([': depth += 1
        elif text[i] in ')]':
            depth -= 1
            if depth == 0: return i + 1
    return -1

HEAD = re.compile(r'\bSlice\[')

def convert_text(text):
    """Alle Sites in einem (ggf. mehrzeiligen) Quelltext ersetzen. Liefert
    (neuer_text, [(alt, neu)], [(alt, grund)])."""
    done, held = [], []
    pos = 0
    while True:
        m = HEAD.search(text, pos)
        if not m: break
        tb = balanced_end(text, m.end() - 1)          # ] des Typarguments
        if tb < 0: break
        T = text[m.end():tb - 1].strip()
        if tb >= len(text) or text[tb] != '(':
            pos = m.end(); continue
        ab = balanced_end(text, tb)
        if ab < 0: pos = m.end(); continue
        whole = text[m.start():ab]
        args = split_args(text[tb + 1:ab - 1])
        if len(args) != 2 or 'allocate' not in args[1]:
            pos = m.end(); continue
        L, R = args[0], norm(args[1])
        am = re.match(r'^([A-Za-z_][\w.]*)\.allocate\s*\(', R)
        if not am:
            pos = m.end(); continue
        recv = am.group(1)
        ae = balanced_end(R, am.end() - 1)
        aargs = split_args(R[am.end():ae - 1])
        tail = norm(R[ae:])
        if len(aargs) != 2 or not re.match(r'^as\s+pointer\[', tail):
            held.append((norm(whole), f'Schwanz nicht `as pointer[..]`: {tail[:40]}'))
            pos = m.end(); continue
        B, A = aargs[0], aargs[1]
        if not bytes_match(L, B, T):
            held.append((norm(whole), f'Bytezahl nicht beweisbar L*sizeof {T}: {norm(B)}'))
            pos = m.end(); continue
        if not align_match(A, T):
            held.append((norm(whole), f'Ausrichtung nicht beweisbar alignof {T}: {norm(A)}'))
            pos = m.end(); continue
        new = f'allocate_slice[{T}]({recv}, {norm(L)})'
        done.append((norm(whole), new))
        text = text[:m.start()] + new + text[ab:]
        pos = m.start() + len(new)
    return text, done, held

def main():
    apply = '--apply' in sys.argv
    files = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not files:
        files = [p for p in sorted(scan.files('packages'))
                 if not p.endswith('containers/Slice.scaly')]
    nd = nh = nf = 0
    for p in files:
        raw = open(p, encoding='utf-8').read()
        if 'Slice[' not in raw or 'allocate' not in raw: continue
        new, done, held = convert_text(raw)
        if not done and not held: continue
        print(f'--- {p}')
        for a, b in done: print(f'   OK   {a[:120]}\n     ->  {b}')
        for a, r in held: print(f'   HALT {r}\n        {a[:120]}')
        nd += len(done); nh += len(held)
        if done:
            nf += 1
            if apply: open(p, 'w', encoding='utf-8').write(new)
    print(f'\nSUMME: {nd} konvertierbar, {nh} liegen gelassen, {nf} Dateien'
          + ('' if apply else '  (nur gezählt)'))

if __name__ == '__main__':
    main()
