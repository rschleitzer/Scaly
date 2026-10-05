"""A HAND BUFFER that is walked purely locally: `allocate_slice[T]` + subscript?

The ~367 hand-`allocate`d buffers counted as unconvertible because
"the local is handed on to a `pointer[T]` callee -- the conversion
is the CALLEE's".  That is true for those where it holds.  This scan
asks it PER SITE, with the same load-bearing guard sliceview.py uses:
EVERY use of the local must be a `*(p + i)`.  Everything else -- callee arg,
bare `*p`, `p + k` as an address, a return -- is floor.

★★★MEASURED 2026-09-04: 239 bindings, of them **230 floor and 9 WALKED PURELY
LOCALLY** -- and the nine are converted (`allocate_slice[T]` + subscript,
opensp 8, dazzle 1), all gates green.  The class had been written off
wholesale ("367 hand-`allocate`d buffers whose local is handed on to a `pointer[T]`
callee -- the conversion is the CALLEE's, not the walk's"); that holds for the
230, not for all.  **A population is a claim, and an
exhaustion claim inherits it** -- the same lesson that already cost
derefcensus once.

★★★THE ESCAPE CHECKER DECIDES BY THE TARGET, NOT BY THE FUNCTION: a
`let p allocate_slice[T](host, n)` passes, a `set m.keys: allocate_slice[..]`
on a field of a parameter is a *reference into a local page escapes
via store*.  That is why exactly the local
binding is the convertible form.
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
                        reasons.add('non-deref use'); continue
                    if any(s[2] == '-' for s in spans): reasons.add('backwards')
                    if other_use(code, local, spans): reasons.add('additional use')
                if uses == 0:      st['UNUSED'] += 1
                elif not reasons:
                    st['WALKED PURELY LOCALLY'] += 1
                    hits.append(f'{p}:{a+k+1}  [{ty}]  {codes[k].strip()[:66]}')
                else:              st['floor: ' + ' + '.join(sorted(reasons))] += 1
for kk, n in st.most_common(): print(f'{n:5}  {kk}')
print('--- total', sum(st.values()))
if hits:
    print('\n=== WALKED PURELY LOCALLY ===')
    for h in hits: print(' ', h)
