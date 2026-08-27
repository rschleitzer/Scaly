#!/usr/bin/env python3
"""Rewrite the scalar OUT-CELL parameters scan.py reports as CELL to `ref[T]`.

Only the parameter LIST of a declaration is touched, and only a parameter whose
body writes through it and which no forwarding path proves to be a buffer.  A
BUFFER or UNSEEN verdict is never rewritten -- an UNSEEN parameter is one the
tool has no evidence about, which is not the same as evidence that it is a cell.

Both argument directions pass without a cast (measured), so no call site moves
and a half-converted tree compiles.
"""
import re, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scan import collect, analyse, DECL, strip_comment, signature

def main(paths, dry=False, only=None):
    routines = collect(paths)
    cand, verdict = analyse(routines)
    todo = {}
    for key, ty in cand.items():
        if verdict[key] != 'CELL': continue
        ri, pi = key; r = routines[ri]
        if only and only not in r['file']: continue
        todo.setdefault(r['file'], []).append((r['line'], r['params'][pi][0], ty))

    total = 0
    for path, items in sorted(todo.items()):
        lines = open(path, encoding='utf8').read().split('\n')
        n = 0
        for line, nm, ty in items:
            i = line - 1
            sig, last = signature(lines, i)
            pat = re.compile(r'(\b%s\s*:\s*)pointer(\[\s*%s\s*\])' % (re.escape(nm), re.escape(ty)))
            hit = False
            for k in range(i, last + 1):
                code_end = len(lines[k].split(';')[0])
                code, tail = lines[k][:code_end], lines[k][code_end:]
                new, c = pat.subn(r'\1ref\2', code)
                if c:
                    lines[k] = new + tail; hit = True; n += c
            if not hit:
                print(f'  MISS {path}:{line} {nm}', file=sys.stderr)
        if n and not dry:
            open(path, 'w', encoding='utf8').write('\n'.join(lines))
        print(f'{n:4}  {path}')
        total += n
    print(f'{total} parameters converted' + (' (dry run)' if dry else ''))

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry' in sys.argv
    only = next((a.split('=')[1] for a in sys.argv[1:] if a.startswith('--only=')), None)
    main(args or ['packages'], dry, only)
