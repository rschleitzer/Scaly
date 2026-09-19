#!/usr/bin/env python3
"""Remove the `mutable g, h` clause after every signature in the given files.

Usage: tools/purify/strip_globals.py <file>...

The inverse of mark_globals.py, for re-marking a tree after its globals moved:
strip, change the code, run tools/write-report.sh, then mark_globals.py again.
A `mutable` PARAMETER mark sits inside the parentheses and is not touched.
"""
import re
import sys

PAT = re.compile(r'^(\s*(?:procedure|function)\s.*?\S)\s+mutable\s+[A-Za-z_][A-Za-z0-9_]*'
                 r'(?:\s*,\s*[A-Za-z_][A-Za-z0-9_]*)*(\s*;.*)?$')

n = files = 0
for path in sys.argv[1:]:
    text = open(path, encoding='utf-8', errors='surrogateescape').read()
    lines = text.split('\n')
    changed = False
    for i, line in enumerate(lines):
        m = PAT.match(line)
        if m:
            lines[i] = m.group(1) + (m.group(2) or '')
            n += 1
            changed = True
    if changed:
        files += 1
        with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
            f.write('\n'.join(lines))
print(f"strip_globals: {n} clauses removed in {files} files")
