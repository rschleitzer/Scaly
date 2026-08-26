import re,glob,collections,sys
APPLY='--apply' in sys.argv
FILES=glob.glob('packages/opensp/0.1.0/**/*.scaly',recursive=True)+glob.glob('packages/dazzle/0.1.0/**/*.scaly',recursive=True)
SRC={p:open(p,encoding='utf-8').read().split('\n') for p in FILES}
def code(l): return l.split(';')[0]
# UNANIMITY: a name whose every ref-returning declaration is optional
decl=collections.defaultdict(set)
for p,src in SRC.items():
    for l in src:
        m=re.search(r'\b(?:function|procedure)\s+([a-z_]\w*)\s*\(.*\)\s*returns\s+(ref\[.*?\]\??)\s*$',code(l).rstrip())
        if m: decl[m.group(1)].add(m.group(2).endswith('?'))
optret={k for k,v in decl.items() if v=={True}}
hits=[]
for p,src in SRC.items():
    for i,l in enumerate(src):
        m=re.match(r'^\s*function\s+(\w+)\s*\(.*\)\s*returns\s+(ref\[[A-Za-z]\w*\])\s*$',code(l).rstrip())
        if not m: continue
        fn,ty=m.group(1),m.group(2)
        j=i+1
        if j>=len(src): continue
        if code(src[j]).strip()=='{':
            d=0; end=None
            for k in range(j,min(j+300,len(src))):
                d+=code(src[k]).count('{')-code(src[k]).count('}')
                if d==0: end=k; break
            if end is None: continue
            body=[code(x).strip() for x in src[j+1:end] if code(x).strip()]
        else: body=[code(src[j]).strip()]
        if not body: continue
        why=None
        for idx,b in enumerate(body):
            r=re.match(r'^return\s+(.*)$',b)
            e=r.group(1).strip() if r else (b if idx==len(body)-1 else None)
            if not e: continue
            mm=re.search(r'([A-Za-z_]\w*)\s*\([^()]*\)\s*$',e)
            if mm and mm.group(1) in optret and mm.group(1)!=fn:
                why=f'tail/return calls {mm.group(1)}() which answers an Option'; break
        # ★Two standing conditions in the root CLAUDE.md, neither of which a
        # sweep may cross: `callcc_call` silently drops jit_apply /
        # jit_function_tail_call from emission when it answers an Option (the
        # link is the only evidence), and `Primitive::dispatch` /
        # `dispatch_rest` / `prim_call` must stay PAGE-FREE -- a region frame
        # is contagious across the whole function and no arm of the thin shell
        # may demand the caller side.
        # ★★★A function the JIT MIRRORS (a `jit_<name>` helper exists) must not
        # change its declaration: those helpers are referenced only by ADDRESS
        # for the pinned (void*, void*) C ABI, so a changed signature drops the
        # helper from emission silently and only `ld` says so.
        if fn in ('callcc_call','dispatch','dispatch_rest','prim_call'): continue
        if ('function jit_'+fn+'(') in ''.join(SRC.get('packages/dazzle/0.1.0/dazzle/Jit.scaly',[])): continue
        if why: hits.append((p,i+1,fn,ty,why))
by=collections.defaultdict(list)
for h in hits: by[h[0]].append(h)
for p,items in by.items():
    src=SRC[p]
    for _,ln,fn,ty,why in sorted(items,key=lambda x:-x[1]):
        src[ln-1]=re.sub(r'(returns\s+'+re.escape(ty)+r')(?!\?)',r'\1?',src[ln-1],1)
    if APPLY: open(p,'w',encoding='utf-8').write('\n'.join(src))
print(f'{len(hits)} functions tail-return an Option under a NON-optional declaration')
for h in hits: print(f'   {h[0].split("/0.1.0/")[-1]}:{h[1]}  {h[2]} -> {h[3]}   ({h[4]})')
