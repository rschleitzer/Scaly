#!/usr/bin/env python3
"""Compiler-as-arbiter driver for a subscript rewrite.

`tools/subscript.py` cannot see TYPES, and the receiver's type is exactly what
decides whether `X[i]` exists: `Array`, `Vector` and `Slice` carry an
`operator []`, while `String` and the ports' own `StringC` carry a
`get_buffer()` and NO subscript at all.  So the rewrite is proposed by the
tool and JUDGED by the compiler, at LINE granularity rather than by file --
reverting a whole file for one bad site throws away the good ones in it.

Loop: emit every package root, collect the `file:line:` of every error,
revert exactly those lines to their pre-rewrite text, emit again.  Stops when
a pass is clean or when a pass reverts nothing (which means the error is NOT
one of our lines and the run has to be read by a human).

Usage: tools/subscript_driver.py <baseline-git-ref-or-'HEAD'>
"""
import os, re, subprocess, sys, collections

SC = os.environ.get('SCRATCH', '/tmp')
ROOTS = [r for r in subprocess.run(['sh', '-c', 'ls packages/*/0.1.0/*.scaly'],
         capture_output=True, text=True).stdout.split()
         if 'stdlib' not in r]
ERR = re.compile(r'^(packages/\S+\.scaly):(\d+):\d+: error:')

def orig(path, ref):
    return subprocess.run(['git', 'show', f'{ref}:{path}'],
                          capture_output=True, text=True).stdout.split('\n')

def main():
    ref = sys.argv[1] if len(sys.argv) > 1 else 'HEAD'
    for rnd in range(1, 12):
        bad = collections.defaultdict(set)
        for r in ROOTS:
            p = subprocess.run(['./scalyc/build/scalyc', '-S', '--no-prelude',
                                '--no-tests', '-o', f'{SC}/drv.ll', r],
                               capture_output=True, text=True)
            for line in (p.stdout + p.stderr).split('\n'):
                m = ERR.match(line.strip())
                if m: bad[m.group(1)].add(int(m.group(2)))
        if not bad:
            print(f'round {rnd}: CLEAN'); return 0
        n = 0
        for path, lns in sorted(bad.items()):
            cur = open(path, encoding='utf8').read().split('\n')
            old = orig(path, ref)
            for ln in sorted(lns):
                # ★A CONFORMANCE error is reported at the FUNCTION's line, not
                # at the expression's (CLAUDE.md records this for exactly this
                # tool). So when the reported line is not one of ours, revert
                # the NEAREST changed line at or after it -- the body of the
                # routine the compiler just named.
                i = ln - 1
                if not (i < len(cur) and i < len(old) and cur[i] != old[i]):
                    j = i
                    while j < len(cur) and j < len(old) and j < i + 60:
                        if cur[j] != old[j]: i = j; break
                        j += 1
                if i < len(cur) and i < len(old) and cur[i] != old[i]:
                    print(f'  revert {path}:{i+1}  {cur[i].strip()[:70]}')
                    cur[i] = old[i]; n += 1
            open(path, 'w', encoding='utf8').write('\n'.join(cur))
        print(f'round {rnd}: reverted {n}')
        if n == 0:
            print('  no reverted line -- the error is not one of ours:')
            for path, lns in sorted(bad.items()):
                print('   ', path, sorted(lns)[:10])
            return 1
    print('did not converge'); return 1

sys.exit(main())
