#!/usr/bin/env python3
"""Emit every root; whenever the compiler rejects a converted line, put THAT
line's original text back and try again.

★★★ `fieldwise.py` cannot see TYPES, so it cannot know whether a value the
field-wise form accepted is one the POSITIONAL-TUPLE path accepts.  The two
differ by exactly one rule and it is deliberate: a `set` TARGET lets a `ref[T]`
read as its pointee (`deref_ref_into_pointee#`), a tuple COMPONENT does not --
`report_component_type_mismatch#`'s acceptor is `type_conformance_accepted#`
MINUS its class 8, the one that strips a level of indirection.  So

    set p.chars: GroveNode.value_token(gpage, av, k)     ; ref[StringC] -> StringC, fine
    &GroveNode^gpage(..., GroveNode.value_token(...))    ; rc 4, and rightly so

★ The COMPILER is therefore the arbiter, and this driver is how that verdict
gets applied without a reader guessing in advance.  Measured 2026-09-01 over
177 converted sites: it held back exactly ONE (dazzle's `Grove.scaly`
`attribute_views`).  ★ And it caught the standing root lesson again -- the
rejected site is reached from `dazzle.scaly` and NOT from `dazzle_cli.scaly`,
which compiled clean throughout.  Pass EVERY root of the package.

Usage:  python3 tools/caretctor/retry.py packages/dazzle/0.1.0/*.scaly
        (run AFTER `fieldwise.py <pkg> --apply`, with the conversion uncommitted
        -- the original text is read from `git show HEAD:<file>`)
"""
import re, subprocess, sys, os, collections

ROOTS = sys.argv[1:]
PKG = ROOTS[0].split('/')[1]
held = []

def orig_lines(path):
    return subprocess.run(['git','show',f'HEAD:{path}'],capture_output=True,text=True).stdout.split('\n')

for _ in range(60):
    err = None
    for r in ROOTS:
        n = os.path.basename(r)[:-6]
        p = subprocess.run(['./scalyc/build/scalyc','-S','--no-prelude','--no-tests',
                            '-o','/dev/null', r], capture_output=True, text=True)
        out = (p.stdout + p.stderr).strip()
        if out:
            m = re.match(r'(\S+?\.scaly):(\d+):(\d+): (.*)', out.split('\n')[0])
            if not m: print('UNPARSED:', out.split('\n')[0]); sys.exit(1)
            err = (m.group(1), int(m.group(2)), m.group(4)); break
    if err is None:
        print('alle Roots sauber'); break
    f, line, msg = err
    cur = open(f, encoding='utf-8').read().split('\n')
    bad = cur[line-1]
    m = re.match(r'^(\s*)let (\w+) &(\S+?)\^(\w+)\(', bad)
    if not m:
        print(f'ZURUECKGEHALTEN? Zeile ist keine Konversion: {f}:{line}: {bad.strip()[:80]}')
        print('  ->', msg); sys.exit(1)
    # find the same `let NAME` in the ORIGINAL file and splice its block back
    name, indent = m.group(2), m.group(1)
    o = orig_lines(f)
    cand = [i for i,l in enumerate(o)
            if re.match(rf'^{re.escape(indent)}let {re.escape(name)}\s+\S+\.allocate\s*\(', l)]
    if len(cand) != 1:
        # disambiguate by nearest line number
        cand = sorted(cand, key=lambda i: abs(i-(line-1)))[:1]
    i = cand[0]
    block = [o[i]]
    k = i+1
    while k < len(o) and re.match(rf'^\s*set {re.escape(name)}\.\w+\s*:', o[k]):
        block.append(o[k]); k += 1
    cur[line-1:line] = block
    open(f,'w',encoding='utf-8').write('\n'.join(cur))
    held.append((f, line, msg))
    print(f'zurueckgenommen {f}:{line}  ({len(block)-1} Felder)  <- {msg}')
print(f'\n{len(held)} Sites zurueckgehalten')
