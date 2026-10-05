"""What REALLY happens in the body to an UNWALKED buffer parameter?

`refslice` says only `not walks(body, p)` -- no arithmetic, no
indexing -- and rightly calls that the absence of evidence.  This
breakdown asks WHAT happens instead.  Exactly this question brought out,
for the hand buffers, nine convertible sites from under a statement about
a class.

★★★MEASURED 2026-09-04 over all packages: **427 UNWALKED, of them 0 dead
parameters and 280 pure forwarders.**  The class is thus what the
refslice comment says -- absence of evidence -- but the
forwarding TARGETS are the real information: `kw_eq` leads the list with
85, and it was through exactly these 85 that this round's find came (the GENERATOR
of `Keywords.scaly` still produced the pre-Slice signature, a regeneration would
have silently reverted the conversion at 88 sites).

★★★THE TRAP THIS SCRIPT FIRST SET ITSELF: `collect()` delivers
`body` as a STRING.  `for l in r['body']` then runs over CHARACTERS, never finds
a parameter name and reports EVERY parameter as "unused" -- the first
run said 314 dead parameters, and the number looked plausible.  Hence the
self-check below: it proves that the instrument fires before a number
is believed.
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

# ★★★SELF-CHECK: `body` is a STRING, not a list of lines -- an iteration
# over it runs over CHARACTERS and NEVER finds a parameter name, whereupon
# EVERY parameter counts as "unused".  That is exactly how this script first
# reported 314 dead parameters.  An instrument that cannot fire is
# worse than none -- so prove first that it fires.
_probe = [x for x in routines if x['fn'] == 'usage_put_stringc']
assert _probe, 'probe routine not found'
assert 'bp' in _probe[0]['body'], 'SELF-CHECK FAILED: the body does not see the parameter'

for r in routines:
    for pn, pt in r['params']:
        m = re.match(r'pointer\[\s*(\w+)\s*\]$', pt.strip())
        if not m: continue
        elem = m.group(1)
        if elem in NON_ELEMENT: continue
        if rs.walks(r['body'], pn): continue          # UNWALKED only
        body = [strip_comment(l) for l in r['body'].split('\n')]
        occ = [l for l in body if re.search(rf'\b{re.escape(pn)}\b', l)]
        if not occ:
            st['UNUSED (dead parameter)'] += 1
            if len(ex['UNUSED (dead parameter)']) < 6:
                ex['UNUSED (dead parameter)'].append(f"{r['file']}:{r['line']}  {r['fn']}({pn})")
            continue
        kinds = set()
        for l in occ:
            if re.search(rf'\*\s*{re.escape(pn)}\b', l):        kinds.add('bare deref *p')
            if re.search(rf'\b{re.escape(pn)}\s*(=|<>)\s*null', l) or \
               re.search(rf'null\s*(=|<>)\s*\b{re.escape(pn)}\b', l): kinds.add('null test')
            if re.search(rf'\b{re.escape(pn)}\s+as\b', l):      kinds.add('cast')
            mm = re.findall(r'\b([A-Za-z_][A-Za-z0-9_.]*)\s*\(', l)
            if mm and re.search(rf'\(\s*[^)]*\b{re.escape(pn)}\b', l):
                kinds.add('handed on as an argument')
                for t in mm: fwd_targets[t.split('.')[-1]] += 1
            if re.search(rf'set\s+\b{re.escape(pn)}\b', l):     kinds.add('reassigned')
        k = ' + '.join(sorted(kinds)) or 'other mention'
        st[k] += 1
        if len(ex[k]) < 4:
            ex[k].append(f"{r['file']}:{r['line']}  {r['fn']}({pn}: pointer[{elem}])")
for k, n in st.most_common():
    print(f'{n:5}  {k}')
    for e in ex[k]: print(f'         {e}')
print('--- total', sum(st.values()))
print('\n=== most frequent forwarding targets ===')
for t, n in fwd_targets.most_common(12): print(f'{n:5}  {t}')
