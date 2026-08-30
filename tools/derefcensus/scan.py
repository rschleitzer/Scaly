#!/usr/bin/env python3
"""Census of the raw `*(name + i)` / `set *(name + i): v` sites that are LEFT
after the refslice campaign closed its mechanizable slice.

The refslice scan asks about PARAMETERS.  This one asks about the ACCESSES,
which is the shape the remaining work is actually in: a deref site whose
receiver is a parameter is what refslice already counted, but a receiver that
is a LOCAL, a FIELD or a CALL RESULT was never in any scan's population and
is where most of the residue turned out to live.

Verdict per site is the receiver's PROVENANCE inside its enclosing routine:

  PARAM     declared in the routine's parameter list -- refslice's population.
  LOCAL     `let`/`var` bound in the body.  A local from `allocate(...) as
            pointer[X]` is the hand-rolled buffer CLAUDE.md accepts as shape 3.
  FIELD     a member hop (`this.x`, `a.b`) or a bare field of the enclosing
            concept -- the receiver is storage the routine does not own.
  CALL      the receiver is a call result (`x.get_buffer()`, `args.get i`).
  GLOBAL    a module-level `mutable`/`shared`/`define`.
  UNKNOWN   nothing in the routine declares the name.
"""
import re, os, sys, collections

ROOT = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith('-') else 'packages'

# a routine header: function/procedure/operator/init, at any indent
HEAD = re.compile(r'^\s*(function|procedure|operator|init)\b([^\n]*)')
DEREF = re.compile(r'\*\(\s*([A-Za-z_][A-Za-z0-9_.]*)\s*(?:\(\s*\))?\s*[+\-]')

def strip_comment(line):
    out, i, instr = [], 0, None
    while i < len(line):
        c = line[i]
        if instr:
            if c == '\\': out.append(line[i:i+2]); i += 2; continue
            if c == instr: instr = None
            out.append(c); i += 1; continue
        if c in '"\'': instr = c; out.append(c); i += 1; continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

def files(root):
    for d, _, fs in os.walk(root):
        for f in fs:
            if f.endswith('.scaly'): yield os.path.join(d, f)

def pkg_of(p):
    parts = p.split(os.sep)
    return parts[parts.index('packages') + 1] if 'packages' in parts else '?'

def param_names(header):
    """Names declared in a routine header's parameter list, across its lines."""
    m = re.search(r'\(', header)
    if not m: return set()
    depth, i, buf = 0, m.start(), []
    while i < len(header):
        c = header[i]
        if c == '(': depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0: break
        buf.append(c); i += 1
    inner = ''.join(buf)[1:]
    names, depth, cur = set(), 0, []
    for c in inner + ',':
        if c in '([': depth += 1
        elif c in ')]': depth -= 1
        if c == ',' and depth == 0:
            t = ''.join(cur).strip()
            if t:
                n = t.split(':')[0].strip()
                if re.match(r'^[A-Za-z_]\w*$', n): names.add(n)
            cur = []
        else: cur.append(c)
    return names

BIND = re.compile(r'\b(?:let|var)\s+([A-Za-z_]\w*)\b')

def main():
    rows = []
    for path in sorted(files(ROOT)):
        src = [strip_comment(l) for l in open(path, encoding='utf-8', errors='replace')]
        # gather module-level globals
        globals_ = set()
        for l in src:
            m = re.match(r'\s*(?:mutable|shared)\s+([A-Za-z_]\w*)', l)
            if m: globals_.add(m.group(1))
            m = re.match(r'\s*define\s+([A-Za-z_]\w*)\s*:', l)
            if m: globals_.add(m.group(1))
        # routine spans: header line -> next header line at same-or-less indent
        heads = [i for i, l in enumerate(src) if HEAD.match(l)]
        heads.append(len(src))
        for hi in range(len(heads) - 1):
            a, b = heads[hi], heads[hi + 1]
            # header may span lines until the paren balances
            hdr, j, depth = '', a, 0
            while j < b:
                hdr += src[j]
                depth += src[j].count('(') - src[j].count(')')
                if '(' in hdr and depth <= 0: break
                j += 1
            params = param_names(hdr)
            body = src[a:b]
            locals_ = set()
            for l in body: locals_ |= set(BIND.findall(l))
            for off, line in enumerate(body):
                for m in DEREF.finditer(line):
                    recv = m.group(1)
                    head = recv.split('.')[0]
                    dotted = '.' in recv
                    if dotted and head in ('this',): v = 'FIELD'
                    elif dotted and head in params: v = 'PARAM'
                    elif dotted and head in locals_: v = 'LOCAL'
                    elif dotted: v = 'FIELD'
                    elif recv in params: v = 'PARAM'
                    elif recv in locals_: v = 'LOCAL'
                    elif recv in globals_: v = 'GLOBAL'
                    else: v = 'UNKNOWN'
                    if line[m.end(1):m.end(1)+3].strip().startswith('()'): v = 'CALL'
                    rows.append((v, pkg_of(path), path, a + off + 1, recv,
                                 line.strip()))
    return rows

if __name__ == '__main__':
    rows = main()
    verbose = '-v' in sys.argv
    only = [a for a in sys.argv[2:] if not a.startswith('-')]
    if verbose:
        for v, pkg, path, ln, recv, txt in rows:
            if only and v not in only: continue
            print(f'{v:8} {pkg:9} {path}:{ln}  {recv}   {txt[:90]}')
    c = collections.Counter((v, pkg) for v, pkg, *_ in rows)
    for (v, pkg), n in sorted(c.items(), key=lambda kv: -kv[1]):
        print(f'{n:5}  {pkg:9} {v}')
    print(f'--- {len(rows)} deref sites')
    cv = collections.Counter(v for v, *_ in rows)
    print('   ' + '  '.join(f'{k}={n}' for k, n in cv.most_common()))
