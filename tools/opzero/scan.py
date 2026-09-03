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

★★★WHAT TRIGGERS IT (measured 2026-08-30, and it is NOT what either earlier
record said): the operator's RECEIVER IS CONSTRUCTED IN THE CALLING ROUTINE.
Received as a PARAMETER the body is CORRECT; constructed there -- inline or
`let`-bound -- it is EMPTY.  A sub-module is NECESSARY BUT NOT SUFFICIENT, so
the earlier "the CALLER'S FILE decides" is withdrawn.

★★★AND IT IS NOT ABOUT `Slice`.  `Vector[char]`'s `operator []` empties in
exactly the same shape -- the bounds check intact, reading field 0 `length`,
then `ret i8 0`.  This is a property of the generic OPERATOR path; every
stdlib container carrying one is exposed, and the `Slice`-shaped framing sent
the reader hunting the wrong concept.

Also refuted by experiment:
  * NOT the archive.  The raw `-S` output is already wrong, before
    `linkonce_odr`->`weak_odr` and before `opt -O2`.
  * NOT `const_char`, and not the element type at all.  `char` and `i64`
    break alike.
  * NOT module ORDER.  Moving `module hashing` after `module Slice` changes
    nothing.
  * NOT a property of the concept's CONTENT.  A hand-written `Holder[T]` with
    the identical operator body is CORRECT in every configuration tried:
    constructed in its own declaring file, in another file of the same
    package, in another PACKAGE, and after pre-instantiation through a method.
    (`SliceCopy`, a textual rename of the whole Slice file, was correct too.)
    What survives every probe is only HOW THE REAL PRELUDE CONTAINER IS
    REACHED, and that is not root-caused.

★The STATEMENTS of the body are emitted (the bounds check is all there, with
the right instantiated types, and it reads the length field correctly).  Only
the RESULT expression is dropped -- explicit `return` included -- and with it
the access to the `data` field, the one whose type names the generic
parameter.

★★★THE SAFE SPELLING IS A METHOD.  `get(i)` and `put(i, v)` emit CORRECT
bodies in the very build where `[i]` is empty: an ordinary method is emitted
for EVERY instantiation, an operator only where USED.  On a locally
constructed generic container, prefer `get`/`put` over `[]` until this is
fixed.

★A program ROOT is unaffected: every isolated probe passes there, which is
exactly why this hid behind "the archive builds it differently".

Usage: tools/opzero/scan.py <root.ll>...
Emit a root with `-S --no-prelude --no-tests` and hand it here.  A finding is
a hard defect; there is no benign shape that matches.
"""
import re, sys

OPERATOR = re.compile(r'(ix|ps|mi|ml|dv|eq|ne|lt|gt)E[a-z0-9]*$')
# ★★★A FLOATING-POINT ZERO IS A CONSTANT RETURN TOO, and leaving it out made
# this scan answer BODY DROPPED 0 on a build where `Slice[double]`'s operator[]
# was demonstrably empty: LLVM prints it `ret double 0.000000e+00`, which the
# integer-only pattern did not match.  Measured 2026-09-03 while converting
# dazzle's CieData -- the probe answered 0 instead of 8 at rc 0 and this file
# called it clean.  A refuter that cannot fire is worse than none, and this one
# could not fire for any f32/f64 container in the tree.
CONSTANT_RET = re.compile(
    r'ret \w+ (0|null|zeroinitializer|false|'
    r'0\.0+e\+0+|0\.0+|-?0x0+(?:p\+0)?)$')

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
