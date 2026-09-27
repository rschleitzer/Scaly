#!/usr/bin/env python3
# tools/aliasing/census.py — the audit behind the aliasing rule (CLAUDE.md,
# "Memory: the aliasing rule"; tools/aliasing/README.md).
#
#   tools/pointer-report.sh <compiler> <out-dir>
#   tools/aliasing/census.py <out-dir>
#
# Classifies every `x as pointer[T]` / `as ref[T]` site of the per-root
# pointer reports by what it can do to a LIVE RECORD: a cast to void/char is
# the C boundary, a typed fresh allocation is new memory, an integer turned
# into a pointer is address arithmetic, a record viewed as ANOTHER type is what
# the rule is about. Distinct locations over all roots; the substrate
# (scaly/memory, scaly/fiber, scaly/cluster) is reported under its own name.
# Known false class: a fixed-size stack array decays to a pointer typed by its
# ELEMENT, so `buf as pointer[pointer[X]]` over `var buf pointer[X][3]` reads
# as a record reinterpretation (dazzle's decode_abc / decode_lmn).
import re, glob, collections, sys
sites = {}
for f in glob.glob(sys.argv[1] + '/*_*.txt'):
    if f.endswith('union.txt'): continue
    for line in open(f):
        m = re.match(r'pointer-report: (\S+):(\d+):(\d+): cast (.+) as (.+)$', line.strip())
        if m: sites[(m.group(1), int(m.group(2)), int(m.group(3)))] = (m.group(4), m.group(5))
SCAL = {'i8','i16','i32','i64','u8','u16','u32','u64','int','size_t','char','bool','float','double','const_char','Rune'}
BOUND = {'void','const_char','char'}
def pointee(t):
    m = re.match(r'(?:Option\[)?(?:pointer|ref)\[(.+)\]\]?$', t)
    return m.group(1) if m else None
src_cache = {}
def line_of(path, n):
    if path not in src_cache:
        try: src_cache[path] = open(path).read().split('\n')
        except: src_cache[path] = []
    L = src_cache[path]; return L[n-1] if 0 < n <= len(L) else ''
cls = collections.Counter(); ex = collections.defaultdict(list)
for (path, ln, col), (s, t) in sites.items():
    tp, sp = pointee(t), pointee(s)
    pkg = path.split('/')[1]
    sub = 'substrate' if re.search(r'scaly/(memory|fiber|cluster)', path) else pkg
    text = line_of(path, ln)
    if tp is None: k = 'target not a pointer'
    elif tp in BOUND: k = 'to void/char (C boundary, opaque)'
    elif sp is None and s in SCAL: k = 'integer to pointer (address arithmetic)'
    elif sp == 'void' and re.search(r'allocat|\bmalloc\(|\bcalloc\(', text): k = 'fresh allocation typed'
    elif sp == 'void': k = 'void to typed (provenance unknown)'
    elif sp is not None and sp == tp: k = 'same pointee (ref<->pointer)'
    elif sp is not None and sp not in SCAL and tp != sp: k = 'RECORD reinterpreted as other type'
    elif sp is not None and sp in SCAL and tp in SCAL: k = 'scalar buffer as other scalar'
    elif sp is not None and sp in SCAL: k = 'scalar buffer as record'
    else: k = 'other: ' + s
    cls[(k, sub)] += 1
    if k.startswith('RECORD') or k.startswith('scalar buffer as record'): ex[(k,sub)].append(f'{path}:{ln}: {s} as {t}  | {text.strip()[:90]}')
tot = collections.Counter()
for (k, sub), n in cls.items(): tot[k] += n
print(f'{len(sites)} cast sites (distinct locations, 24 roots)')
for k, n in tot.most_common():
    parts = ', '.join(f'{sub} {m}' for (kk, sub), m in sorted(cls.items(), key=lambda x: -x[1]) if kk == k)
    print(f'{n:5}  {k}   [{parts}]')
print()
for key, lst in sorted(ex.items()):
    print('==', key, len(lst))
    for e in lst[:12]: print('  ', e)
