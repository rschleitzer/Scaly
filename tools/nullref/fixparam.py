import re,sys,collections
APPLY='--apply' in sys.argv
SAFE={'ref[GroveNode]','ref[ELObj]','ref[Insn]','ref[ElementType]','ref[AttributeDefinitionList]',
      'ref[MultiModeRec]','ref[Identifier]','ref[Entity]','ref[Origin]','ref[Part]'}
rows=[l.rstrip('\n').split('\t') for l in open('/tmp/prm.txt')]
F={}
def L(p):
    if p not in F: F[p]=open(p,encoding='utf-8').read().split('\n')
    return F[p]
edits=collections.defaultdict(set); skipped=0
for loc,kind,base,ty,gen,text in rows:
    if kind!='param' or ty not in SAFE: skipped+= (kind=='param'); continue
    path,ln=loc.rsplit(':',1); src=L(path); i=int(ln)-1
    sig=None
    for k in range(i,-1,-1):
        if re.match(r'\s*(?:function|procedure)\s+[a-z_]\w*\s*\(',src[k]): sig=k; break
    if sig is None: continue
    # the physical line carrying `base: ty` (signature may span lines)
    k=sig; txt=src[sig].split(';')[0]
    while txt.count('(')>txt.count(')') and k+1<len(src): k+=1; txt+=src[k].split(';')[0]
    for kk in range(sig,k+1):
        m=re.search(r'(?<![:\w])'+re.escape(base)+r'\s*:\s*'+re.escape(ty)+r'(?!\?)',src[kk])
        if m: edits[path].add((kk,base,ty)); break
n=0
for path,items in edits.items():
    src=L(path)
    for kk,base,ty in sorted(items,reverse=True):
        m=re.search(r'(?<![:\w])'+re.escape(base)+r'\s*:\s*'+re.escape(ty)+r'(?!\?)',src[kk])
        if not m: continue
        src[kk]=src[kk][:m.end()]+'?'+src[kk][m.end():]; n+=1
    if APPLY: open(path,'w',encoding='utf-8').write('\n'.join(src))
print(f'{n} parameters made nullable  (skipped {skipped} whose type is not pointer-dominant upstream)')
