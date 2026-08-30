#!/usr/bin/env python3
"""Drop the `as pointer[void]` from a `Page.get(...)` argument.

`Page.get(address: pointer[void])`'s PARAMETER is frozen and must stay so: the
Emitter and the Planner look the routine up by its Itanium literal
(`"_Z3getPv"`, `"_ZN4Page3getEPv"`, `"_ZN4scaly6memory4Page3getEPv"`) rather
than calling it, so re-signing it is an undefined symbol at link time in every
program that takes a caller page.

The CALL SITE is not frozen, and the cast there buys nothing.  Measured on all
three argument shapes -- `ref[T]`, `pointer[T]` and the NPO `ref[T]?` -- cast
against bare: same page answered, and the emitted body byte-identical but for
the module id.  Seven uncast call sites already live in the tree (Escape,
FrameMark x2, cluster, fiber x2, checker), in three packages; this only rolls
the form out to the other 138.

★ The failure mode is LOUD, which is why this is a sweep at all: an operand
that is not pointer-shaped -- a `size_t` local, integer arithmetic -- does not
resolve without the cast and is a hard rc-4, never a silent wrong answer.
Even so the tool refuses anything but a REFERENCE-SHAPED operand: a name, a
member chain, or a call chain.  Arithmetic keeps its cast; a cast is where an
integer legitimately becomes a pointer.

★ A match inside a STRING is skipped -- the compiler's own sources carry
`Page.get` in diagnostics -- and so is a comment, and so is an argument whose
parentheses do not close on the line (none exist today; the tool says so
rather than guessing).

Usage: tools/pageget.py [--apply] [root ...]      (default: dry run)
"""
import re, sys, os, glob, collections

def spans(line):
    """(code_end, string_spans) -- the comment cut, and where the strings are."""
    out = []; i = 0; q = None; start = 0
    while i < len(line):
        c = line[i]
        if q:
            if c == '\\': i += 2; continue
            if c == q: out.append((start, i + 1)); q = None
            i += 1; continue
        if c in '"\'': q = c; start = i; i += 1; continue
        if c == ';': return i, out
        i += 1
    return len(line), out

def in_string(pos, ss):
    return any(a <= pos < b for a, b in ss)

# a name, a member chain, or a call chain -- and nothing else
OPERAND = re.compile(r'^&?[A-Za-z_]\w*(\s*\.\s*[A-Za-z_]\w*)*(\s*\(.*\))?$')

def sweepable(src):
    src = src.strip()
    if not OPERAND.match(src):
        return False
    # no top-level operator hiding inside an accepted call chain
    depth = 0; flat = []
    for ch in src:
        if ch in '([': depth += 1
        elif ch in ')]': depth -= 1
        elif depth == 0: flat.append(ch)
    flat = ''.join(flat)
    return not re.search(r'[-+*/%<>=|~]|\bas\b', flat)

def sweep(path, apply):
    lines = open(path, encoding='utf-8').read().split('\n')
    hits = []; unbalanced = 0
    for n, line in enumerate(lines):
        if 'Page.get(' not in line: continue
        end, ss = spans(line)
        new = line; shift = 0
        for m in list(re.finditer(r'Page\.get\(', line[:end])):
            if in_string(m.start(), ss): continue
            j = m.end(); d = 1; k = j
            while k < end and d:
                if line[k] == '(': d += 1
                elif line[k] == ')': d -= 1
                k += 1
            if d: unbalanced += 1; continue
            arg = line[j:k - 1]
            mm = re.match(r'^(.*?)\s+as\s+pointer\[void\]\s*$', arg)
            if not mm: continue
            src = mm.group(1)
            if not sweepable(src): continue
            a, b = j + shift, k - 1 + shift
            new = new[:a] + src.strip() + new[b:]
            shift += len(src.strip()) - (b - a)
            hits.append((n + 1, arg.strip(), src.strip()))
        if hits and new != line: lines[n] = new
    if apply and hits:
        open(path, 'w', encoding='utf-8').write('\n'.join(lines))
    return hits, unbalanced

roots = [a for a in sys.argv[1:] if not a.startswith('-')] or ['packages', 'tests', 'demo']
apply = '--apply' in sys.argv
per = collections.Counter(); total = 0; unbal = 0
for root in roots:
    for f in sorted(glob.glob(root + '/**/*.scaly', recursive=True)):
        hits, u = sweep(f, apply)
        unbal += u
        if not hits: continue
        pkg = f.split('/')[1] if f.startswith('packages/') else root
        per[pkg] += len(hits); total += len(hits)
        if '-v' in sys.argv:
            for n, a, s in hits: print(f"{f}:{n}  {a}  ->  {s}")
print(f"{'APPLIED' if apply else 'would strip'} {total} casts", dict(per))
if unbal: print(f"!! {unbal} Page.get( arguments do not close on their line -- read them by hand")
