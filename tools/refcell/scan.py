#!/usr/bin/env python3
"""Find POINTER-POINTEE out-cell parameters -- `pointer[pointer[X]]` naming a
single cell the callee writes a reference into, not a buffer of pointers it
walks.

The route is the one recorded as proven and never executed: a
`ref[ref[X]?]` parameter deref-stored from a caller's `var cell: ref[X]? null`
answers correctly on both the taken and the untaken branch.  The old reason for
the inner pointer -- "the outer pointer is what makes it writable, so the inner
one must match" -- is false: a `ref` is writable through the target position,
so neither pointer is needed.

The verdicts are `refout`'s, for the same reason: a BUFFER proves itself by
ARITHMETIC or INDEXING on the name, a FORWARDER proves nothing on its own, so
the sinks seed a fixpoint and each forwarder inherits its callee's verdict.  An
UNSEEN parameter is one this tool has no evidence about, which is not evidence
that it is a cell, and is never converted.

Two exclusions this tool owes that `refout` does not:

  * a POINTEE that is not a concept (`char`, `const_char`, `void`).  A
    `pointer[pointer[const_char]]` out-cell hands a C string back to a caller on
    its way to an extern -- accepted shape 2 -- and `ref[const_char]` is not a
    borrowed concept reference.

  * a routine whose ITANIUM MANGLED NAME is written out as a `"_Z..."` literal
    anywhere in the tree.  The mangling encodes the parameter types and the
    compiler LOOKS THESE UP, so a rename is an undefined symbol at link time.
    The set is derived from the tree, never hardcoded.
"""
import re, os, sys, subprocess, collections

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refout'))
from scan import (collect, split_args, ARITH, STORE, DEREF, CALL, PEELED,
                  signature, strip_comment, DECL)

NON_CONCEPT = ('void', 'char', 'const_char', 'u8', 'i8', 'byte')

def frozen_names(root='.'):
    """Routine names whose mangled symbol is spelled out as a string literal."""
    out = set()
    txt = subprocess.run(['grep', '-rho', '"_Z[A-Za-z0-9_]*"', '--include=*.scaly', root],
                         capture_output=True, text=True).stdout
    for lit in set(txt.split()):
        lit = lit.strip('"')
        m = re.match(r'^_Z(?:N\d+[A-Za-z_]\w*)?(\d+)([A-Za-z_]\w*)', lit)
        if m:
            out.add(m.group(2)[:int(m.group(1))])
    return out

def candidates(routines, frozen):
    cand = {}
    for ri, r in enumerate(routines):
        if r['fn'] in frozen: continue
        for pi, (pn, pt) in enumerate(r['params']):
            m = re.match(r'pointer\[\s*pointer\[\s*(\w+)\s*\]\s*\]$', pt.strip())
            if m and m.group(1) not in NON_CONCEPT and m.group(1)[:1].isupper():
                cand[(ri, pi)] = m.group(1)
    return cand

def analyse(routines, cand):
    """BUFFER is an absorbing state on the ALIAS graph, not a one-way inheritance.

    A bare parameter passed into another candidate slot IS that slot's value, so
    the two must reach the same verdict and evidence flows BOTH ways.  Reading it
    as callee-to-caller only was measurably wrong: `Primitive.dispatch(args)` is
    a proven buffer that hands `args` to forty leaf primitives, and the leaves
    that happen to read only `*args` -- element ZERO of that array -- came back
    as cells.  A companion COUNT parameter is the reader's tell; the flood is the
    tool's.
    """
    verdict = {}
    for (ri, pi) in cand:
        nm = routines[ri]['params'][pi][0]
        body = routines[ri]['body']
        if any(rx.search(body) for rx in ARITH(nm)):
            verdict[(ri, pi)] = 'BUFFER'
        elif STORE(nm).search(body) or DEREF(nm).search(body):
            verdict[(ri, pi)] = 'CELL'
        else:
            verdict[(ri, pi)] = 'UNSEEN'

    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines): by_name[r['fn']].append(ri)

    adj = collections.defaultdict(set)
    for (ri, pi) in cand:
        for key in aliases(routines, by_name, cand, ri, routines[ri]['params'][pi][0]):
            adj[(ri, pi)].add(key); adj[key].add((ri, pi))

    work = [k for k in cand if verdict[k] == 'BUFFER']
    while work:
        k = work.pop()
        for nb in adj[k]:
            if verdict.get(nb) != 'BUFFER':
                verdict[nb] = 'BUFFER'; work.append(nb)
    return verdict

def aliases(routines, by_name, cand, ri, nm):
    """Candidate slots this routine's parameter `nm` is passed into, bare."""
    out = set()
    body = routines[ri]['body']
    bare = re.compile(r'^\s*&?%s\s*$' % re.escape(nm))
    for m in CALL.finditer(body):
        callee = m.group(1)
        if callee not in by_name: continue
        args = split_args(body, m.end() - 1)
        pos = [k for k, a in enumerate(args) if bare.match(a)]
        if not pos: continue
        for cri in by_name[callee]:
            params = routines[cri]['params']
            offs = [0] + ([1] if params and params[0][0] in PEELED else [])
            for off in offs:
                for k in pos:
                    if (cri, k + off) in cand: out.add((cri, k + off))
    return out

if __name__ == '__main__':
    paths = [a for a in sys.argv[1:] if not a.startswith('--')] or ['packages']
    routines = collect(paths)
    frozen = frozen_names()
    cand = candidates(routines, frozen)
    verdict = analyse(routines, cand)
    per = collections.Counter()
    for key in sorted(cand, key=lambda k: (routines[k[0]]['file'], routines[k[0]]['line'])):
        ri, pi = key; r = routines[ri]
        print(f"{verdict[key]:7} pointer[pointer[{cand[key]}]] "
              f"{r['file']}:{r['line']} {r['fn']}({r['params'][pi][0]})")
        per[(r['file'].split('/')[1], verdict[key])] += 1
    print('---')
    for k, v in sorted(per.items()): print(f'{v:5}  {k[0]:8} {k[1]}')
    print(f'--- {len(cand)} candidates; frozen names skipped: {len(frozen)}')
