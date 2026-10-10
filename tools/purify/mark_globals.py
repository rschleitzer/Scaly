#!/usr/bin/env python3
"""R1, globals: give every procedure the clause naming the globals it writes.

Usage: tools/purify/mark_globals.py <gw.txt> [--apply] [--generated-too]

The input is the `gw` lines `tools/write-report.sh` writes into its per-root
reports (`<file>:<line>:<col>: gw <name> a,b,c`, located at the declaration):
the globals a body writes, itself or through what it calls. The runtime's own
state (globals declared in scaly/memory, scaly/fiber*, scaly/os*,
scaly/cluster*) is ambient and dropped, and so is every `shared atomic` cell. What is left is written as a clause
after the signature, sorted: `procedure f(mutable this) returns int mutable a, b`
- at the end of the line that closes the parameter list, before a trailing
comment. A `function` that writes a global becomes a `procedure` on the way.

The clause is not part of the mangled name and the emitter reads it nowhere, so
every root must re-emit byte-identically; the caller proves that with `cmp`.

Refused, each listed with its reason:
  generated   one of the generated files (--generated-too marks them as the
              target their generators must reproduce)
  init        an initializer has no clause (the check does not judge one)
  mismatch    the declaration at the location is not `function|procedure <name>`
  same-line   the body starts on the signature's line
"""
import glob
import os
import re
import sys
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(__file__))
from relabel import is_generated   # noqa: E402

GW = re.compile(r'^(?P<file>[^:]+):(?P<line>\d+):(?P<col>\d+): gw (?P<name>\S+) (?P<names>.*)$')
RUNTIME = re.compile(r'packages/scaly/0\.1\.0/scaly/(memory/|fiber|os|cluster)')


def ambient_names():
    out = set()
    # `shared atomic` cells coordinate themselves and are never named
    for f in glob.glob('packages/*/0.1.0/**/*.scaly', recursive=True):
        for m in re.finditer(r'^\s*shared\s+atomic\s+([A-Za-z][A-Za-z0-9_]*)\s*:',
                             open(f, errors='replace').read(), re.M):
            out.add(m.group(1))
    for f in glob.glob('packages/scaly/0.1.1/scaly/**/*.scaly', recursive=True):
        if not RUNTIME.search(f):
            continue
        for m in re.finditer(r'^\s*(?:mutable|shared)\s+([A-Za-z][A-Za-z0-9_]*)\s*:',
                             open(f, errors='replace').read(), re.M):
            out.add(m.group(1))
    return out


def skip_type(text, i):
    """The end of one type token at i (brackets balanced, `?` included)."""
    depth = 0
    while i < len(text):
        c = text[i]
        if c == '[':
            depth += 1
        elif c == ']':
            depth -= 1
        elif c in ' \t\n' and depth == 0:
            break
        i += 1
    return i


def signature_end(text, at, name):
    """(offset to insert the clause, keyword) or (None, reason)."""
    m = re.compile(r'(function|procedure)\s+' + re.escape(name)
                   + r'(#|\[[^\]\n]*\])?\s*\(').match(text, at)
    if not m:
        return None, 'mismatch'
    depth, i = 0, m.end() - 1
    while i < len(text):
        c = text[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
            if depth == 0:
                break
        i += 1
    i += 1
    while True:
        j = i
        while j < len(text) and text[j] in ' \t':
            j += 1
        word = re.match(r'(returns|throws)\b', text[j:j + 8])
        if word:
            k = j + len(word.group(1))
            while k < len(text) and text[k] in ' \t':
                k += 1
            i = skip_type(text, k)
            continue
        # an `io` already declared: the clause goes after it
        if re.match(r'io(\s|$)', text[j:j + 3]):
            i = j + 2
            continue
        # an existing clause: extend it
        if text.startswith('mutable ', j):
            return ('extend', j), m.group(1)
        # the rest of the line must be empty or a comment
        eol = text.find('\n', j)
        rest = text[j:eol]
        if rest == '' or rest.startswith(';'):
            return ('insert', i), m.group(1)
        return None, 'same-line'


def main():
    src = sys.argv[1]
    apply = '--apply' in sys.argv[2:]
    generated_too = '--generated-too' in sys.argv[2:]
    ambient = ambient_names()
    want = defaultdict(set)
    for raw in open(src):
        m = GW.match(raw.rstrip('\n'))
        if not m:
            continue
        names = {n for n in m.group('names').split(',') if n and n not in ambient}
        if names:
            want[(m.group('file'), int(m.group('line')), int(m.group('col')), m.group('name'))] |= names
    refused = defaultdict(list)
    by_file = defaultdict(list)
    for (path, line, col, name), names in want.items():
        if name == 'init':
            refused['init'].append(f"{path}:{line}:{col} {sorted(names)}")
            continue
        if is_generated(path) and not generated_too:
            refused['generated'].append(f"{path}:{line}:{col} {name} {sorted(names)}")
            continue
        by_file[path].append((line, col, name, names))
    done = Counter()
    relabelled = 0
    for path, entries in sorted(by_file.items()):
        text = open(path, encoding='utf-8', errors='surrogateescape').read()
        starts = [0]
        for ln in text.split('\n'):
            starts.append(starts[-1] + len(ln) + 1)
        edits = []                      # (offset, old_len, new_text)
        for line, col, name, names in entries:
            at = starts[line - 1] + col - 1
            where, kw = signature_end(text, at, name)
            if where is None:
                refused[kw].append(f"{path}:{line}:{col} {name} {sorted(names)}")
                continue
            if kw == 'function':
                edits.append((at, len('function'), 'procedure'))
                relabelled += 1
            how, off = where
            # `io` is computed since 2026-09-19 -- the generated interfaces say
            # it, a source does not; a function that reaches it is still
            # relabelled a procedure (the name set was non-empty)
            wants_io = False
            names = names - {'io'}
            sig = text[at:off]
            has_io = re.search(r'\)\s.*\bio\s*$', sig) is not None or re.search(r'\bio\s*$', sig) is not None
            io_text = ' io' if wants_io and not has_io else ''
            if how == 'insert':
                clause = (' mutable ' + ', '.join(sorted(names))) if names else ''
                if io_text or clause:
                    edits.append((off, 0, io_text + clause))
            else:
                eol = text.find('\n', off)
                have = re.match(r'mutable\s+([A-Za-z0-9_, ]+?)\s*(;|$)', text[off:eol])
                old = {n.strip() for n in have.group(1).split(',')} if have else set()
                new = sorted(old | names)
                clause = 'mutable ' + ', '.join(new)
                edits.append((off, len('mutable ') + len(have.group(1)), (io_text.strip() + ' ' if io_text else '') + clause))
            done[path.split('/')[1]] += 1
        for off, n, new in sorted(edits, reverse=True):
            text = text[:off] + new + text[off + n:]
        if apply and edits:
            with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
                f.write(text)
    print(f"mark_globals{'' if apply else ' (dry run)'}: {sum(done.values())} clauses, "
          f"{relabelled} functions relabelled; {len(ambient)} runtime globals ambient")
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
