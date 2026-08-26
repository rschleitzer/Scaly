import re,sys,collections
APPLY='--apply' in sys.argv
rows=[l.rstrip('\n').split('\t') for l in open(sys.argv[1])]
# dedupe: several findings share one declaration
decls=collections.OrderedDict()
for loc,kind,base,ty,gen,text in rows:
    if kind!='local': continue
    path,ln=loc.rsplit(':',1); src=open(path,encoding='utf-8').read().split('\n')
    for k in range(int(ln)-1,max(0,int(ln)-400),-1):
        m=re.match(r'^(\s*)(let|var)\s+'+re.escape(base)+r'\s+(\S.*)$',src[k].split(';')[0])
        if m: decls.setdefault((path,k),(m.group(1),m.group(2),base,ty,m.group(3).rstrip(),src[k])); break
        if re.match(r'\s*(let|var)\s+'+re.escape(base)+r'\s*:',src[k]): break   # already annotated
by=collections.defaultdict(list)
for (path,k),v in decls.items(): by[path].append((k,v))
n=0
for path,items in by.items():
    src=open(path,encoding='utf-8').read().split('\n')
    for k,(ind,kw,base,ty,init,orig) in sorted(items,reverse=True):
        tail=orig.split(';',1)[1] if ';' in orig else ''
        src[k]=f'{ind}{kw} {base}: {ty}? {init}'+((' ;'+tail) if tail else '')
        n+=1
    if APPLY: open(path,'w',encoding='utf-8').write('\n'.join(src))
print(f'{n} local declarations annotated nullable')
for (path,k),v in list(decls.items())[:6]:
    print(f'   {path.split("/0.1.0/")[-1]}:{k+1}   {v[5].strip()[:60]}  ->  {v[1]} {v[2]}: {v[3]}? {v[4][:30]}')
