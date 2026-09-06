#!/usr/bin/env python3
"""Second pass of the ASCII-literal campaign: the uses of a converted `Slice[char]`
parameter that textslice.py does not know (it rewrites derefs and checks call sites).

For every parameter in the spec, inside its routine's body:
  strlen(p) / strlen p            -> p.length
  String(p) / String^h(p)         -> String(p.data, p.length)      (the (cstr, length) init)
  x.append(p) / x.append p        -> x.append(p.data, p.length)    (StringBuilder's (start, length) append)
  scaly_eputs(p) / scaly_eputs p  -> write(2, p.data as pointer[void], p.length)
and it prints a HAND line for every use that has no mechanical spelling: `p as pointer[..]`
(a NUL-walk to rewrite as a length-bounded loop), `p = null` / `p <> null` (a Slice cannot be
null-tested), `print(p)` / `println(p)` (the prelude's console takes a C string).

Usage: post.py <spec.py> [--apply]
"""
import re, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'textslice'))
import textslice as T

def rewrite(code, p):
    P = re.escape(p)
    n0 = code
    code = re.sub(rf'\bstrlen\s*\(\s*{P}\s*\)', f'{p}.length', code)
    code = re.sub(rf'\bstrlen\s+{P}(?![\w.\[])', f'{p}.length', code)
    code = re.sub(rf'(String(?:\^\w+)?)\(\s*{P}\s*\)', rf'\1({p}.data, {p}.length)', code)
    code = re.sub(rf'\.append\(\s*{P}\s*\)', f'.append({p}.data, {p}.length)', code)
    code = re.sub(rf'\.append\s+{P}\s*$', f'.append({p}.data, {p}.length)', code)
    code = re.sub(rf'\bscaly_eputs\s*\(\s*{P}\s*\)', f'write(2, {p}.data as pointer[void], {p}.length)', code)
    code = re.sub(rf'\bscaly_eputs\s+{P}\s*$', f'write(2, {p}.data as pointer[void], {p}.length)', code)
    return code, code != n0

def hand_hits(code, p):
    P = re.escape(p); h = []
    if re.search(rf'(?<![\w.]){P}\s+as\s+pointer', code): h.append('as-pointer')
    if re.search(rf'(?<![\w.]){P}\s*(=|<>)\s*null', code) or re.search(rf'null\s*(=|<>)\s*{P}(?![\w.])', code): h.append('null-test')
    if re.search(rf'\b(print|println)\s*\(?\s*{P}(?![\w.])', code): h.append('console')
    if re.search(rf'\*\(?\s*{P}(?![\w.])', code): h.append('deref')
    return h

def main():
    apply = '--apply' in sys.argv
    spec = T.load_spec([a for a in sys.argv[1:] if not a.startswith('--')][0])
    files = {}
    stats = {'rewritten': 0, 'hand': 0}
    for (fpath, fn, line, p, _ln) in spec.params:
        if fpath not in files: files[fpath] = open(fpath, encoding='utf-8').read().split('\n')
        lines = files[fpath]
        pat = r'\s*init\s*\(' if fn == 'init' else rf'\s*(function|procedure)\s+{re.escape(fn)}\s*\('
        cands = [i for i in range(max(0, line - 60), min(len(lines), line + 10)) if re.match(pat, T.code_only(lines[i]))]
        if not cands: print(f'WARN routine not found {fpath}:{line} {fn}'); continue
        i = min(cands, key=lambda c: abs(c - (line - 1)))
        sig, je, bs, be = T.routine_at(lines, i)
        if not re.search(rf'{re.escape(p)}\s*:\s*Slice\[char\]', T.code_only(sig)):
            print(f'WARN {fpath}:{i+1} {fn}({p}) is not a Slice[char] parameter yet -- run textslice --apply first'); continue
        for k in range(bs, be + 1):
            code, cmt = T.split_code_comment(lines[k])
            new, changed = rewrite(code, p)
            if changed:
                stats['rewritten'] += 1
                print(f'{fpath}:{k+1}: {code.strip()}  ->  {new.strip()}')
                lines[k] = new + cmt
            for h in hand_hits(T.code_only(lines[k]), p):
                stats['hand'] += 1
                print(f'HAND {fpath}:{k+1}: {h} on {p} in {fn}: {lines[k].strip()}')
    if apply:
        for fpath, lines in files.items(): open(fpath, 'w', encoding='utf-8').write('\n'.join(lines))
    print('---', stats, 'APPLIED' if apply else 'dry run')

if __name__ == '__main__': main()
