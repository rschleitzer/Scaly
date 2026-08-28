# -*- coding: utf-8 -*-
"""Re-measure the `pointer[` residue, by category and by package.

The instrument behind the residue table in CLAUDE.md, committed because a
measurement claim owes its tool: the figures it replaced were stale the day
they were written, and nobody could tell without re-running the count.

Code lines only -- a `;` comment line is skipped and a trailing comment is cut
at the first `;` that is not inside a string, because a COMMENT once held 45
declarations that a hazard scan then reported as live.

★ It counts OCCURRENCES, including the inner half of a `pointer[pointer[X]]`,
so the categories overlap by construction; the header says so and the reader
must not add the rows up.  And a count is not a verdict: `refcell/scan.py` and
`refout/scan.py` are what separate a BUFFER from a CELL.
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

PKG=sys.argv[1] if len(sys.argv)>1 else 'packages'
rows=collections.Counter(); per=collections.defaultdict(collections.Counter)
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
            if is_extern: cat='extern'
            elif inner=='void': cat='void'
            elif inner=='Page': cat='Page'
            elif inner in ('const_char',): cat='cstring'
            elif inner in ('int','bool','u8','u16','u32','u64','i8','i16','i32','i64','size_t','char','double','float','byte'): cat='primitive'
            elif re.fullmatch(r'[A-Z]', inner): cat='generic-param'
            elif inner.startswith('pointer['): cat='pointer-to-pointer'
            elif re.match(r'LLVM|.*Ref$', inner): cat='llvm-handle'
            else: cat='concept'
            rows[cat]+=1; per[pkg][cat]+=1
            detail[cat].append((f,n,inner,raw.rstrip()))
print(f"{'category':20s} total  " + "  ".join(f"{p:9s}" for p in sorted(per)))
for cat,_ in rows.most_common():
    print(f"{cat:20s} {rows[cat]:5d}  " + "  ".join(f"{per[p][cat]:9d}" for p in sorted(per)))
print(f"{'TOTAL':20s} {sum(rows.values()):5d}  " + "  ".join(f"{sum(per[p].values()):9d}" for p in sorted(per)))
if '--dump' in sys.argv:
    import pickle; pickle.dump(dict(detail), open('/tmp/ptr_detail.pkl','wb'))
