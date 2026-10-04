#!/usr/bin/env python3
"""tools/set-becomes.py — `set target: source` becomes `target := source`.

The assignment is written `x := y` since 2026-10-04 (the Algol tradition;
ROADMAP-operation.md), and `set` runs out. Both spellings model the same
Action, so the rewrite is emission-neutral: prove it by emitting the root
before and after and comparing the IR.

    tools/set-becomes.py [--apply] <file-or-directory>...

Without --apply the rewritten lines are shown. A `set` inside a comment or a
string is left alone; a leading `=` of the source (`set b: = 7`) goes with
the colon. Generated files (GENERATED in their first 15 lines) are skipped
and listed: their generator is what has to change.
"""
import os
import sys

apply = '--apply' in sys.argv
paths = [a for a in sys.argv[1:] if a != '--apply']


def files(path):
    if os.path.isfile(path):
        yield path
        return
    for root, dirs, names in os.walk(path):
        dirs[:] = [d for d in dirs if d != 'interface']
        for name in sorted(names):
            if name.endswith('.scaly'):
                yield os.path.join(root, name)


def rewrite(line):
    """One line of code (no line break): the line with every `set t:` turned
    into `t :=`, and how many were turned."""
    out = []
    count = 0
    i = 0
    n = len(line)
    in_string = False
    while i < n:
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
            out.append(line[i:])
            break
        boundary = i == 0 or line[i - 1] in ' \t'
        if boundary and line.startswith('set ', i):
            # the target: up to the colon outside parentheses, brackets and strings
            k = i + 4
            depth = 0
            quoted = False
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
                    break
                elif d == ';':
                    k = n
                    break
                k += 1
            if k < n:
                target = line[i + 4:k].strip()
                rest = line[k + 1:]
                stripped = rest.lstrip(' ')
                if stripped.startswith('= '):
                    stripped = stripped[2:]
                out.append(target + ' :=' + (' ' + stripped if stripped else ''))
                count += 1
                # the source may hold another `set` only after a separator;
                # none does in this tree, so the rest goes out as it stands
                break
        out.append(c)
        i += 1
    return ''.join(out), count


total = 0
touched = 0
for path in paths:
    for f in files(path):
        text = open(f, encoding='utf-8').read()
        if 'GENERATED' in '\n'.join(text.split('\n')[:15]):
            if ' set ' in text or '\nset ' in text:
                print('generated, skipped: ' + f)
            continue
        lines = text.split('\n')
        in_comment = False
        changed = 0
        for idx, line in enumerate(lines):
            if in_comment:
                if '*;' in line:
                    in_comment = False
                continue
            if ';*' in line and '*;' not in line.split(';*', 1)[1]:
                in_comment = True
                line_code = line.split(';*', 1)[0]
                new, c = rewrite(line_code)
                if c:
                    lines[idx] = new + ';*' + line.split(';*', 1)[1]
                    changed += c
                continue
            new, c = rewrite(line)
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
