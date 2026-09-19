#!/usr/bin/env python3
"""R1, step 2: mark every parameter a procedure writes as `mutable`.

Usage: tools/purify/mark.py <union.txt> [--apply]

The input is the union `tools/write-report.sh` writes. A procedure line reads
`<file>:<line>:<col>: proc <name> writes <params>`, located at the declaration's
first byte; `<params>` is a comma list of `name:type` plus `global`. Every named
parameter gets `mutable` in front of it, except
  global      not a parameter
  a page      `ref[Page]`/`pointer[Page]`/`Page`: the result region, which the
              check allows everywhere, like the implicit caller page
A procedure that writes nothing through its parameters stays unmarked: since
step 2 an unmarked parameter is one the procedure only reads.

The mark is not part of the mangled name and the emitter reads it nowhere, so
the change must leave every root's emission byte-identical; the caller of this
tool proves that with `cmp`.

Refused, each listed with its reason:
  generated   one of the generated files (its generator owns the text)
  mismatch    the declaration at the location does not read `procedure <name>(`
  missing     a written name is not in the declaration's parameter list
Without --apply nothing is written; --generated-too marks the generated files as
well, as the target their generators must reproduce.
"""
import os
import re
import sys
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from relabel import split_params, is_generated   # noqa: E402

PROC = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): proc (?P<name>\S+) '
                  r'writes (?P<params>.*)$')


def is_page(entry):
    return bool(re.search(r'Page\]$', entry)) or entry.endswith(':Page')


def written_names(params):
    out = set()
    for p in split_params(params):
        if not p or p == 'global' or is_page(p):
            continue
        out.add(p.split(':', 1)[0])
    return out


def param_list_span(text, start, name):
    """(open, close) offsets of the parameter list of `procedure <name>` at start."""
    m = re.compile(r'procedure\s+' + re.escape(name) + r'#?\s*\(').match(text, start)
    if not m:
        return None
    open_ = m.end() - 1
    depth = 0
    for i in range(open_, len(text)):
        c = text[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
            if depth == 0:
                return open_, i
    return None


def items(text, open_, close):
    """[(offset of the item's name, name)] at bracket depth 0 between the parens."""
    out, depth, begin = [], 0, open_ + 1
    for i in range(open_ + 1, close + 1):
        c = text[i]
        if c == '[' or c == '(':
            depth += 1
        elif c == ']' or (c == ')' and i != close):
            depth -= 1
        if (c == ',' and depth == 0) or i == close:
            seg = text[begin:i]
            m = re.match(r'\s*(mutable\s+)?([A-Za-z][A-Za-z0-9_]*)', seg)
            if m:
                out.append((begin + m.start(2), m.group(2), bool(m.group(1))))
            begin = i + 1
    return out


def main():
    union = sys.argv[1]
    apply = '--apply' in sys.argv[2:]
    # the generated files are marked too, as the TARGET their generators must
    # then reproduce (tools/gencheck.sh proves they do)
    generated_too = '--generated-too' in sys.argv[2:]
    want = defaultdict(set)              # (file, line, col, name) -> names
    for raw in open(union):
        m = PROC.match(raw.rstrip('\n'))
        if not m:
            continue
        names = written_names(m.group('params'))
        if names:
            key = (m.group('file'), int(m.group('line')), int(m.group('col')), m.group('name'))
            want[key] |= names
    by_file = defaultdict(list)
    refused = defaultdict(list)
    for (path, line, col, name), names in want.items():
        if is_generated(path) and not generated_too:
            refused['generated'].append(f"{path}:{line}:{col} {name} {sorted(names)}")
            continue
        by_file[path].append((line, col, name, names))
    done = Counter()
    marks = 0
    for path, entries in sorted(by_file.items()):
        text = open(path, encoding='utf-8', errors='surrogateescape').read()
        starts = [0]
        for ln in text.split('\n'):
            starts.append(starts[-1] + len(ln) + 1)
        inserts = []
        for line, col, name, names in entries:
            at = starts[line - 1] + col - 1
            span = param_list_span(text, at, name)
            if span is None:
                refused['mismatch'].append(f"{path}:{line}:{col} {name}")
                continue
            found = {n: (off, marked) for off, n, marked in items(text, *span)}
            for n in sorted(names):
                if n not in found:
                    refused['missing'].append(f"{path}:{line}:{col} {name} {n}")
                    continue
                off, marked = found[n]
                if not marked:
                    inserts.append(off)
        if not inserts:
            continue
        for off in sorted(set(inserts), reverse=True):
            text = text[:off] + 'mutable ' + text[off:]
        marks += len(set(inserts))
        done[path.split('/')[1]] += len(set(inserts))
        if apply:
            with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
                f.write(text)
    print(f"mark{'' if apply else ' (dry run)'}: {marks} parameters marked mutable "
          f"in {sum(1 for _ in want)} procedures")
    for pkg, n in done.most_common():
        print(f"  {n:6d}  {pkg}")
    for reason, lst in sorted(refused.items()):
        print(f"refused, {reason}: {len(lst)}")
        for it in lst[:25]:
            print(f"    {it}")
        if len(lst) > 25:
            print(f"    ... {len(lst) - 25} more")


if __name__ == '__main__':
    main()
