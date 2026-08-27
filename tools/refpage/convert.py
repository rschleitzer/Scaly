#!/usr/bin/env python3
"""Convert non-peeled `pointer[Page]` PARAMETERS to `ref[Page]`.

Only the parameter LIST of a function/procedure/init declaration is rewritten, and
only for parameters the Modeler does not peel (see scan.py).  A return type, a
field, a local and a cast are left alone -- they are separate slices.

Both argument directions were measured to need no cast (pointer -> ref and
ref -> pointer), so no call site moves and a half-converted tree still compiles.
"""
import re, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scan import DECL, PTRPAGE, strip_comment, signature, split_params

def convert_file(path, dry=False):
    src = open(path, encoding='utf8').read()
    lines = src.split('\n')
    changed = 0
    i = 0
    while i < len(lines):
        if not DECL.match(strip_comment(lines[i])):
            i += 1; continue
        sig, last = signature(lines, i)
        if re.search(r'\bextern\s*$', sig.strip()) or not PTRPAGE.search(sig):
            i = last + 1; continue
        params = split_params(sig)
        # Belt and braces: never touch a declaration whose FIRST parameter is the
        # peeled caller page (`page`/`rp` at index 0) -- rewrite nothing at all there.
        if params and params[0].split(':')[0].strip() in ('page', 'rp'):
            i = last + 1; continue
        # Rewrite inside the parameter list only: for each line of the
        # declaration, recompute the paren depth at its start and replace every
        # `pointer[Page]` that sits at depth >= 1 (i.e. inside the parameter list).
        for k in range(i, last + 1):
            line = lines[k]
            code_end = len(line.split(';')[0])
            code, tail = line[:code_end], line[code_end:]
            d = _depth_before(lines, i, k)
            res = ''; j = 0
            while j < len(code):
                m = PTRPAGE.match(code, j)
                if m and d >= 1:
                    res += 'ref[Page]'; j = m.end(); changed += 1; continue
                ch = code[j]
                if ch in '([': d += 1
                elif ch in ')]': d -= 1
                res += ch; j += 1
            lines[k] = res + tail
        i = last + 1
    new = '\n'.join(lines)
    if new != src and not dry:
        open(path, 'w', encoding='utf8').write(new)
    return changed

def _depth_before(lines, first, k):
    d = 0
    for t in range(first, k):
        c = strip_comment(lines[t])
        d += c.count('(') + c.count('[') - c.count(')') - c.count(']')
    return d

if __name__ == '__main__':
    root = sys.argv[1]
    dry = '--dry' in sys.argv
    total = 0; files = 0
    for dp, dn, fn in os.walk(root):
        for f in sorted(fn):
            if not f.endswith('.scaly'): continue
            n = convert_file(os.path.join(dp, f), dry)
            if n: total += n; files += 1
    print(f"{'(dry) ' if dry else ''}{total} Parameter in {files} Dateien: pointer[Page] -> ref[Page]")
