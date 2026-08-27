#!/usr/bin/env python3
"""Find `pointer[Page]` PARAMETERS that the Modeler does not peel.

The caller-page peel (Modeler.scaly:1868 for function/procedure, :2153 for init)
fires only when the FIRST parameter is named exactly `page` or `rp` AND is either
typeless or typed exactly `pointer[Page]`.  Every other `pointer[Page]` parameter
is an ordinary user parameter and converts to `ref[Page]` freely -- a pointer
argument passes to a ref parameter without a cast (measured), so no call site moves.
"""
import re, os, sys, collections

DECL = re.compile(r'^(\s*)(function|procedure|init)\b')
PTRPAGE = re.compile(r'pointer\[\s*(?:scaly\.memory\.)?Page\s*\]')

def strip_comment(line):
    return line.split(';')[0]

def signature(lines, i):
    """Return (text, last_line_index) for the declaration starting at line i,
    following continuation lines until parens balance."""
    txt = strip_comment(lines[i]); depth = txt.count('(') - txt.count(')')
    j = i
    while depth > 0 and j + 1 < len(lines):
        j += 1
        nxt = strip_comment(lines[j])
        txt += ' ' + nxt
        depth += nxt.count('(') - nxt.count(')')
    return txt, j

def split_params(sig):
    """Split the parameter list of a signature at top-level commas."""
    o = sig.find('(')
    if o < 0: return []
    depth = 0; buf = ''; out = []
    for ch in sig[o:]:
        if ch in '([':
            depth += 1
            if depth == 1: continue          # drop the opening paren itself
        elif ch in ')]':
            depth -= 1
            if depth == 0:
                out.append(buf)
                return [q.strip() for q in out if q.strip()]
        if depth == 1 and ch == ',':
            out.append(buf); buf = ''; continue
        buf += ch
    return [q.strip() for q in out if q.strip()]

def scan(root='packages'):
    hits = []
    for dp, dn, fn in os.walk(root):
        for f in sorted(fn):
            if not f.endswith('.scaly'): continue
            p = os.path.join(dp, f)
            lines = open(p, encoding='utf8', errors='replace').read().split('\n')
            i = 0
            while i < len(lines):
                if not DECL.match(strip_comment(lines[i])): i += 1; continue
                sig, last = signature(lines, i)
                if re.search(r'\bextern\s*$', sig.strip()):
                    i = last + 1; continue
                if PTRPAGE.search(sig):
                    params = split_params(sig)
                    for idx, prm in enumerate(params):
                        if not PTRPAGE.search(prm): continue
                        nm = prm.split(':')[0].strip()
                        peeled = (idx == 0 and nm in ('page', 'rp'))
                        hits.append((p, i + 1, last + 1, idx, nm, peeled))
                i = last + 1
    return hits

if __name__ == '__main__':
    hits = scan(sys.argv[1] if len(sys.argv) > 1 else 'packages')
    conv = collections.Counter(); peel = collections.Counter(); names = collections.Counter()
    for p, a, b, idx, nm, pl in hits:
        pkg = p.split('/')[1]
        (peel if pl else conv)[pkg] += 1
        if not pl: names[nm] += 1
    print(f"{'package':12} {'konvertierbar':>14} {'gepeelt':>9}")
    for pkg in sorted(set(list(conv) + list(peel))):
        print(f"{pkg:12} {conv[pkg]:14d} {peel[pkg]:9d}")
    print(f"{'SUMME':12} {sum(conv.values()):14d} {sum(peel.values()):9d}")
    print("\nParameternamen der konvertierbaren:", dict(names.most_common(12)))
    ml = [h for h in hits if h[2] > h[1] and not h[5]]
    print(f"davon mehrzeilige Signaturen: {len(ml)}")
