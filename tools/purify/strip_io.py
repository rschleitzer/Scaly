#!/usr/bin/env python3
"""Remove the `io` mark from every routine header in the given files.

Usage: tools/purify/strip_io.py <file>...

I/O is COMPUTED by the census (WriteCensus: `body_io` and its fixpoint), and a
package's generated interface states it for a bodyless declaration -- a source
never writes it. This takes out the marks the interim explicit rule had put on
the procedures. A line qualifies when, after its LAST closing parenthesis, only
header clauses follow (routine attributes, `returns`/`throws` with a type) and
then `io`, optionally followed by a `mutable`/`reads` clause, an opening brace
or a comment: a body line never has that shape. Works on generator sources
(a Python template line) as well as on `.scaly` files.
"""
import re
import sys

TAIL = re.compile(
    r'^((?:\s*@[A-Za-z_][A-Za-z0-9_]*\s+\S+)*'
    r'(?:\s*returns\s+\S+)?'
    r'(?:\s*throws\s+\S+)?)'
    r'\s+io\b'
    r'(\s*(?:(?:mutable|reads)\s+[A-Za-z_][^;]*)?\s*\{?\s*(?:;.*)?)$')


def strip_line(line):
    k = line.rfind(')')
    if k < 0:
        return None
    m = TAIL.match(line[k + 1:])
    if not m:
        return None
    rest = m.group(2)
    head = line[:k + 1] + m.group(1)
    return head + (' ' + rest.lstrip() if rest.strip() else '')


n = files = 0
for path in sys.argv[1:]:
    text = open(path, encoding='utf-8', errors='surrogateescape').read()
    lines = text.split('\n')
    changed = False
    for i, line in enumerate(lines):
        new = strip_line(line)
        if new is not None and new != line:
            lines[i] = new
            n += 1
            changed = True
    if changed:
        files += 1
        with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
            f.write('\n'.join(lines))
print(f"strip_io: {n} marks removed in {files} files")
