#!/usr/bin/env python3
"""tools/operation-as.py — parentheses for a cast of the right side alone.

A postfix (`as`, `is`) takes the whole operation so far: `a + b as T` is
`(a + b) as T` (the author, 2026-10-04). The old
collapse gave a lone thing between an operator and the postfix the cast to
itself, so such a place means `a + (b as T)` today and has to say so. The
operation driver lists them (Planner.operation_postfix_site#):

    OPERATION_KEEP=<dir> tools/operation-compare.sh
    cat <dir>/*.on.err | grep '^operation-as ' | tools/operation-as.py [--apply]

Each line is `operation-as <file> <start> <end>`: byte offsets of the thing
and of the end of the postfix. Without --apply the places are only shown.
"""
import collections
import os
import sys

apply = '--apply' in sys.argv
root = os.getcwd() + '/'
sites = collections.defaultdict(set)
for line in sys.stdin:
    parts = line.split()
    if len(parts) != 4 or parts[0] != 'operation-as':
        continue
    path = parts[1]
    if path.startswith(root):
        path = path[len(root):]
    sites[path].add((int(parts[2]), int(parts[3])))

total = 0
for path in sorted(sites):
    data = open(path, 'rb').read()
    edits = []
    for start, end in sorted(sites[path]):
        # the thing: a parenthesis, a string, or a name (with its subscript)
        # up to the blank;
        # then ` as ` / ` is ` and the type with its brackets and its `?`
        # (the listed end runs on over white space and comments)
        k = start
        if data[k:k + 1] == b'(':
            depth = 0
            while True:
                c = data[k:k + 1]
                if c == b'"':
                    k += 1
                    while data[k:k + 1] != b'"':
                        k += 2 if data[k:k + 1] == b'\\' else 1
                depth += (c == b'(') - (c == b')')
                k += 1
                if depth == 0:
                    break
        elif data[k:k + 1] == b'"':
            k += 1
            while data[k:k + 1] != b'"':
                k += 2 if data[k:k + 1] == b'\\' else 1
            k += 1
        else:
            depth = 0
            while depth > 0 or data[k:k + 1] not in (b' ', b'\n'):
                depth += (data[k:k + 1] == b'[') - (data[k:k + 1] == b']')
                k += 1
        assert data[k:k + 4] in (b' as ', b' is '), (path, start, data[start:k + 12])
        k += 4
        depth = 0
        while True:
            c = data[k:k + 1]
            if c == b'[':
                depth += 1
            elif c == b']':
                if depth == 0:
                    break
                depth -= 1
            elif depth == 0 and not (c.isalnum() or c in (b'_', b'.', b'?')):
                break
            k += 1
        text = data[start:k]
        assert k <= end, (path, start, text)
        edits.append((start, start + len(text)))
    # a place inside another one cannot be rewritten by offsets
    for (a1, b1), (a2, b2) in zip(edits, edits[1:]):
        assert b1 <= a2, (path, a1, a2)
    total += len(edits)
    if not apply:
        for a, b in edits:
            print('%s:%d: (%s)' % (path, data.count(b'\n', 0, a) + 1, data[a:b].decode()))
        continue
    for a, b in reversed(edits):
        data = data[:a] + b'(' + data[a:b] + b')' + data[b:]
    open(path, 'wb').write(data)
print('%d places in %d files%s' % (total, len(sites), ' rewritten' if apply else ''))
