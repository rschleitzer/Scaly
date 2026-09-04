"""Ein HANDPUFFER, der rein lokal gelaufen wird: `allocate_slice[T]` + Subscript?

CLAUDE.md haelt die ~367 hand-`allocate`ten Puffer fuer unkonvertierbar, weil
"das Local an einen `pointer[T]`-Callee weitergereicht wird -- die Konversion
ist die des CALLEES".  Das ist wahr fuer die, bei denen es stimmt.  Dieser Scan
fragt es PRO SITE, mit demselben load-bearing Guard, den sliceview.py benutzt:
JEDE Nutzung des Locals muss ein `*(p + i)` sein.  Alles andere -- Callee-Arg,
bares `*p`, `p + k` als Adresse, Rueckgabe -- ist Fundament.

★★★GEMESSEN 2026-09-04: 239 Bindungen, davon **230 Fundament und 9 REIN LOKAL
GELAUFEN** -- und die neun sind konvertiert (`allocate_slice[T]` + Subscript,
opensp 8, dazzle 1), alle Gates gruen.  CLAUDE.md schrieb die Klasse pauschal
ab ("367 hand-`allocate`d buffers whose local is handed on to a `pointer[T]`
callee -- the conversion is the CALLEE's, not the walk's"); das gilt fuer die
230, nicht fuer alle.  **Eine Population ist eine Behauptung, und eine
Erschoepfungsaussage erbt sie** -- dieselbe Lehre, die derefcensus schon einmal
gekostet hat.

★★★DER ESCAPE-CHECKER ENTSCHEIDET NACH DEM ZIEL, NICHT NACH DER FUNKTION: ein
`let p allocate_slice[T](host, n)` geht, ein `set m.keys: allocate_slice[..]`
auf einem Feld eines Parameters ist ein *reference into a local page escapes
via store* (siehe tools/caretctor/README.md).  Deshalb ist genau die lokale
Bindung die konvertierbare Form.
"""
import re, os, sys, collections
sys.path.insert(0, 'tools')
from subscript_local import routine_spans, strip_comment, deref_spans, other_use

ALLOC = re.compile(r'^(\s*)(?:let|var)\s+([A-Za-z_][A-Za-z0-9_\']*)\s+'
                   r'(.*\ballocate\(.*)\bas pointer\[([A-Za-z_][A-Za-z0-9_]*)\]\s*$')

st = collections.Counter(); hits = []
for root, dirs, fs in os.walk('packages'):
    for f in sorted(fs):
        if not f.endswith('.scaly'): continue
        p = os.path.join(root, f)
        lines = open(p, encoding='utf8').read().split('\n')
        for a, b in routine_spans(lines):
            body = lines[a:b]; codes = [strip_comment(l) for l in body]
            for k in range(len(body)):
                m = ALLOC.match(codes[k])
                if not m: continue
                local, ty = m.group(2), m.group(4)
                uses = 0; reasons = set()
                for j in range(k + 1, len(body)):
                    code = codes[j]
                    if not re.search(rf'\b{re.escape(local)}\b', code): continue
                    uses += 1
                    spans = deref_spans(code, local)
                    if not spans:
                        reasons.add('nicht-Deref-Nutzung'); continue
                    if any(s[2] == '-' for s in spans): reasons.add('rueckwaerts')
                    if other_use(code, local, spans): reasons.add('zusaetzliche Nutzung')
                if uses == 0:      st['UNGENUTZT'] += 1
                elif not reasons:
                    st['REIN LOKAL GELAUFEN'] += 1
                    hits.append(f'{p}:{a+k+1}  [{ty}]  {codes[k].strip()[:66]}')
                else:              st['Fundament: ' + ' + '.join(sorted(reasons))] += 1
for kk, n in st.most_common(): print(f'{n:5}  {kk}')
print('--- gesamt', sum(st.values()))
if hits:
    print('\n=== REIN LOKAL GELAUFEN ===')
    for h in hits: print(' ', h)
