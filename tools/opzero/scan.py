#!/usr/bin/env python3
"""Find a generic-concept OPERATOR whose body was dropped: every `ret` in it
returns a CONSTANT.

★★★The defect this refutes is SILENT and it answers ZERO.  Measured 2026-08-29:
`Slice[char]`'s `operator []` came out of the STDLIB root as

    define linkonce_odr i8 @_ZN5SliceIcEixEm(ptr %0, i64 %1) {
    entry:  ... the bounds check, correctly ...
    if.end: ret i8 0                      <-- `*(data + index)` is GONE
    }

so `hashing.hash` XOR'd a zero for every byte and `String.hash()` degenerated
to a LENGTH-ONLY hash: two 18-byte files with different content hashed
identically.  Nothing is reported, at any stage.

★★★WHAT TRIGGERS IT (measured, and it is NOT what the earlier record said):
the operator's FIRST use sits in a SUB-MODULE.  The identical call moved into
`containers.scaly` -- the file that DECLARES the modules -- emits a correct
body.  Three things the record blamed are refuted by experiment:
  * NOT the archive.  The raw `-S` output is already wrong, before
    `linkonce_odr`->`weak_odr` and before `opt -O2`.
  * NOT `const_char`, and not the element type at all.  `Slice[i64]` breaks
    the same way, and `Slice[u8]`/`Slice[int]` are correct only because their
    only subscript callers are the test functions in `containers.scaly`.
  * NOT module ORDER.  Moving `module hashing` after `module Slice` changes
    nothing.
★The STATEMENTS of the body are emitted (the bounds check is all there, with
the right instantiated types, and it reads field 1 `length` correctly).  Only
the RESULT expression is dropped -- explicit `return` included -- and with it
the access to field 0 `data`, the one field whose type names the generic
parameter.

★A program ROOT is unaffected: every isolated probe passes there, which is
exactly why this hid behind "the archive builds it differently".

Usage: tools/opzero/scan.py <root.ll>...
Emit a root with `-S --no-prelude --no-tests` and hand it here.  A finding is
a hard defect; there is no benign shape that matches.
"""
import re, sys

OPERATOR = re.compile(r'(ix|ps|mi|ml|dv|eq|ne|lt|gt)E[a-z0-9]*$')
CONSTANT_RET = re.compile(r'ret \w+ (0|null|zeroinitializer|false)$')

def scan(path):
    cur, body, bad, total = None, [], [], 0
    for line in open(path, encoding='utf-8', errors='replace'):
        line = line.rstrip('\n')
        if line.startswith('define'):
            m = re.search(r'@"?([^"(]+)"?\s*\(', line)
            cur, body = (m.group(1) if m else None), []
        elif line == '}' and cur:
            # a generic instantiation ('I' in the name) of an operator
            if OPERATOR.search(cur) and 'I' in cur:
                total += 1
                rets = [x.strip() for x in body if x.strip().startswith('ret ')]
                if rets and all(CONSTANT_RET.fullmatch(r) for r in rets):
                    bad.append(cur)
            cur = None
        elif cur is not None:
            body.append(line)
    return total, bad

rc = 0
for p in sys.argv[1:]:
    total, bad = scan(p)
    print(f"{p.split('/')[-1]:24s} generic operators {total:4d}   BODY DROPPED {len(bad)}")
    for b in bad:
        print("     ", b)
    if bad: rc = 1
sys.exit(rc)
