#!/usr/bin/env python3
"""tools/set-becomes.py — `set target: source` becomes `target := source`.

The assignment is written `x := y` since 2026-10-04 (the Algol tradition),
and `set` runs out. Both spellings model the same
Action, so the rewrite is emission-neutral: prove it by emitting the root
before and after and comparing the IR.

    tools/set-becomes.py [--apply] [--generated-too] <file-or-directory>...

Without --apply the rewritten lines are shown. A `set` inside a comment or a
string is left alone; a leading `=` of the source (`set b: = 7`) goes with
the colon. Generated files (GENERATED in their first 15 lines) are skipped
and listed: their generator is what has to change (--generated-too rewrites
them as the target the changed generator then has to reproduce).
"""
import os
import re
import sys

apply = '--apply' in sys.argv
generated_too = '--generated-too' in sys.argv
paths = [a for a in sys.argv[1:] if not a.startswith('--')]


def files(path):
    if os.path.isfile(path):
        yield path
        return
    for root, dirs, names in os.walk(path):
        dirs[:] = [d for d in dirs if d != 'interface']
        for name in sorted(names):
            if name.endswith('.scaly'):
                yield os.path.join(root, name)


def rewrite(line, in_block):
    """One line (no line break): the line with every `set t:` turned into
    `t :=`, how many were turned, and whether a `;* ... *;` comment is still
    open at its end."""
    out = []
    count = 0
    i = 0
    n = len(line)
    in_string = False
    while i < n:
        if in_block:
            end = line.find('*;', i)
            if end < 0:
                out.append(line[i:])
                return ''.join(out), count, True
            out.append(line[i:end + 2])
            i = end + 2
            in_block = False
            continue
        c = line[i]
        if in_string:
            out.append(c)
            if c == '\\' and i + 1 < n:
                out.append(line[i + 1])
                i += 2
                continue
            if c == '"':
                in_string = False
            i += 1
            continue
        if c == '"':
            in_string = True
            out.append(c)
            i += 1
            continue
        if c == ';':
            if line.startswith(';*', i):
                in_block = True
                out.append(';*')
                i += 2
                continue
            out.append(line[i:])
            break
        boundary = i == 0 or line[i - 1] in ' \t{'
        if boundary and line.startswith('set ', i):
            # the target: up to the colon outside parentheses, brackets and strings
            k = i + 4
            depth = 0
            quoted = False
            found = False
            while k < n:
                d = line[k]
                if quoted:
                    if d == '\\':
                        k += 1
                    elif d == '"':
                        quoted = False
                elif d == '"':
                    quoted = True
                elif d in '([':
                    depth += 1
                elif d in ')]':
                    depth -= 1
                elif d == ':' and depth == 0:
                    found = True
                    break
                elif d == ';':
                    break
                k += 1
            if found:
                target = line[i + 4:k].strip()
                k += 1
                while k < n and line[k] == ' ':
                    k += 1
                if line.startswith('= ', k):
                    k += 2
                out.append(target + ' :=' + (' ' if k < n else ''))
                count += 1
                i = k
                continue
        out.append(c)
        i += 1
    return ''.join(out), count, in_block


total = 0
touched = 0
for path in paths:
    for f in files(path):
        text = open(f, encoding='utf-8').read()
        if not generated_too and 'GENERATED' in '\n'.join(text.split('\n')[:15]):
            if ' set ' in text or '\nset ' in text:
                print('generated, skipped: ' + f)
            continue
        lines = text.split('\n')
        in_block = False
        changed = 0
        for idx, line in enumerate(lines):
            if f.endswith('.sgm'):
                # an SGML entity (`&lt;`) ends in the comment sign
                held = re.sub(r'&(#?[a-z0-9]+);', lambda m: '\x01' + m.group(1) + '\x02', line)
                new, c, in_block = rewrite(held, in_block)
                new = new.replace('\x01', '&').replace('\x02', ';')
            else:
                new, c, in_block = rewrite(line, in_block)
            if c:
                if not apply:
                    print('%s:%d: %s' % (f, idx + 1, new.strip()))
                lines[idx] = new
                changed += c
        if changed:
            total += changed
            touched += 1
            if apply:
                open(f, 'w', encoding='utf-8').write('\n'.join(lines))
print('%d assignments in %d files%s' % (total, touched, ' rewritten' if apply else ''))
