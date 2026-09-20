#!/usr/bin/env python3
"""R2: remove every `let`/`var` mark from the field lists of the given files.

Usage: tools/purify/strip_var.py <file>...

The inverse of mark_var.py, for the two-phase seed refresh: a committed seed
whose parser predates the marks cannot read a source that carries them, so
the compiler is first built and its seed refreshed on unmarked sources, and
the marks go back in afterwards (mark_var.py off the same report). Only a
mark inside a record's field list is touched; `let`/`var` statements in
bodies are not field marks and are left alone.
"""
import re
import sys

n = files = 0
for path in sys.argv[1:]:
    text = open(path, encoding='utf-8', errors='surrogateescape').read()
    out = []
    pos = 0
    changed = False
    for m in re.finditer(r'^[ \t]*define [A-Z][A-Za-z0-9_]*(?:\[[^\]]*\])?[ \t]*\n?[ \t]*\(', text, re.M):
        start = m.end() - 1
        depth = 0
        i = start
        while i < len(text):
            c = text[i]
            if c == ';':
                i = text.find('\n', i)
                if i < 0:
                    i = len(text)
                continue
            if c == '(':
                depth += 1
            elif c == ')':
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body = text[start:i]
        # a `;` comment inside the list may spell a field name: never touched
        masked = re.sub(r';[^\n]*', lambda c: ' ' * len(c.group(0)), body)
        cuts = [(mm.start(1) + len(mm.group(1)), mm.end()) for mm in
                re.finditer(r'(^|[ \t(,])(?:let|var)[ \t]+(?=[a-z_][a-z_0-9]*[ \t]*:)', masked, re.M)]
        new_body = body
        for a_, b_ in sorted(cuts, reverse=True):
            new_body = new_body[:a_] + new_body[b_:]
        k = len(cuts)
        if k:
            out.append(text[pos:start])
            out.append(new_body)
            pos = i
            n += k
            changed = True
    out.append(text[pos:])
    if changed:
        files += 1
        with open(path, 'w', encoding='utf-8', errors='surrogateescape') as f:
            f.write(''.join(out))
print(f"strip_var: {n} marks removed in {files} files")
