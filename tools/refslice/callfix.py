#!/usr/bin/env python3
"""Wrap `(buffer, length)` argument pairs at the call sites of routines whose
buffer parameter has become a `Slice[T]`.

The conversion itself (convert.py) deliberately leaves call sites alone so the
COMPILER names them. This turns that error list into edits.

★The buffer's ARGUMENT index is derived from the declaration, never tabulated:
it is the Slice parameter's position minus one when the routine takes `this`
(a receiver is written at the declaration and not at the call).

★A DECLARATION line is never rewritten. Some arity errors are reported at the
callee's own line, and rewriting one produced
`function characters( Slice[u32](this: ..., s: Slice[u32]))`.

Usage: callfix.py <site-list> [package-dir ...]
       where <site-list> holds `file:line` per line, from the compiler.
"""
import re, os, sys, collections

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refout'))
from scan import collect, PEELED

DECL = re.compile(r'\s*(function|procedure|init)\b')
ONLY = set()

def split_args(text):
    out, depth, buf = [], 0, ''
    for ch in text:
        if ch in '([': depth += 1
        elif ch in ')]': depth -= 1
        if ch == ',' and depth == 0:
            out.append(buf); buf = ''; continue
        buf += ch
    out.append(buf); return out

def slice_targets(paths):
    """routine name -> (expected argument count, [(arg index, element type)]).

    ★The ARITY is part of the target: without it a call that is ALREADY correct
    gets wrapped again -- `text_matches(slice, "const")` has two arguments at
    the buffer position and reads exactly like an unconverted
    `text_matches(text, text_len, "const")`. Only a call with MORE arguments
    than the callee declares is a site.
    """
    # ★★A NAME-KEYED index answers about the WRONG NAME. Two routines can share
    # a name and only one be converted -- `SymbolDump.append_escaped` took a
    # Slice while `TypeDump.append_escaped` did not, and the wrap landed on
    # both. The arity guard catches most of it; where two same-named routines
    # differ, a name is AMBIGUOUS and is dropped rather than guessed at.
    seen = collections.Counter(r['fn'] for r in collect(paths))
    t = {}
    for r in collect(paths):
        # ★`--only` lifts the ambiguity guard for a name the READER has
        # decided, and nothing else does. opensp declares `put` twice and
        # `put_char` three times; the converted ones are MessageArg.put and
        # ParserState.put_char, and their same-named partners in Esis/Rast take
        # a different arity, so the arity guard below still refuses a wrong
        # wrap. Without the flag the tool is right to drop the name -- with it,
        # the evidence is the caller's, not the tool's.
        if r['fn'] not in ONLY and seen[r['fn']] > 1: continue
        params = r['params']
        shift = 1 if params and params[0][0] in PEELED else 0
        slots = []
        for pi, (pn, pt) in enumerate(params):
            m = re.match(r'Slice\[\s*(\w+)\s*\]$', pt.strip())
            if m:
                slots.append((pi - shift, m.group(1)))
        if slots:
            t[r['fn']] = (len(params) - shift, slots)
    return t

def fix_line(line, targets):
    if DECL.match(line): return line
    changed = True
    while changed:
        changed = False
        for name, (arity, slots) in targets.items():
            for m in re.finditer(rf'(?<![\w]){re.escape(name)}\(', line):
                start = m.end() - 1
                depth, end = 0, None
                for k in range(start, len(line)):
                    if line[k] == '(': depth += 1
                    elif line[k] == ')':
                        depth -= 1
                        if depth == 0: end = k; break
                if end is None: continue
                args = split_args(line[start+1:end])
                if len(args) <= arity: continue          # already correct
                for bi, elem in sorted(slots):
                    if len(args) < bi + 2: continue
                    a, b = args[bi].strip(), args[bi+1].strip()
                    if not a or a.startswith('Slice[') or not b: continue
                    # ★★★A CHAIN converts, not a routine: once the CALLER's own
                    # buffer parameter is a Slice too, its body reads
                    # `f(out, out.length, …)` and the length is not a thing to
                    # wrap, it is a surplus argument to DROP. The pair must be
                    # exactly (name, name.length) -- anything looser is the
                    # LOOSE verdict's mistake, a length that measures something
                    # else. Found converting opensp's Message.put family, where
                    # five forwarders sit between `format` and `put`.
                    if b == a + '.length':
                        args = args[:bi+1] + args[bi+2:]
                        line = line[:start+1] + ', '.join(x.strip() for x in args) + line[end:]
                        changed = True; break
                    if b.endswith('.length'): continue
                    # ★★★The LENGTH comes first: `Slice[T]` is {length, data}
                    # since the 2026-08-30 flip, and the construction runs the
                    # POSITIONAL tuple path (Slice declares only `init ()`), so
                    # the order here IS the field order. This line read
                    # `({a}, {b})` before the flip and would now put the
                    # pointer in the length slot -- caught by
                    # report_component_type_mismatch#, but a tool must not emit
                    # a form the compiler rejects.
                    args = args[:bi] + [f'Slice[{elem}]({b}, {a})'] + args[bi+2:]
                    line = line[:start+1] + ', '.join(x.strip() for x in args) + line[end:]
                    changed = True; break
                if changed: break
            if changed: break
    return line

def main(sitefile, paths):
    targets = slice_targets(paths)
    sites = collections.defaultdict(set)
    for l in open(sitefile):
        l = l.strip()
        if not l: continue
        f, n = l.rsplit(':', 1)
        sites[f].add(int(n))
    total = 0
    for f, lns in sites.items():
        if not os.path.exists(f): continue
        lines = open(f).read().split('\n')
        for n in sorted(lns):
            if n - 1 < len(lines):
                new = fix_line(lines[n-1], targets)
                if new != lines[n-1]:
                    lines[n-1] = new; total += 1
        open(f, 'w').write('\n'.join(lines))
    print("call sites rewritten:", total)

if __name__ == '__main__':
    argv = sys.argv[1:]
    if argv and argv[0].startswith('--only='):
        ONLY = set(argv[0].split('=', 1)[1].split(','))
        argv = argv[1:]
    main(argv[0], argv[1:] or ['packages'])
