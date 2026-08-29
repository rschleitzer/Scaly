#!/usr/bin/env python3
"""Rewrite ONE buffer parameter and its adjacent length into a `Slice[T]`.

Usage: convert.py <file> <routine> <buffer> <length> <elem>

Scoped to the routine's own body, because a bare length name like `n` occurs in
every second function in these files and a file-wide substitution answers about
the wrong one -- `refstar.py` made exactly that mistake from the other side.

★★★It rewrites the DECLARATION and the BODY only. Call sites are left to the
compiler, which rejects a pointer where a `Slice` is declared; that is the
intended driver. ★And it does NOT check what the length COUNTS -- see the
scanner's note on `make_translate`, where the length counts PAIRS and the slice
is twice as long as the parameter says.
"""
import re, sys

def routine_range(lines, name):
    start = None
    for i, l in enumerate(lines):
        if re.match(rf'\s*(function|procedure)\s+{re.escape(name)}\s*\(', l):
            start = i; break
    if start is None: return None
    indent = len(lines[start]) - len(lines[start].lstrip())
    j = start + 1
    depth = 0; seen = False
    while j < len(lines):
        s = lines[j].split(';')[0]
        depth += s.count('{') - s.count('}')
        if '{' in s: seen = True
        if seen and depth <= 0: return (start, j)
        if not seen and s.strip() and (len(s) - len(s.lstrip())) <= indent and j > start + 1:
            return (start, j - 1)
        j += 1
    return (start, len(lines) - 1)

def main(path, routine, buf, length, elem):
    lines = open(path).read().split('\n')
    rng = routine_range(lines, routine)
    if rng is None: sys.exit(f"routine not found: {routine}")
    a, b = rng
    # declaration: `buf: pointer[E], length: size_t` -> `buf: Slice[E]`
    for i in range(a, min(a + 8, len(lines))):
        new = re.sub(rf'{re.escape(buf)}: pointer\[{re.escape(elem)}\],\s*{re.escape(length)}: \w+',
                     f'{buf}: Slice[{elem}]', lines[i])
        if new != lines[i]:
            lines[i] = new; break
    else:
        sys.exit(f"declaration not matched in {routine}")
    # body
    for i in range(a, b + 1):
        s = lines[i]
        s = re.sub(rf'\*\({re.escape(buf)} \+ ([^()]*?)\)', rf'{buf}[\1]', s)
        if i > a:
            s = re.sub(rf'(?<![\w.]){re.escape(length)}(?![\w])', f'{buf}.length', s)
        lines[i] = s
    open(path, 'w').write('\n'.join(lines))
    print(f"{path}: {routine}({buf}) -> Slice[{elem}]")

if __name__ == '__main__':
    main(*sys.argv[1:6])
