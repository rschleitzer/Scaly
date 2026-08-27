#!/usr/bin/env python3
"""Sweep `*r` off a reference, driven by the COMPILER's own findings.

`report_deref_of_ref#` names every site (file:line:col), so there is no scoping
heuristic left to get wrong -- the planner has already decided that the operand
is a non-optional `ref[T]`.  Feed it the diagnostics and it rewrites the shapes
that have a transparent spelling:

    set *r: v        -> set r: v
    choose *r        -> choose r
    (*r).m()         -> r.m()            (r may be a dotted chain)
    return *r        -> return r
    f(..., *r, ...)  -> f(..., r, ...)

Anything else is printed and left alone -- an untyped `let q *r` COPIES where
`let q r` would ALIAS, and `(*r) as T` cannot lose its star at all, because a
reference cast to an integer is how this tree takes an ADDRESS.

Usage:  <compiler> -S --no-prelude -o /dev/null <root> 2>&1 | tools/refstar_gate.py
"""
import re, sys, collections

FIND = re.compile(r'^(.*?):(\d+):(\d+): error: dereferencing a reference')

def rewrite(code, col):
    """One rewrite on the code half of a line; None when no rule applies."""
    for pat, rep in (
        (r'^(\s*set\s+)\*(\w+)(\s*:)', r'\1\2\3'),
        (r'^(\s*choose\s+)\*([\w.]+)\b', r'\1\2'),
        (r'^(\s*return\s+)\*([\w.]+)\s*$', r'\1\2'),
    ):
        new, n = re.subn(pat, rep, code)
        if n: return new
    # a deref RECEIVER: `(*x).m()` -- the member chain already auto-derefs
    new, n = re.subn(r'\(\s*\*([\w.]+)\s*\)\s*\.', r'\1.', code)
    if n: return new
    # a bare `*x` as an argument or operand: drop the star, and the parens too
    # when they are the author's grouping rather than a call's argument list
    new, n = re.subn(r'(?<![\w)\]])\(\s*\*(\w+)\s*\)(?!\s*(?:as|\.))', r'\1', code)
    new, n2 = re.subn(r'(?<![\w\)])\*(\w+)\b(?!\s*(?:as\b|\.))', r'\1', new)
    if n or n2: return new
    return None

def main():
    sites = collections.defaultdict(set)
    for line in sys.stdin:
        m = FIND.match(line)
        if m: sites[m.group(1)].add((int(m.group(2)), int(m.group(3))))
    done = 0; left = []
    for path, hits in sorted(sites.items()):
        lines = open(path, encoding='utf8').read().split('\n')
        for ln, col in sorted(hits):
            i = ln - 1
            cut = len(lines[i].split(';')[0])
            head, tail = lines[i][:cut], lines[i][cut:]
            new = rewrite(head, col)
            if new is None or new == head:
                left.append(f'{path}:{ln}: {head.strip()[:110]}')
            else:
                lines[i] = new + tail; done += 1
        open(path, 'w', encoding='utf8').write('\n'.join(lines))
    for l in left: print('LEFT ' + l)
    print(f'{done} rewritten, {len(left)} left')

if __name__ == '__main__':
    main()
