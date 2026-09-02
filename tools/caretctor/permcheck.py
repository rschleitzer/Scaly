#!/usr/bin/env python3
"""Did a field-order permutation land every value in the right slot?

`opcheck.py` cannot see it (a GEP index is an OPERAND) and a raw store diff
cannot either (an SSA temp is renumbered whenever anything above it moves).
What IS decisive for a permutation of LITERALS: per (struct, field), the
LLVM TYPE of the stored value, and -- where the value is a CONSTANT -- the
constant itself.  If `included(false)` had landed in `attributes`'s slot, the
field would hold `i1 false` where it held `ptr null`.
"""
import re, sys, collections
GEP   = re.compile(r'^\s*(%\S+) = getelementptr inbounds nuw (%\S+), ptr (%\S+), i32 0, i32 (\d+)')
STORE = re.compile(r'^\s*store ([^,]+), ptr (%\S+), align')
CONST = re.compile(r'^(null|true|false|-?\d+|zeroinitializer)$')

def functions(path):
    out, cur, name = {}, None, None
    for l in open(path):
        if l.startswith('define'):
            m = re.search(r'@([\w.$]+)\(', l); name = m.group(1) if m else None; cur = []
            continue
        if name is None: continue
        if l.startswith('}'): out[name] = cur; name = None; continue
        cur.append(l.rstrip())
    return out

def slots(body):
    slot, out = {}, {}
    for l in body:
        m = GEP.match(l)
        if m: slot[m.group(1)] = (m.group(2), int(m.group(4))); continue
        m = STORE.match(l)
        if m and m.group(2) in slot:
            st, idx = slot[m.group(2)]
            v = m.group(1).strip()
            parts = v.rsplit(' ', 1)
            ty, val = (parts[0], parts[1]) if len(parts) == 2 else (v, '')
            out[(st, idx)] = (ty, val if CONST.match(val) else '<ssa>')
    return out

a, b = functions(sys.argv[1]), functions(sys.argv[2])
common = sorted(set(a) & set(b)); bad = []
for f in common:
    sa, sb = slots(a[f]), slots(b[f])
    if sa != sb: bad.append((f, sa, sb))
print('%d Funktionen gemeinsam, %d mit abweichendem (Feld -> Typ/Konstante)' % (len(common), len(bad)))
for f, sa, sb in bad[:15]:
    print('\n### %s' % f)
    for k in sorted(set(sa) | set(sb), key=lambda x: (x[0], x[1])):
        if sa.get(k) != sb.get(k):
            print('   %s Feld %d: alt %r  neu %r' % (k[0], k[1], sa.get(k), sb.get(k)))
