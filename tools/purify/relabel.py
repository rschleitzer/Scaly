#!/usr/bin/env python3
"""R1, step 1: relabel every `function` that writes as a `procedure`.

Usage: tools/purify/relabel.py <union.txt> [--apply]

The input is the union `tools/write-report.sh` writes. A body line reads
`<file>:<line>:<col>: fn <name> writes <params> direct|transitive`, located at the
declaration's first byte. A function is relabelled when it writes anything other
than an explicit PAGE parameter. A page parameter is where the function builds its
answer, like the implicit caller page, so writing only there keeps it pure.

The keyword is not part of the mangled name and the compiler reads `pure` nowhere
(2026-09-19), so the change must leave every root's emission byte-identical; the
caller of this tool proves that with `cmp`.

Refused, each listed with its reason:
  operator    an operator is pure by definition and has no `procedure` spelling
  generated   the file carries GENERATED in its head: the generator owns the text
  mismatch    the declaration at the location does not read `function <name>`
Without --apply nothing is written.
"""
import os
import re
import sys
from collections import Counter, defaultdict

BODY = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): fn (?P<name>\S+) '
                  r'writes (?P<params>\S*) (?P<how>direct|transitive)$')


def only_pages(params):
    for p in params.split(','):
        if not p:
            continue
        if p == 'global' or p.startswith('this:'):
            return False
        if not (re.search(r'Page\]$', p) or p.endswith(':Page')):
            return False
    return True


# The generated files, by their generators (tools/gencheck.sh checks the same set).
# A banner is not the test: codelens.scaly QUOTES one, and CharNames.scaly writes
# it in lower case.
GENERATED_FILES = {
    'packages/scalyc/0.1.0/scalyc/compiler/parser.scaly',    # codegen/parser-scaly.scm
    'packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly',    # codegen/syntax-scaly.scm
    'packages/dazzle/0.1.0/dazzle/CharNames.scaly',          # tools/chartablegen.py
    'packages/dazzle/0.1.0/dazzle/Sdata.scaly',              # tools/chartablegen.py
}


def is_generated(path, cache={}):
    if path in GENERATED_FILES:
        return True
    if not path.startswith('packages/tscaly/'):
        return False
    # packages/tscaly/tools/gen*.py stamp GENERATED into their outputs' heads
    if path not in cache:
        with open(path, errors='replace') as f:
            head = ''.join(f.readline() for _ in range(15))
        cache[path] = 'GENERATED' in head
    return cache[path]


def main():
    union = sys.argv[1]
    apply = '--apply' in sys.argv[2:]
    targets = defaultdict(list)          # file -> [(line, col, name)]
    refused = defaultdict(list)          # reason -> [loc name]
    kept_page = 0
    for raw in open(union):
        m = BODY.match(raw.rstrip('\n'))
        if not m:
            continue
        if only_pages(m.group('params')):
            kept_page += 1
            continue
        path, line, col, name = m.group('file'), int(m.group('line')), int(m.group('col')), m.group('name')
        loc = f"{path}:{line}:{col}"
        if not os.path.exists(path):
            refused['missing file'].append(f"{loc} {name}")
            continue
        if is_generated(path):
            refused['generated'].append(f"{loc} {name}")
            continue
        targets[path].append((line, col, name))

    done = Counter()
    for path, items in sorted(targets.items()):
        lines = open(path, encoding='utf-8', errors='surrogateescape').read().split('\n')
        changed = False
        for line, col, name in items:
            text = lines[line - 1]
            rest = text[col - 1:]
            if rest.startswith('operator'):
                refused['operator'].append(f"{path}:{line}:{col} {name}")
                continue
            if rest.startswith('procedure '):
                done['already'] += 1
                continue
            if not re.match(r'function\s+' + re.escape(name) + r'\b', rest):
                refused['mismatch'].append(f"{path}:{line}:{col} {name} | {text.strip()[:70]}")
                continue
            lines[line - 1] = text[:col - 1] + 'procedure' + rest[len('function'):]
            done[path.split('/')[1]] += 1
            changed = True
        if changed and apply:
            with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
                f.write('\n'.join(lines))
    total = sum(v for k, v in done.items() if k != 'already')
    print(f"relabel{'' if apply else ' (dry run)'}: {total} functions -> procedure; "
          f"kept (writes only a page parameter): {kept_page}")
    for pkg, n in done.most_common():
        print(f"  {n:6d}  {pkg}")
    for reason, items in sorted(refused.items()):
        print(f"refused, {reason}: {len(items)}")
        for it in items[:25]:
            print(f"    {it}")
        if len(items) > 25:
            print(f"    ... {len(items) - 25} more")


if __name__ == '__main__':
    main()
