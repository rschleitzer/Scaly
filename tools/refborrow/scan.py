#!/usr/bin/env python3
"""Find container parameters that are BORROWED but spelled as VALUES.

`function collect_methods_in(..., acc: Array[Function])` appends to `acc` and
the CALLER reads the result back.  That works only because the emitter lowers
every by-value struct parameter to a POINTER, which makes the declaration a lie
that the ABI happens to honour -- see packages/scalyc/CLAUDE.md, "A STRUCT
PARAMETER IS BORROWED AND MUTABLE".  The honest spelling is `ref[Array[T]]`,
which is this campaign's own doctrine applied to the slice it never swept.

Two kinds of evidence, and the second is a FIXPOINT:

  SINK       the body calls a mutating method on the name (`acc.add f`).
             ★Parenless calls count -- `acc.add f` has no parenthesis, and a
             regex demanding one undercounted this class by half.

  FORWARDER  the name is passed into a parameter position already known to be
             borrowed.  A forwarder proves nothing on its own, so the sinks
             seed the fixpoint and it runs to closure -- the same shape
             refout/refcell use, and for the same reason.

Reported and never converted:
  UNSEEN     no mutation and no forwarding into a borrowed position.  That is
             the absence of evidence, not evidence of a copy.
"""
import re, os, sys, collections

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refout'))
from scan import collect, split_args, PEELED

CONTAINERS = ('Array', 'Vector', 'List', 'HashMap', 'HashSet', 'StringBuilder')
MUT = r'(add|put|remove|clear|append|insert|reallocate|push|pop)\w*'

def pkg_of(path):
    p = path.split(os.sep)
    return p[p.index('packages') + 1] if 'packages' in p else '?'

def candidates(routines):
    out = {}
    for ri, r in enumerate(routines):
        for pi, (pn, pt) in enumerate(r['params']):
            t = pt.strip()
            if pn in PEELED or not t: continue
            if t.startswith(('ref[', 'pointer[')): continue
            b = re.match(r'^([A-Z]\w*)', t)
            if b and b.group(1) in CONTAINERS:
                out[(ri, pi)] = t
    return out

def main(paths):
    routines = collect(paths)
    cand = candidates(routines)

    verdict = {}
    for key, t in cand.items():
        ri, pi = key
        nm = routines[ri]['params'][pi][0]
        if re.search(rf'\b{re.escape(nm)}\.{MUT}\s*[\(\s]', routines[ri]['body']):
            verdict[key] = 'SINK'

    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines):
        by_name[r['fn']].append(ri)

    # fixpoint: a candidate handed into an already-borrowed position is borrowed
    changed = True
    while changed:
        changed = False
        # callee fn -> ARGUMENT indices as written at a call site.
        # ★A method's declaration carries `this` as parameter 0 while the call
        # site does not, so the declaration index is the argument index PLUS
        # ONE. Without the shift every forwarder in a method family scores
        # UNSEEN -- which is exactly how `collect_lambda_names_action` and its
        # four siblings were missed and only caught downstream by a name match.
        borrowed_args = collections.defaultdict(set)
        for (ri, pi) in verdict:
            params = routines[ri]['params']
            shift = 1 if params and params[0][0] in PEELED else 0
            borrowed_args[routines[ri]['fn']].add(pi - shift)
        for key, t in cand.items():
            if key in verdict: continue
            ri, pi = key
            nm = routines[ri]['params'][pi][0]
            for callee, idxs in borrowed_args.items():
                # ★The argument list must be matched with BALANCED parens: a
                # `[^()]*` body stops at the first nested call, and every
                # `collect_lambda_names_*` forwarder passes `(*p)`-shaped
                # arguments -- so the whole forwarder family scored UNSEEN and
                # was only caught downstream by a NAME coincidence.
                for m in re.finditer(rf'\b{re.escape(callee)}\s*\(', routines[ri]['body']):
                    body = routines[ri]['body']
                    depth, end = 0, None
                    for k in range(m.end() - 1, len(body)):
                        if body[k] == '(': depth += 1
                        elif body[k] == ')':
                            depth -= 1
                            if depth == 0: end = k; break
                    if end is None: continue
                    args = [a.strip() for a in split_args(body[m.end() - 1:end + 1], 0)]
                    for ai, a in enumerate(args):
                        if a == nm and ai in idxs:
                            verdict[key] = 'FORWARDER'; changed = True; break
                    if key in verdict: break
                if key in verdict: break

    rows = []
    for key, t in sorted(cand.items()):
        ri, pi = key
        r = routines[ri]
        rows.append((verdict.get(key, 'UNSEEN'), pkg_of(r['file']), r['file'], r['line'],
                     r['fn'], r['params'][pi][0], t))
    return rows

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    rows = main(args or ['packages'])
    tally = collections.Counter()
    for v, pkg, f, ln, fn, pn, t in rows:
        tally[(v, pkg)] += 1
        if v != 'UNSEEN' or '-v' in sys.argv:
            print(f"{v:10s} {f}:{ln} {fn}({pn}: {t})")
    print('---')
    for (v, pkg), n in sorted(tally.items(), key=lambda kv: (-kv[1], kv[0])):
        print(f"{n:5d}  {pkg:8s} {v}")
