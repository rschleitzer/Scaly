#!/usr/bin/env python3
"""Compare two emitted .ll files by the OPCODE SEQUENCE of every defined symbol.

The checksum for a conversion that renames symbols.  A signature change moves
the Itanium name (`encode_type` mangles `pointer` -> "P" and `ref` -> "R"), so
comparing by NAME reports every converted routine as missing and proves
nothing.  Identical machine code cannot answer differently, so:

  * symbols present in both, compared instruction for instruction;
  * symbols present in one, PAIRED by their opcode sequence -- a pair whose two
    names differ only in the expected substitution is a rename and nothing more;
  * anything left over is a real difference and must be explained.

Usage: tools/opcheck.py <a.ll> <b.ll> [expected-substitution ...]
       tools/opcheck.py a.ll b.ll P4Page=R4Page
"""
import re, sys, collections, hashlib

DEFINE = re.compile(r'^define\b.*?@"?([^"(]+)"?\s*\(')

def bodies(path):
    out = {}
    name = None; ops = []
    for line in open(path, encoding='utf-8', errors='replace'):
        if line.startswith('define'):
            m = DEFINE.match(line)
            name = m.group(1) if m else None
            ops = []
            continue
        if name is None: continue
        if line.startswith('}'):
            out[name] = ops; name = None; continue
        s = line.strip()
        if not s or s.endswith(':'): continue
        # the opcode is the first word after an optional `%x =`
        s = re.sub(r'^%\S+\s*=\s*', '', s)
        ops.append(s.split(' ', 1)[0])
    return out

def h(ops):
    return hashlib.sha256('\n'.join(ops).encode()).hexdigest()[:16]

def main(a, b, subs):
    A, B = bodies(a), bodies(b)
    common = set(A) & set(B)
    differ = [n for n in sorted(common) if A[n] != B[n]]
    onlyA = sorted(set(A) - set(B)); onlyB = sorted(set(B) - set(A))

    # pair the leftovers by opcode sequence
    byhash = collections.defaultdict(list)
    for n in onlyB: byhash[h(B[n])].append(n)
    renames = []; unpaired_a = []
    for n in onlyA:
        cands = byhash.get(h(A[n]))
        if cands:
            m = cands.pop(0); renames.append((n, m))
        else:
            unpaired_a.append(n)
    unpaired_b = [n for v in byhash.values() for n in v]

    expected = []; unexpected = []
    for n, m in renames:
        if subs:
            got = n
            for s in subs:
                src, dst = s.split('=')
                got = got.replace(src, dst)
            ok = got == m
        else:
            # default: the only accepted rename is a pointer -> ref substitution,
            # i.e. equal length differing only where A has "P" and B has "R"
            ok = len(n) == len(m) and all(x == y or (x == 'P' and y == 'R')
                                          for x, y in zip(n, m))
        (expected if ok else unexpected).append((n, m))

    print(f'{a} -> {b}')
    print(f'  defines            {len(A):6} -> {len(B)}')
    print(f'  common, identical  {len(common) - len(differ):6}')
    print(f'  common, DIFFERING  {len(differ):6}')
    print(f'  renamed (opcode-identical) {len(renames):6}   as expected {len(expected)}'
          + (f'   UNEXPECTED {len(unexpected)}' if unexpected else ''))
    print(f'  unpaired in A      {len(unpaired_a):6}')
    print(f'  unpaired in B      {len(unpaired_b):6}')
    for n in differ[:20]: print(f'    DIFFER  {n}')
    for n, m in unexpected[:20]: print(f'    RENAME? {n}\n         -> {m}')
    for n in unpaired_a[:20]: print(f'    ONLY-A  {n}')
    for n in unpaired_b[:20]: print(f'    ONLY-B  {n}')
    return 0 if not (differ or unpaired_a or unpaired_b or unexpected) else 1

if __name__ == '__main__':
    sys.exit(main(sys.argv[1], sys.argv[2], sys.argv[3:]))
