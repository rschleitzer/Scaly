import re,glob,collections,json,sys
FILES=glob.glob('packages/opensp/0.1.0/**/*.scaly',recursive=True)+glob.glob('packages/dazzle/0.1.0/**/*.scaly',recursive=True)
SRC={p:open(p,encoding='utf-8').read().split('\n') for p in FILES}
def code(l): return l.split(';')[0]
def split_args(s):
    out,d,cur=[],0,''
    for c in s:
        if c in '([': d+=1
        elif c in ')]': d-=1
        if c==',' and d==0: out.append(cur.strip()); cur=''
        else: cur+=c
    if cur.strip(): out.append(cur.strip())
    return out
# functions returning an Option
optret=set()
for p,src in SRC.items():
    for l in src:
        m=re.search(r'\b(?:function|procedure)\s+([a-z_]\w*)\s*\(.*\)\s*returns\s+ref\[.*\]\?\s*$',code(l).rstrip())
        if m: optret.add(m.group(1))
# optional names per file
optname=collections.defaultdict(set)
for p,src in SRC.items():
    for l in src:
        for m in re.finditer(r'(?<![:\w])([a-z_]\w*)\s*:\s*ref\[[^\]]*\]\?',code(l)): optname[p].add(m.group(1))
rows=[l.rstrip('\n').split('\t') for l in open('/tmp/w4.txt')]
targets={}
for loc,kind,base,ty,gen,text in rows:
    if kind!='param': continue
    path,ln=loc.rsplit(':',1); src=SRC[path]; i=int(ln)-1
    sig=None
    for k in range(i,-1,-1):
        if re.match(r'\s*(?:function|procedure)\s+[a-z_]\w*\s*\(',code(src[k])): sig=k; break
    if sig is None: continue
    txt=code(src[sig]); k=sig
    while txt.count('(')>txt.count(')') and k+1<len(src): k+=1; txt+=code(src[k])
    fn=re.match(r'\s*(?:function|procedure)\s+([a-z_]\w*)',txt).group(1)
    names=[a.split(':')[0].strip() for a in split_args(txt[txt.index('(')+1:txt.rindex(')')])]
    if base not in names: continue
    targets[(path,sig,fn,base,ty)]=(names.index(base),names and names[0]=='this')
res=collections.defaultdict(list)
for (path,sig,fn,base,ty),(idx,hasthis) in targets.items():
    ev=set()
    for cp,csrc in SRC.items():
        for li,l in enumerate(csrc):
            c=code(l)
            if re.match(r'\s*(?:function|procedure)\s',c): continue
            for m in re.finditer(r'(?<![\w])'+re.escape(fn)+r'\s*\(',c):
                o=m.end()-1; d=0; e=None
                for kk in range(o,len(c)):
                    if c[kk]=='(': d+=1
                    elif c[kk]==')':
                        d-=1
                        if d==0: e=kk; break
                if e is None: continue
                args=split_args(c[o+1:e])
                dotted=bool(re.search(r'\.\s*'+re.escape(fn)+r'\s*$',c[:m.end()-1]))
                j=idx-1 if (dotted and hasthis) else idx
                if j<0 or j>=len(args): continue
                a=args[j]
                if a=='null': ev.add('literal null')
                elif re.match(r'^[a-z_]\w*$',a) and a in optname[cp]: ev.add(f'nullable name `{a}`')
                else:
                    mm=re.search(r'([A-Za-z_]\w*)\s*\([^()]*\)\s*$',a)
                    if mm and mm.group(1) in optret: ev.add(f'nullable call `{mm.group(1)}()`')
    res['NULLABLE' if ev else 'no evidence'].append((path,sig+1,fn,base,ty,sorted(ev)[:2]))
print(f"{len(res['NULLABLE'])} params have caller evidence of null; {len(res['no evidence'])} do not\n")
for r in res['NULLABLE']: print(f"   {r[0].split('/0.1.0/')[-1]}:{r[1]}  {r[2]}({r[3]}: {r[4]})  <- {', '.join(r[5])}")
json.dump({k:[list(x) for x in v] for k,v in res.items()},open('/tmp/callev.json','w'))
