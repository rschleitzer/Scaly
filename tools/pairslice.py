#!/usr/bin/env python3
"""Das (Zeiger, Laenge)-PAAR wieder zu EINEM Wert machen.

    StringC^host(x.data_ptr(),   x.get_size())     ->  StringC^host(x.as_slice())
    StringC^host(b.get_buffer(), b.get_length())   ->  StringC^host(b.as_slice())

★★★WARUM DAS DIE GROESSTE EINZELKLASSE IST: `define StringC (data:
pointer[u32], size: size_t)` -- opensps zentraler Stringtyp IST ein
`Slice[u32]` unter anderem Namen.  Sein `data_ptr()` hat 201 Aufrufstellen und
davon erreichen VIER eine C-Funktion; der Rest reicht den Zeiger an Scaly-Code
weiter, ganz ueberwiegend als genau dieses Paar.  Das ist kein Zeigerproblem,
sondern ein Slice, der zerlegt und beim Empfaenger wieder zusammengesetzt wird.

★DIE BEDINGUNG, DIE DAS WERKZEUG PRUEFT, ist die Identitaet des EMPFAENGERS:
`x.data_ptr(), x.get_size()` nur dann, wenn beide Male derselbe Ausdruck
davorsteht.  `f(a.data_ptr(), b.get_size())` misst zwei verschiedene Dinge und
ist genau die Falle, an der `refslice`s LOOSE-Verdikt haengt -- eine Laenge,
die etwas anderes misst als der Puffer.

★DIE ZWEITE BEDINGUNG ist, dass der Empfaenger ein `as_slice()` HAT.  Drei
Traeger sind belegt: `StringC` (seit 2026-09-04), `Array[T]` und `Vector[T]`
(`Slice[T](length, get_buffer())` -- Wort fuer Wort das Paar) und `String`
(u8).  Alles andere wird gemeldet und bleibt liegen; das Werkzeug raet nicht,
welcher Typ links steht, sondern der COMPILER lehnt ab, was nicht traegt, und
der Treiber nimmt die Datei dann zurueck.

★★★WAS ES NICHT ANFASST: eine Stelle, deren Zielkonzept KEIN `Slice`-init hat.
`String(buffer.get_buffer(), buffer.get_length())` steht in der stdlib und
wuerde einen Seed schulden -- `--only` waehlt das Zielkonzept, und ohne Angabe
ist es `StringC`.

★★★ZWEI SPERREN, DIE BEIDE EINEN LAUF GEKOSTET HAETTEN:

  (1) EIN TREFFER IN PROSA IST KEINE SITE.  Die erste Fassung haette den
      Doc-Kommentar umgeschrieben, den `data_ptr` gerade bekommen hat -- er
      ZITIERT die alte Form, um sie zu erklaeren, und waere zu Unsinn
      geworden.  CLAUDE.md nennt genau das ("es schreibt PROSA um") als die
      Falle des Slice-Umbaus.  Alles hinter einem `;` bleibt unangetastet.

  (2) EINE GENERIERTE DATEI WIRD ABGELEHNT.  Eine Konversion, die nicht auch
      im Generator steht, ist ein stilles Zurueckdrehen bei der naechsten
      Regeneration.  Der Marker wird in Zeile 1-15 gesucht, CASE-INSENSITIV:
      `CharProps.scaly` schreibt "Generated" mit grossem G, und ein
      case-sensitiver Grep danach meldete die Datei als sauber.
      `--allow-generated` hebt die Sperre fuer einen Fall auf, den der LESER
      entschieden hat -- fuer `CharProps.scaly` etwa, wo `charpropgen.py`
      nachweislich nur 58 Zeilen TABELLEN ausgibt und keine Funktion, der
      Fundort also im handgeschriebenen Teil liegt.

Aufruf: tools/pairslice.py [--apply] [--only=StringC[,String]]
                           [--allow-generated] [<datei>...]
"""
import re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'derefcensus'))
import scan

PAIR = re.compile(
    r'(?P<target>\b[A-Z]\w*)(?P<sigil>\^\w+|#)?\('
    r'\s*(?P<r1>[\w.]+)\.(?P<acc>data_ptr|get_buffer)\s*\(\s*\)'
    r'\s*,\s*(?P<r2>[\w.]+)\.(?P<len>get_size|get_length)\s*\(\s*\)\s*\)')

def code_end(line):
    """Index, ab dem die Zeile Kommentar ist (ein `;` ausserhalb eines Strings)."""
    return len(scan.strip_comment(line).rstrip('\n'))

def convert_line(line, targets):
    done, held = [], []
    out, pos, ce = '', 0, code_end(line)
    for m in PAIR.finditer(line):
        if m.start() >= ce:
            continue                       # (1) Prosa, keine Site
        if m.group('target') not in targets:
            held.append((m.group(0), f"Zielkonzept {m.group('target')} nicht gewaehlt"))
            continue
        if m.group('r1') != m.group('r2'):
            held.append((m.group(0), f"verschiedene Empfaenger: {m.group('r1')} / {m.group('r2')}"))
            continue
        new = f"{m.group('target')}{m.group('sigil') or ''}({m.group('r1')}.as_slice())"
        out += line[pos:m.start()] + new
        pos = m.end()
        done.append((m.group(0), new))
    return out + line[pos:], done, held

def main():
    apply = '--apply' in sys.argv
    only = 'StringC'
    allow_gen = '--allow-generated' in sys.argv
    for a in sys.argv[1:]:
        if a.startswith('--only='): only = a.split('=', 1)[1]
    targets = set(only.split(','))
    files = [a for a in sys.argv[1:] if not a.startswith('-')]
    if not files:
        files = sorted(scan.files('packages'))
    nd = nh = nf = 0
    ngen = 0
    for p in files:
        src = open(p, encoding='utf-8').readlines()
        if not allow_gen and re.search(r'generat', ''.join(src[:15]), re.I):
            d = []
            for raw in src:                # ZEILENWEISE -- `code_end` auf dem
                _, dd, _ = convert_line(raw, targets)   # ganzen Text findet das
                d += dd                    # erste `;` der Datei und sieht nichts
            if d:
                ngen += len(d)
                print(f'--- {p}\n   GENERIERT: {len(d)} Site(s) uebersprungen '
                      f'-- eine Konversion ohne den Generator ist ein stilles '
                      f'Zurueckdrehen (--allow-generated hebt es auf)')
            continue
        out, fd, fh = [], [], []
        for i, raw in enumerate(src):
            # Kommentare bleiben unangetastet -- eine Prosa-Zeile ist keine Site.
            if scan.strip_comment(raw).strip() != raw.strip() and raw.lstrip().startswith(';'):
                out.append(raw); continue
            new, d, h = convert_line(raw, targets)
            out.append(new)
            fd += [(i + 1, a, b) for a, b in d]
            fh += [(i + 1, a, r) for a, r in h]
        if not fd and not fh: continue
        print(f'--- {p}')
        for ln, a, b in fd: print(f'   OK   :{ln}  {a[:95]}\n     ->  {b}')
        for ln, a, r in fh: print(f'   HALT :{ln}  {r}\n        {a[:95]}')
        nd += len(fd); nh += len(fh)
        if fd:
            nf += 1
            if apply: open(p, 'w', encoding='utf-8').writelines(out)
    print(f'\nSUMME: {nd} konvertierbar, {nh} liegen gelassen, '
          f'{ngen} in generierten Dateien uebersprungen, {nf} Dateien'
          + ('' if apply else '  (nur gezählt)'))

if __name__ == '__main__':
    main()
