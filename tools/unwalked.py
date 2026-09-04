"""Was passiert im Body WIRKLICH mit einem UNWALKED-Pufferparameter?

`refslice` sagt nur `not walks(body, p)` -- keine Arithmetik, keine
Indizierung -- und nennt das zu Recht die Abwesenheit von Evidenz.  Diese
Aufschluesselung fragt, WAS statt dessen passiert.  Genau diese Frage hat bei
den Handpuffern neun konvertierbare Sites unter einer Klassenaussage
hervorgeholt.

★★★GEMESSEN 2026-09-04 ueber alle Pakete: **427 UNWALKED, davon 0 tote
Parameter und 280 reine Forwarder.**  Die Klasse ist damit das, was der
refslice-Kommentar sagt -- Abwesenheit von Evidenz -- aber die
Weiterreich-ZIELE sind die eigentliche Auskunft: `kw_eq` fuehrt die Liste mit
85, und genau ueber diese 85 kam der Fund dieser Runde (der GENERATOR von
`Keywords.scaly` erzeugte noch die Vor-Slice-Signatur, eine Regeneration haette
die Konversion an 88 Stellen still zurueckgedreht).

★★★DIE FALLE, DIE DIESES SKRIPT ZUERST SELBST STELLTE: `collect()` liefert
`body` als STRING.  `for l in r['body']` laeuft dann ueber ZEICHEN, findet nie
einen Parameternamen und meldet JEDEN Parameter als "unbenutzt" -- der erste
Lauf sagte 314 tote Parameter, und die Zahl sah plausibel aus.  Deshalb die
Selbstpruefung unten: sie beweist, dass das Instrument feuert, bevor eine Zahl
geglaubt wird.
"""
import re, os, sys, collections
sys.path.insert(0, os.path.abspath('tools/refout'))
from scan import collect, strip_comment
import importlib.util as ilu
spec = ilu.spec_from_file_location('rs', 'tools/refslice/scan.py')
rs = ilu.module_from_spec(spec); spec.loader.exec_module(rs)

NON_ELEMENT = ('void', 'cstring')
st = collections.Counter(); ex = collections.defaultdict(list)
fwd_targets = collections.Counter()

routines = collect(['packages'])

# ★★★SELBSTPRUEFUNG: `body` ist ein STRING, kein Zeilenlist -- eine Iteration
# darueber laeuft ueber ZEICHEN und findet NIE einen Parameternamen, worauf
# JEDER Parameter als "unbenutzt" gilt.  Genau so hat dieses Skript zuerst
# 314 tote Parameter gemeldet.  Ein Instrument, das nicht feuern kann, ist
# schlimmer als keins -- also erst beweisen, dass es feuert.
_probe = [x for x in routines if x['fn'] == 'usage_put_stringc']
assert _probe, 'Sondenroutine nicht gefunden'
assert 'bp' in _probe[0]['body'], 'SELBSTPRUEFUNG FEHLGESCHLAGEN: Body sieht den Parameter nicht'

for r in routines:
    for pn, pt in r['params']:
        m = re.match(r'pointer\[\s*(\w+)\s*\]$', pt.strip())
        if not m: continue
        elem = m.group(1)
        if elem in NON_ELEMENT: continue
        if rs.walks(r['body'], pn): continue          # nur UNWALKED
        body = [strip_comment(l) for l in r['body'].split('\n')]
        occ = [l for l in body if re.search(rf'\b{re.escape(pn)}\b', l)]
        if not occ:
            st['UNBENUTZT (toter Parameter)'] += 1
            if len(ex['UNBENUTZT (toter Parameter)']) < 6:
                ex['UNBENUTZT (toter Parameter)'].append(f"{r['file']}:{r['line']}  {r['fn']}({pn})")
            continue
        kinds = set()
        for l in occ:
            if re.search(rf'\*\s*{re.escape(pn)}\b', l):        kinds.add('bare deref *p')
            if re.search(rf'\b{re.escape(pn)}\s*(=|<>)\s*null', l) or \
               re.search(rf'null\s*(=|<>)\s*\b{re.escape(pn)}\b', l): kinds.add('null-Test')
            if re.search(rf'\b{re.escape(pn)}\s+as\b', l):      kinds.add('Cast')
            mm = re.findall(r'\b([A-Za-z_][A-Za-z0-9_.]*)\s*\(', l)
            if mm and re.search(rf'\(\s*[^)]*\b{re.escape(pn)}\b', l):
                kinds.add('als Argument weitergereicht')
                for t in mm: fwd_targets[t.split('.')[-1]] += 1
            if re.search(rf'set\s+\b{re.escape(pn)}\b', l):     kinds.add('neu zugewiesen')
        k = ' + '.join(sorted(kinds)) or 'sonstige Erwaehnung'
        st[k] += 1
        if len(ex[k]) < 4:
            ex[k].append(f"{r['file']}:{r['line']}  {r['fn']}({pn}: pointer[{elem}])")
for k, n in st.most_common():
    print(f'{n:5}  {k}')
    for e in ex[k]: print(f'         {e}')
print('--- gesamt', sum(st.values()))
print('\n=== haeufigste Weiterreich-Ziele ===')
for t, n in fwd_targets.most_common(12): print(f'{n:5}  {t}')
