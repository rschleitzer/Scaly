import re,sys,os,glob,collections
APPLY='--apply' in sys.argv
def bal(s,i):
    d=0
    for k in range(i,len(s)):
        if s[k]=='[': d+=1
        elif s[k]==']':
            d-=1
            if d==0: return k+1
    return -1
hits=[]
for path in glob.glob('packages/*/0.1.0/**/*.scaly',recursive=True):
    src=open(path,encoding='utf-8').read().split('\n')
    # every record field declared `name: ref[X]?`
    optf={}
    for l in src:
        c=l.split(';')[0]
        for m in re.finditer(r'(?<![:\w])([a-z_]\w*)\s*:\s*ref\[',c):
            ob=c.index('[',m.end()-1); cb=bal(c,ob)
            if cb>0 and cb<len(c) and c[cb]=='?': optf.setdefault(m.group(1),c[m.end()-4:cb])
    for i,l in enumerate(src):
        m=re.match(r'^\s*function\s+\w+\s*\(([^)]*)\)\s*returns\s+(ref\[[^\]]*\])\s*$',l.split(';')[0])
        if not m: continue
        body=src[i+1].split(';')[0].strip() if i+1<len(src) else ''
        if not re.match(r'^[a-z_]\w*$',body): continue      # body is a bare field read
        if body not in optf: continue
        if optf[body].rstrip('?')!=m.group(2): continue      # same pointee
        hits.append((path,i+1,body,m.group(2),l.rstrip()))
by=collections.defaultdict(list)
for h in hits: by[h[0]].append(h)
for path,items in by.items():
    src=open(path,encoding='utf-8').read().split('\n')
    for p,ln,fld,ty,orig in sorted(items,key=lambda x:-x[1]):
        src[ln-1]=src[ln-1].replace('returns '+ty,'returns '+ty+'?',1)
    if APPLY: open(path,'w',encoding='utf-8').write('\n'.join(src))
print(f'{len(hits)} accessors return a NON-optional ref of a field that IS optional')
for p,ln,fld,ty,orig in hits:
    print(f'  {p.split("/0.1.0/")[-1]}:{ln}  {orig.strip()[:78]}   (field {fld}: {ty}?)')
