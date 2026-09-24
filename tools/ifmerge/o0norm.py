#!/usr/bin/env python3
"""o0norm.py OLD.ll NEW.ll [FUNC...] -- compare UNOPTIMIZED functions structurally.

The second proof for the functions o2norm.py cannot settle. `opt -O2` may
canonicalize two equivalent control-flow shapes differently (a switch against a
bit mask, a phi against a branch), so a handful of functions differ there
although the source change is a pure refolding. At -O0 the two shapes are
isomorphic up to EMPTY FORWARDING BLOCKS (`if.end: br label %x`, one per
nested `if`) and block order. This removes every forwarding block (its
predecessors branch to its target instead), renames the blocks in depth-first
order from the entry following the terminator's successor order, masks value
names, and compares the listings. Equal means the same instructions in the same
control flow. Blocks that only return the same constant count as one (a run
of `if x / return true` has one per `if`, its `or` fold one in all), also
after the frame release a routine with a frame emits before every return.
"""
import re
import sys

VAL = re.compile(r'%"[^"]*"|%[-\w.$]+')
LABEL_REF = re.compile(r'label %("[^"]*"|[-\w.$]+)')


def functions(path):
    out, cur, blocks, order, bname = {}, None, None, None, None
    for line in open(path, errors='replace'):
        if line.startswith('define '):
            m = re.search(r'@([-\w.$]+)\(', line)
            cur = m.group(1)
            blocks, order, bname = {}, [], 'entry'
            blocks[bname] = []
            order.append(bname)
            continue
        if cur is None:
            continue
        if line.startswith('}'):
            out[cur] = (blocks, order)
            cur = None
            continue
        s = line.rstrip('\n')
        m = re.match(r'^("[^"]*"|[-\w.$]+):', s)
        if m:
            bname = m.group(1)
            if bname != 'entry' or bname not in blocks:
                blocks[bname] = []
                order.append(bname)
            continue
        s = s.strip()
        if s:
            s = re.sub(r'\s*;.*$', '', s)
            s = re.sub(r', !dbg ![\w.]+', '', s)
            blocks[bname].append(s)
    return out


def canon(blocks, order):
    # forwarding blocks: exactly one instruction, an unconditional branch
    fwd = {}
    for b in order:
        ins = blocks[b]
        if len(ins) == 1:
            m = re.fullmatch(r'br label %("[^"]*"|[-\w.$]+)', ins[0])
            if m and b != 'entry':
                fwd[b] = m.group(1)

    # blocks that only return a constant are interchangeable: a run of
    # `if x / return true` has one per `if`, its `or` fold has one in all
    ret_rep = {}
    for b in order:
        ins = blocks[b]
        # `ret <const>`, optionally after releasing this function's frame
        key = None
        if b != 'entry' and ins and re.fullmatch(r'ret \w+ (true|false|-?\d+|null)', ins[-1]):
            rest = ins[:-1]
            if not rest or (len(rest) == 1 and re.fullmatch(r'call void @_Z19scaly_release_frameP5Frame\(ptr %frame\)', rest[0])):
                key = '|'.join(ins)
        if key:
            ret_rep.setdefault(key, b)
            if ret_rep[key] != b:
                fwd[b] = ret_rep[key]

    def resolve(b):
        seen = set()
        while b in fwd and b not in seen:
            seen.add(b)
            b = fwd[b]
        return b
    # depth-first numbering from the entry
    names, stack, seq = {}, ['entry'], []
    while stack:
        b = resolve(stack.pop())
        if b in names or b not in blocks:
            continue
        names[b] = 'b%d' % len(names)
        seq.append(b)
        succ = [resolve(x) for x in LABEL_REF.findall(' '.join(blocks[b][-1:]))]
        for x in reversed(succ):
            if x not in names:
                stack.append(x)
    listing = []
    for b in seq:
        listing.append(names[b] + ':')
        for ins in blocks[b]:
            ins = LABEL_REF.sub(lambda m: 'label %' + names.get(resolve(m.group(1)), '?'), ins)
            # phi incoming blocks: `[ v, %blk ]`
            ins = re.sub(r'\[ ([^,\]]+), %("[^"]*"|[-\w.$]+) \]',
                         lambda m: '[ ' + m.group(1) + ', %' + names.get(resolve(m.group(2)), '?') + ' ]', ins)
            ins = re.sub(r'%(?!b\d)("[^"]*"|[-\w.$]+)', '%v', ins)
            listing.append('  ' + ins)
    return listing


def main():
    a, b = functions(sys.argv[1]), functions(sys.argv[2])
    names = sys.argv[3:] or sorted(set(a) & set(b))
    bad = 0
    for n in names:
        if n not in a or n not in b:
            print('MISSING', n)
            bad += 1
            continue
        if canon(*a[n]) != canon(*b[n]):
            print('DIFFERS', n)
            bad += 1
        else:
            print('SAME   ', n)
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
