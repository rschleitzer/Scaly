import re,sys,glob,collections
APPLY='--apply' in sys.argv
hits=[]
for path in glob.glob('packages/opensp/0.1.0/**/*.scaly',recursive=True)+glob.glob('packages/dazzle/0.1.0/**/*.scaly',recursive=True):
    src=open(path,encoding='utf-8').read().split('\n')
    # containers whose ELEMENT is optional: name -> X
    optel={}
    for l in src:
        c=l.split(';')[0]
        for m in re.finditer(r'(?<![:\w])([a-z_]\w*)\s*:\s*(?:ref\[)?(?:Array|Vector)\[ref\[([A-Za-z]\w*)\]\?\]',c):
            optel[m.group(1)]=m.group(2)
    if not optel: continue
    for i,l in enumerate(src):
        m=re.match(r'^\s*function\s+(\w+)\s*\([^)]*\)\s*returns\s+ref\[([A-Za-z]\w*)\]\s*$',l.split(';')[0])
        if not m: continue
        fn,ty=m.group(1),m.group(2)
        # gather up to 6 body lines
        body='\n'.join(x.split(';')[0] for x in src[i+1:i+7])
        mm=re.search(r'\*\s*\(\s*([a-z_]\w*)\s*\.\s*get_buffer\s*\(\s*\)\s*\+',body)
        if not mm: continue
        base=mm.group(1)
        if optel.get(base)!=ty: continue
        hits.append((path,i+1,fn,ty,base))
by=collections.defaultdict(list)
for h in hits: by[h[0]].append(h)
for path,items in by.items():
    src=open(path,encoding='utf-8').read().split('\n')
    for p,ln,fn,ty,base in sorted(items,key=lambda x:-x[1]):
        src[ln-1]=re.sub(r'(returns\s+ref\['+re.escape(ty)+r'\])(?!\?)',r'\1?',src[ln-1],1)
    if APPLY: open(path,'w',encoding='utf-8').write('\n'.join(src))
print(f'{len(hits)} accessors return a NON-optional ref of an OPTIONAL container element')
for p,ln,fn,ty,base in hits: print(f'   {p.split("/0.1.0/")[-1]}:{ln}  {fn}(...) returns ref[{ty}]   <- {base}: Array[ref[{ty}]?]')
