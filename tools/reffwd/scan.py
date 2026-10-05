#!/usr/bin/env python3
"""Find pointer parameters that are FORWARDED into an already-converted
`ref[...]` position -- the residue every previous fixpoint was blind to.

`refout` and `refcell` seed their fixpoint from the SINKS and propagate a
BUFFER verdict upward: a parameter handed to a proven buffer is a buffer.
That direction cannot see the opposite evidence.  A pure forwarder derefs
nothing and walks nothing, so it scores UNSEEN -- "absence of evidence" --
even when its callee has ALREADY been converted and is declared `ref`.
The lesson is recorded twice (a dead forwarder is invisible to a
fixpoint; seed from the CALLEE side) without a tool that applies it.

The evidence here is not a body but a CALL: the parameter is passed BARE
into a position the callee declares `ref[...]`, whose pointee matches.  A
pointer argument passes to a `ref` parameter without a cast, so such a site
compiles today and says nothing -- which is exactly why it survived.

BUFFER still wins: arithmetic or indexing on the name is proof about THIS
body and outranks any call it makes.  The tool reports; the reader decides.
"""
import re, sys, os, collections, importlib.util, subprocess

_ro = os.path.dirname(os.path.abspath(__file__)) + '/../refout/scan.py'
_spec = importlib.util.spec_from_file_location('refout_scan', _ro)
refout = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(refout)
collect, split_args, ARITH, PEELED = (refout.collect, refout.split_args,
                                      refout.ARITH, refout.PEELED)

CALL = re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\(')


def frozen_names(root='.'):
    """Routines whose Itanium mangled name is written out as a string literal.

    The compiler LOOKS THESE UP rather than calling them, and the mangling
    encodes the parameter types, so re-signing one is an undefined symbol at
    link time.  `refpage/scan.py` freezes only the names carrying `P4Page`;
    here ANY pointer parameter is at stake -- `Channel.send` is `P7ChannelPv`
    and `TaskPool.submit` is `P8TaskPoolPvPvP9TaskGroup` -- so every literal
    counts.  Derived from the tree, never hardcoded.
    """
    out = set()
    try:
        txt = subprocess.run(['grep', '-rho', '"_Z[A-Za-z0-9_]*"',
                              '--include=*.scaly', root],
                             capture_output=True, text=True).stdout
    except Exception:
        return out
    for lit in set(txt.split()):
        out |= {n for n in (itanium_routine(lit.strip('"')),) if n}
    return out


def itanium_routine(lit):
    """The routine name inside an Itanium symbol, or None.

    `_ZN7Channel4sendEP7ChannelPv` -> `send`; `_Z23scaly_release_root_pageP4Page`
    -> `scaly_release_root_page`.  A greedy regex answers `Pv` and `de` here --
    the nested form must be walked component by component, and the routine is
    the LAST component before the `E`.  Only a symbol carrying a pointer
    parameter is of interest; the CONCEPT is dropped, so the name over-freezes
    rather than under-freezes, which is the safe direction for a proposal.
    """
    if not lit.startswith('_Z') or 'P' not in lit:
        return None
    i = 2
    comps = []
    nested = lit[i:i + 1] == 'N'
    if nested:
        i += 1
    while i < len(lit):
        m = re.match(r'(\d+)', lit[i:])
        if not m:
            break
        n = int(m.group(1))
        i += len(m.group(1))
        comps.append(lit[i:i + n])
        i += n
        if not nested:
            break
    if nested and lit[i:i + 1] != 'E':
        return None
    return comps[-1] if comps else None


FROZEN = frozen_names()


def pointee(ty):
    """`pointer[X]` -> X, else None."""
    m = re.match(r'pointer\[(.*)\]$', ty.strip())
    return m.group(1).strip() if m else None


def ref_pointee(ty):
    """`ref[X]` / `ref[X]?` -> X, else None."""
    m = re.match(r'ref\[(.*)\]\??$', ty.strip())
    return m.group(1).strip() if m else None


def compatible(ptr_inner, ref_inner):
    """Does a `pointer[ptr_inner]` argument feed a `ref[ref_inner]` parameter?

    Two shapes, and only two.  A scalar or concept cell: the pointees are the
    same name.  A pointer-to-pointer cell: `pointer[pointer[X]]` feeds
    `ref[ref[X]?]`, the spelling the refcell campaign settled on.
    """
    if ptr_inner == ref_inner:
        return True
    inner = pointee(ptr_inner)
    if inner is None:
        return False
    ri = ref_pointee(ref_inner)
    return ri is not None and ri == inner


def code_only(body):
    """The body with every comment cut away.

    The oldest instrument rule: a hazard scan reads CODE, not prose.
    The first draft of this tool skipped it and reported `normalize_append`
    twice, because a COMMENT four lines below the body explains why
    `Page.get(buf)` is not a real page -- and a `get(buf)` in prose reads
    exactly like a call forwarding `buf` into a `ref[u32]` position.
    """
    return '\n'.join(refout.strip_comment(l) for l in body.split('\n'))


def analyse(routines):
    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines):
        by_name[r['fn']].append(ri)

    # every pointer parameter is a candidate; the pointee is judged later
    cand = {}
    for ri, r in enumerate(routines):
        if r['fn'] in FROZEN:
            continue
        for pi, (pn, pt) in enumerate(r['params']):
            inner = pointee(pt)
            if inner is not None and inner not in ('void', 'const_char'):
                cand[(ri, pi)] = inner

    # a body that walks the name proves a buffer, whatever it forwards to
    verdict = {}
    for key, inner in cand.items():
        ri, pi = key
        nm = routines[ri]['params'][pi][0]
        verdict[key] = 'BUFFER' if any(rx.search(code_only(routines[ri]['body']))
                                       for rx in ARITH(nm)) else 'UNSEEN'

    # declared types, so a converted callee counts as evidence from the start
    def param_type(ri, pk):
        ps = routines[ri]['params']
        return ps[pk][1] if pk < len(ps) else ''

    evidence = {}
    changed = True
    while changed:
        changed = False
        for key, inner in cand.items():
            if verdict[key] != 'UNSEEN':
                continue
            ri, pi = key
            nm = routines[ri]['params'][pi][0]
            body = code_only(routines[ri]['body'])
            bare = re.compile(r'^\s*&?%s\s*$' % re.escape(nm))
            for m in CALL.finditer(body):
                for cri in by_name.get(m.group(1), ()):
                    args = split_args(body, m.end() - 1)
                    pos = [k for k, a in enumerate(args) if bare.match(a)]
                    if not pos:
                        continue
                    params = routines[cri]['params']
                    offs = [0]
                    if params and params[0][0] in PEELED:
                        offs.append(1)
                    for off in offs:
                        for k in pos:
                            pk = k + off
                            ty = param_type(cri, pk)
                            ci = ref_pointee(ty)
                            if ci is None:
                                # a callee we have already judged a cell counts too
                                if verdict.get((cri, pk)) == 'CELL' and \
                                        cand.get((cri, pk)) == inner:
                                    verdict[key] = 'CELL'
                                    evidence[key] = (routines[cri]['fn'], pk, ty or '?')
                                    changed = True
                                continue
                            if compatible(inner, ci):
                                verdict[key] = 'CELL'
                                evidence[key] = (routines[cri]['fn'], pk, ty)
                                changed = True
    return cand, verdict, evidence


if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    routines = collect(args or ['packages'])
    cand, verdict, evidence = analyse(routines)
    per = collections.Counter()
    for key in sorted(cand):
        if verdict[key] != 'CELL':
            continue
        ri, pi = key
        r = routines[ri]
        fn, pk, ty = evidence[key]
        print(f"CONVERT  {r['file']}:{r['line']} {r['fn']}({r['params'][pi][0]}: "
              f"pointer[{cand[key]}])  ->  {fn} arg {pk} is {ty}")
        per[r['file'].split('/')[1]] += 1
    print('---')
    print("frozen by an Itanium literal, skipped: " + ", ".join(sorted(FROZEN)))
    for k, v in per.most_common():
        print(f"{v:5}  {k}")
    print(f"{sum(per.values()):5}  TOTAL")
