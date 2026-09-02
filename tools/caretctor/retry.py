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

import re, subprocess, sys, os, json

JOURNAL_PATH = 'tools/caretctor/.journal.json'
ROOTS = sys.argv[1:]
if not ROOTS:
    print('usage: retry.py <root.scaly> ...  (after fieldwise.py <pkg> --apply)')
    sys.exit(2)
if not os.path.exists(JOURNAL_PATH):
    print('retry: no %s -- run `fieldwise.py <pkg> --apply` first' % JOURNAL_PATH)
    sys.exit(2)
journal = json.load(open(JOURNAL_PATH, encoding='utf-8'))
# ★ keyed by the LINE'S CONTENT, never by its position: the file shifts under
# every revert, and one file can hold a dozen sites at one indent.
by_text = {}
for e in journal:
    by_text.setdefault((e['f'], e['new'].strip()), []).append(e)

held = []
for _ in range(200):
    err = None
    for r in ROOTS:
        p = subprocess.run(['./scalyc/build/scalyc', '-S', '--no-prelude', '--no-tests',
                            '-o', '/dev/null', r], capture_output=True, text=True)
        out = (p.stdout + p.stderr).strip()
        if out:
            m = re.match(r'(\S+?\.scaly):(\d+):(\d+): (.*)', out.split('\n')[0])
            if not m:
                print('UNPARSED:', out.split('\n')[0]); sys.exit(1)
            err = (m.group(1), int(m.group(2)), m.group(4)); break
    if err is None:
        print('alle Roots sauber'); break
    f, line, msg = err
    cur = open(f, encoding='utf-8').read().split('\n')
    key = (f, cur[line - 1].strip())
    if key not in by_text or not by_text[key]:
        print('HALT: %s:%d ist keine Konversion dieses Laufs' % (f, line))
        print('  ->', msg)
        print('  ->', cur[line - 1].strip()[:100])
        sys.exit(1)
    e = by_text[key].pop()
    cur[line - 1:line] = e['orig']
    open(f, 'w', encoding='utf-8').write('\n'.join(cur))
    held.append((f, line, msg))
    print('zurueckgenommen %s:%d (%d Zeilen)  <- %s' % (f, line, len(e['orig']), msg))
else:
    print('HALT: 200 Runden ohne sauberen Stand'); sys.exit(1)
print('\n%d Sites zurueckgehalten' % len(held))
