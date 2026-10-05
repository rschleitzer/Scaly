#!/usr/bin/env python3
# tools/layout-report.py — what a record's field order costs in padding.
#
#   scalyc -S --no-prelude -o /tmp/x.ll packages/<pkg>/0.1.0/<pkg>.scaly
#   python3 tools/layout-report.py /tmp/x.ll [top-n]
#
# Reads every `%name = type { … }` of an emission, computes size and alignment
# by LLVM's rules (i1/i8 1, i32 4, i64/ptr 8, arrays and structs by element) and
# compares the DECLARED order with the same fields sorted by alignment. Records
# are laid out in declaration order, so the
# difference is what an automatic reordering would save PER INSTANCE.
#
# ★★★Measured 2026-09-18, and the number is why the compiler does NOT reorder:
# tscaly 38 of 2314 records carry a hole and 632 bytes per instance in total,
# dazzle 26 of 1262 (504 B), opensp 19 of 698 (352 B), scalyc 15 of 1207 (200 B)
# — and the biggest savers are SINGLETONS (Checker 104, Transformer 96,
# ModuleResolver 48). The mass records (AstNode, Type, Symbol, the link rows)
# have no hole at all, because slices 332–345 packed them by hand. Re-run this
# before proposing the compiler change again: it pays only if a MASS record
# shows up with a hole.
# size/alignment by LLVM's rules for the shapes our emission uses
import re, sys
def parse_fields(body):
    out=[]; depth=0; cur=''
    for ch in body:
        if ch=='{' or ch=='[' or ch=='(': depth+=1
        if ch=='}' or ch==']' or ch==')': depth-=1
        if ch==',' and depth==0: out.append(cur.strip()); cur=''
        else: cur+=ch
    if cur.strip(): out.append(cur.strip())
    return out

def sa(t, types):
    t=t.strip()
    m=re.match(r'^i(\d+)$', t)
    if m:
        bits=int(m.group(1)); n=max(1,(bits+7)//8)
        return n, min(n, 8) if n in (1,2,4,8) else 1
    if t in ('ptr','float','double'):
        return (8,8) if t in ('ptr','double') else (4,4)
    m=re.match(r'^\[(\d+) x (.+)\]$', t)
    if m:
        n=int(m.group(1)); es,ea=sa(m.group(2), types); return n*es, ea
    if t.startswith('{'):
        return struct_sa(parse_fields(t[1:-1]), types)
    if t.startswith('%'):
        b=types.get(t.split()[0])
        if b is None: return 8,8
        return struct_sa(parse_fields(b), types)
    return 8,8

def struct_sa(fields, types):
    off=0; al=1
    for f in fields:
        s,a=sa(f, types)
        al=max(al,a)
        off=(off+a-1)//a*a
        off+=s
    return (off+al-1)//al*al, al

path=sys.argv[1]
types={}
for line in open(path):
    m=re.match(r'^(%[\w.]+) = type \{(.*)\}$', line.strip())
    if m: types[m.group(1)]=m.group(2)
rows=[]
for name, body in types.items():
    fs=parse_fields(body)
    if len(fs)<2: continue
    cur,_=struct_sa(fs, types)
    best,_=struct_sa(sorted(fs, key=lambda f: -sa(f,types)[1]), types)
    if cur>best: rows.append((cur-best, cur, best, name, len(fs)))
rows.sort(reverse=True)
print("%-6s %-6s %-6s %-5s %s" % ("save","now","packed","flds","record"))
for d,c,b,n,k in rows[:20]: print("%-6d %-6d %-6d %-5d %s" % (d,c,b,k,n[:70]))
print("\nrecords with a hole: %d of %d; bytes saved per instance, summed: %d" % (len(rows), len(types), sum(r[0] for r in rows)))
