#!/usr/bin/env python3
"""o2norm.py OLD.ll NEW.ll -- compare two optimized modules function by function.

Each `define`d body is reduced to the MULTISET of its instructions with every
SSA value, block label, metadata and attribute-group reference masked, so two
bodies that differ only in naming or in the ORDER of their blocks compare
equal. Prints the functions that differ and the functions only one side has;
exit status 1 if there is any.
"""
import re
import sys
from collections import Counter

MASK = [
    (re.compile(r'%"[^"]*"'), '%v'),      # quoted value names (`%"a.b().c1"`)
    (re.compile(r'%[-\w.$]+'), '%v'),
    (re.compile(r'![\w.]+'), '!m'),
    (re.compile(r'#\d+'), '#a'),
    (re.compile(r'@\.(str|sconst|unwrap\.at)[.\w]*'), '@.K'),
]
SKIP = {'scaly_build_stamp'}


def bodies(path):
    out, cur, buf = {}, None, []
    for line in open(path, errors='replace'):
        if line.startswith('define '):
            m = re.search(r'@([-\w.$]+)\(', line)
            cur, buf = (m.group(1) if m else line), []
            continue
        if cur is None:
            continue
        if line.startswith('}'):
            out[cur] = buf
            cur = None
            continue
        s = line.strip()
        if not s or s.endswith(':') or re.match(r'^[-\w.]+:\s', s):
            continue                      # a block label
        for rx, rep in MASK:
            s = rx.sub(rep, s)
        s = re.sub(r'\s*;.*$', '', s)     # `; preds = ...`
        # a phi's incoming ORDER follows the block order: sort it
        m = re.match(r'^(%v = phi [^\[]*)(\[.*\])$', s)
        if m:
            items = re.findall(r'\[[^\]]*\]', m.group(2))
            s = m.group(1) + ', '.join(sorted(items))
        buf.append(s)
    return {k: Counter(v) for k, v in out.items()}


def main():
    a, b = bodies(sys.argv[1]), bodies(sys.argv[2])
    only_a = sorted(set(a) - set(b) - SKIP)
    only_b = sorted(set(b) - set(a) - SKIP)
    diff = sorted(k for k in set(a) & set(b) if k not in SKIP and a[k] != b[k])
    print(f'functions {len(a)} / {len(b)}: {len(diff)} differ, {len(only_a)} only old, {len(only_b)} only new')
    for k in diff[:40]:
        print('  DIFFERS', k)
    for k in only_a[:10]:
        print('  ONLY OLD', k)
    for k in only_b[:10]:
        print('  ONLY NEW', k)
    sys.exit(1 if diff or only_a or only_b else 0)


if __name__ == '__main__':
    main()
