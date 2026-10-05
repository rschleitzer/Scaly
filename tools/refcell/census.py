# -*- coding: utf-8 -*-
"""Re-measure the `pointer[` residue, by category and by package.

The instrument behind the residue table, committed because a
measurement claim owes its tool: the figures it replaced were stale the day
they were written, and nobody could tell without re-running the count.

Code lines only -- a `;` comment line is skipped and a trailing comment is cut
at the first `;` that is not inside a string, because a COMMENT once held 45
declarations that a hazard scan then reported as live.

★ It counts OCCURRENCES, including the inner half of a `pointer[pointer[X]]`,
so the categories overlap by construction; the header says so and the reader
must not add the rows up.  And a count is not a verdict: `refcell/scan.py` and
`refout/scan.py` are what separate a BUFFER from a CELL.

★ `--shapes` adds the SYNTACTIC breakdown, which is the half the pointee
categories cannot show: an `as` cast, a `sizeof`, an `extern` line and a
DECLARATION are four different claims, and only the last is a signature a
reader litigates.  It also cross-tabs the declarations by pointee, so the
question "what is still written down in a signature" has one answer instead
of a subtraction.
"""
import re, sys, glob, collections

def code(line):
    out=[];i=0;q=None
    while i<len(line):
        c=line[i]
        if q:
            if c=='\\': i+=2; out.append('  '); continue
            if c==q: q=None
            out.append(c); i+=1; continue
        if c in '"\'': q=c; out.append(c); i+=1; continue
        if c==';': break
        out.append(c); i+=1
    return ''.join(out)

PRIM=('int','bool','u8','u16','u32','u64','i8','i16','i32','i64',
      'size_t','char','double','float','byte')

def pointee_category(inner, is_extern):
    if is_extern: return 'extern'
    if inner=='void': return 'void'
    if inner=='Page': return 'Page'
    if inner=='const_char': return 'cstring'
    if inner in PRIM: return 'primitive'
    if re.fullmatch(r'[A-Z]', inner): return 'generic-param'
    if inner.startswith('pointer['): return 'pointer-to-pointer'
    if re.match(r'LLVM|.*Ref$', inner): return 'llvm-handle'
    return 'concept'

def shape(pre, is_extern, line):
    """What the occurrence IS, read off what stands in front of it."""
    if is_extern: return 'extern decl'
    if re.search(r'\bas\s+$', pre):
        # the `allocate(...) as pointer[X]` idiom is where raw memory becomes a
        # typed object -- deliberately untouched, and worth its own row.
        return 'as-cast (allocate)' if 'allocate(' in line else 'as-cast'
    if re.search(r'\b(sizeof|alignof)\s+$', pre): return 'sizeof/alignof'
    if re.search(r'\breturns\s+$', pre): return 'returns'
    if re.search(r':\s*$', pre): return 'typed decl'
    return 'other'          # overwhelmingly the inner half of pointer[pointer[X]]

def table(title, rows, per, pkgs, w=20):
    print(f"{title:{w}s} total  " + "  ".join(f"{p:9s}" for p in pkgs))
    for k,_ in rows.most_common():
        print(f"{k:{w}s} {rows[k]:5d}  " + "  ".join(f"{per[p][k]:9d}" for p in pkgs))
    print(f"{'TOTAL':{w}s} {sum(rows.values()):5d}  "
          + "  ".join(f"{sum(per[p][k] for k in rows):9d}" for p in pkgs))

args=[a for a in sys.argv[1:] if not a.startswith('-')]
PKG=args[0] if args else 'packages'
rows=collections.Counter(); per=collections.defaultdict(collections.Counter)
srows=collections.Counter(); sper=collections.defaultdict(collections.Counter)
drows=collections.Counter(); dper=collections.defaultdict(collections.Counter)
detail=collections.defaultdict(list)
for f in glob.glob(PKG+'/**/*.scaly', recursive=True):
    pkg=f.split('/')[1] if f.startswith('packages/') else PKG
    for n,raw in enumerate(open(f,encoding='utf-8',errors='replace'),1):
        c=code(raw)
        if 'pointer[' not in c: continue
        is_extern = re.search(r'\bextern\b', c) is not None
        for m in re.finditer(r'pointer\[', c):
            # take the inner type name
            j=m.end(); depth=1; k=j
            while k<len(c) and depth:
                if c[k]=='[': depth+=1
                elif c[k]==']': depth-=1
                k+=1
            inner=c[j:k-1].strip()
            cat=pointee_category(inner, is_extern)
            rows[cat]+=1; per[pkg][cat]+=1
            detail[cat].append((f,n,inner,raw.rstrip()))
            s=shape(c[:m.start()], is_extern, c)
            srows[s]+=1; sper[pkg][s]+=1
            if s in ('typed decl','returns'):
                d=pointee_category(inner, False)
                drows[d]+=1; dper[pkg][d]+=1
pkgs=sorted(per)
table('category', rows, per, pkgs)
if '--shapes' in sys.argv:
    print(); table('shape', srows, sper, pkgs)
    print(); table('declared pointee', drows, dper, pkgs)
if '--dump' in sys.argv:
    import pickle; pickle.dump(dict(detail), open('/tmp/ptr_detail.pkl','wb'))
