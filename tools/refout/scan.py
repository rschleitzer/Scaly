#!/usr/bin/env python3
"""Find scalar OUT-CELL parameters -- `pointer[int]` and friends naming a single
cell the callee writes through, not a buffer it walks.

The two are spelled identically today: `pointer[u32]` names both a StringC
character buffer and a one-cell codepoint output, and neither a reader nor a
checker can tell them apart.  A `ref[T]` parameter may be deref-stored
(`set *out: v`) and a pointer argument passes to a `ref` parameter and back
without a cast -- both measured -- so an out-cell converts and no call site
moves.  After the split, walking one is a hard rc-4 from the ref-arithmetic
gate instead of a silent read past the cell.

A buffer proves itself by ARITHMETIC or INDEXING on the name.  A FORWARDER
proves nothing on its own -- `get_identifier_token(buf)` only ever writes
`*buf`, and every walk of it happens one call down in `kw_eq` -- so the sinks
seed a fixpoint and each forwarder inherits its callee's verdict.

The tool reports; the reader decides.  Nothing here proves an UNSEEN parameter
is a cell.
"""
import re, os, sys, collections

SCALARS = ('int','bool','u32','size_t','u64','i64','u16','i32','i16','u8','char','double','float','i8')
PEELED = ('this', 'rp', 'page')

DECL = re.compile(r'^(\s*)(function|procedure|init)\b')

def strip_comment(line):
    return line.split(';')[0]

def signature(lines, i):
    txt = strip_comment(lines[i]); depth = txt.count('(') - txt.count(')')
    j = i
    while depth > 0 and j + 1 < len(lines):
        j += 1
        nxt = strip_comment(lines[j])
        txt += ' ' + nxt
        depth += nxt.count('(') - nxt.count(')')
    return txt, j

def split_args(text, open_at):
    """Split a parenthesised list starting at text[open_at] == '('."""
    depth = 0; buf = ''; out = []
    for ch in text[open_at:]:
        if ch in '([':
            depth += 1
            if depth == 1: continue
        elif ch in ')]':
            depth -= 1
            if depth == 0:
                out.append(buf); return out
        if depth == 1 and ch == ',':
            out.append(buf); buf = ''; continue
        buf += ch
    out.append(buf)
    return out

def split_params(sig):
    o = sig.find('(')
    return split_args(sig, o) if o >= 0 else []

def body_lines(lines, sig_end, indent):
    out = []; j = sig_end + 1
    while j < len(lines):
        s = strip_comment(lines[j])
        if s.strip():
            ind = len(s) - len(s.lstrip())
            if ind <= len(indent) and (DECL.match(s) or not s.lstrip().startswith(('{', '}'))):
                if out: break
        out.append(lines[j]); j += 1
    return out

def collect(paths):
    """Every routine: name, declared parameters, body text."""
    routines = []
    for path in paths:
        for root, dirs, files in os.walk(path):
            dirs[:] = [d for d in dirs if d not in ('tests', 'out')]
            for f in sorted(files):
                if not f.endswith('.scaly'): continue
                p = os.path.join(root, f)
                lines = open(p, encoding='utf-8', errors='replace').read().split('\n')
                for i, line in enumerate(lines):
                    m = DECL.match(strip_comment(line))
                    if not m: continue
                    sig, end = signature(lines, i)
                    if re.search(r'\bextern\b', sig): continue
                    nm = re.match(r'\s*\w+\s+(\w+)', sig)
                    params = []
                    for prm in split_params(sig):
                        pm = re.match(r'\s*(\w+)\s*:\s*(.+?)\s*$', prm)
                        if pm: params.append((pm.group(1), pm.group(2)))
                        else: params.append((prm.strip(), ''))
                    routines.append(dict(file=p, line=i+1, fn=nm.group(1) if nm else '?',
                                         params=params, kind=m.group(2),
                                         body='\n'.join(body_lines(lines, end, m.group(1)))))
    return routines

ARITH = lambda nm: [
    re.compile(r'\b%s\s*\[' % re.escape(nm)),
    re.compile(r'\b%s\s*[-+]\s' % re.escape(nm)),
    re.compile(r'[-+]\s*%s\b' % re.escape(nm)),
]
STORE = lambda nm: re.compile(r'\bset\s+\*%s\s*:' % re.escape(nm))
DEREF = lambda nm: re.compile(r'\*%s\b' % re.escape(nm))

def analyse(routines):
    """Verdict per (routine index, param index) for scalar-pointer parameters."""
    cand = {}
    for ri, r in enumerate(routines):
        for pi, (pn, pt) in enumerate(r['params']):
            m = re.match(r'pointer\[\s*(\w+)\s*\]$', pt)
            if m and m.group(1) in SCALARS:
                cand[(ri, pi)] = m.group(1)

    # sinks: a body that does arithmetic on the name proves a buffer
    verdict = {}
    for key, ty in cand.items():
        ri, pi = key
        nm = routines[ri]['params'][pi][0]
        body = routines[ri]['body']
        if any(rx.search(body) for rx in ARITH(nm)):
            verdict[key] = 'BUFFER'
        elif STORE(nm).search(body) or DEREF(nm).search(body):
            verdict[key] = 'CELL'
        else:
            verdict[key] = 'UNSEEN'

    # index declarations by name, so a call can be matched to its callee
    by_name = collections.defaultdict(list)
    for ri, r in enumerate(routines):
        by_name[r['fn']].append(ri)

    # forwarding fixpoint: a parameter handed to a BUFFER position is a buffer
    changed = True
    while changed:
        changed = False
        for key, ty in cand.items():
            if verdict[key] == 'BUFFER': continue
            ri, pi = key
            nm = routines[ri]['params'][pi][0]
            if forwarded_to_buffer(routines, by_name, cand, verdict, ri, nm):
                verdict[key] = 'BUFFER'; changed = True
    return cand, verdict

CALL = re.compile(r'(\w+)\s*\(')

def forwarded_to_buffer(routines, by_name, cand, verdict, ri, nm):
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
            # align: a dotted or method call omits the leading peeled parameter
            offs = [0]
            if params and params[0][0] in PEELED: offs.append(1)
            for off in offs:
                for k in pos:
                    pk = k + off
                    if (cri, pk) in cand and verdict[(cri, pk)] == 'BUFFER':
                        return True
                    if pk < len(params) and 'pointer[' in params[pk][1] and (cri, pk) not in cand:
                        # a pointer position we do not judge (pointer[void], ptr-ptr)
                        pass
            # counts disagree entirely: taint if the callee holds any buffer at all
            if len(args) not in (len(params), len(params) - 1):
                if any(verdict.get((cri, k)) == 'BUFFER' for k in range(len(params))):
                    return True
    return False

if __name__ == '__main__':
    routines = collect(sys.argv[1:] or ['packages'])
    cand, verdict = analyse(routines)
    per = collections.Counter(); byty = collections.Counter()
    for key, ty in sorted(cand.items()):
        ri, pi = key; r = routines[ri]
        v = verdict[key]
        print(f"{v:7} {ty:8} {r['file']}:{r['line']} {r['fn']}({r['params'][pi][0]})")
        per[(r['file'].split('/')[1], v)] += 1; byty[(v, ty)] += 1
    print('---')
    for k, v in sorted(per.items()): print(f'{v:5}  {k[0]:8} {k[1]}')
    print('---')
    for k, v in sorted(byty.items()): print(f'{v:5}  {k[0]:7} pointer[{k[1]}]')
