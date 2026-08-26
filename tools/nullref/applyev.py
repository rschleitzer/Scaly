import re,json,sys,collections
APPLY='--apply' in sys.argv
d=json.load(open('/tmp/callev.json'))
apply_,hold=[],[]
for path,ln,fn,base,ty,ev in d['NULLABLE']:
    strong=[e for e in ev if e=='literal null' or e.startswith('nullable call')]
    selfref=[e for e in ev if e==f'nullable name `{base}`']
    other=[e for e in ev if e.startswith('nullable name') and e not in selfref]
    if strong or other: apply_.append((path,ln,fn,base,ty,(strong+other)[0]))
    else: hold.append((path,ln,fn,base,ty,ev))
print(f'{len(apply_)} to apply, {len(hold)} held (evidence is the parameter\'s OWN name -- possibly circular)')
for h in hold: print(f'   HOLD {h[0].split("/0.1.0/")[-1]}:{h[1]} {h[2]}({h[3]})  <- {h[5]}')
by=collections.defaultdict(list)
for path,ln,fn,base,ty,why in apply_: by[path].append((ln,base,ty,why))
n=0
for path,items in by.items():
    src=open(path,encoding='utf-8').read().split('\n')
    for ln,base,ty,why in sorted(items,reverse=True):
        k=ln-1; txt=src[k].split(';')[0]; kk=k
        while txt.count('(')>txt.count(')') and kk+1<len(src): kk+=1; txt+=src[kk].split(';')[0]
        for j in range(k,kk+1):
            m=re.search(r'(?<![:\w])'+re.escape(base)+r'\s*:\s*'+re.escape(ty)+r'(?!\?)',src[j])
            if m: src[j]=src[j][:m.end()]+'?'+src[j][m.end():]; n+=1; break
    if APPLY: open(path,'w',encoding='utf-8').write('\n'.join(src))
print(f'\n{n} parameters made nullable')
