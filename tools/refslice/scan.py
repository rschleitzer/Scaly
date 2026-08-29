#!/usr/bin/env python3
"""Find BUFFER parameters that already carry their own LENGTH -- the slice of
the buffer-arithmetic campaign that is mechanizable at all.

The campaign's binding decision is that a buffer parameter becomes `Slice[T]`
by value.  READ THE COST NOTE AT THE BOTTOM BEFORE CONVERTING ANYTHING.  A `Slice` is {data, length}, so the conversion is only defined where
a length is IN HAND.  A `pointer[T]` parameter with no length beside it names a
buffer whose extent the callee learns some other way -- a NUL terminator, a
field, a sibling call -- and for those the length has to be found by reading,
not by a tool.  So this scan reports exactly three verdicts:

  ADJACENT the length parameter IMMEDIATELY follows the buffer -- the `(buf, n)`
           convention.  The strongest form, and a candidate.

  NAMED    the length shares the buffer's STEM (`min_buf`/`min_len`,
           `set_codes`/`set_n`).  Equally strong, and it is the form adjacency
           cannot see: parallel buffers over one length (`decode_key_args(keys,
           pos, n_keys)`) put the length after the LAST buffer.

  LOOSE    some length-shaped parameter exists elsewhere in the list, bound to
           this buffer by nothing.  NOT a candidate -- this is where a
           name-keyed guess answers about the wrong name: `insert(slots,
           name_len)` measures the NAME, not the slot table, and
           `encode_utf8_bytes(chars, out, len)` measures the INPUT, not the
           output buffer.  The reader decides; the tool must not.

  WRITTEN  the body STORES through the name (`set *(out + i): v`).  Not a
           candidate: `Slice[T]` is a READ-ONLY view -- its `operator []`
           answers T by value and it has no `put` -- so an output buffer has no
           Slice spelling at all.  ★This is not a nicety: `Recognizer.zero_bytes`
           and the `Message.put`/`ParserState.put_char` family all score
           ADJACENT on their `(out, cap)` pair and would have been converted
           into something that cannot compile, or worse, into a subscript store
           the compiler now rejects outright (rc 4).

  LENLESS  a buffer parameter with no length in the signature.  Not a
           candidate; it is the reading work the campaign says is not
           mechanical.

  UNWALKED this body shows no arithmetic and no indexing on the name.  That is
           the ABSENCE of evidence, never evidence of a cell -- refout's
           forwarder fixpoint calls many of these BUFFER, and this tool runs no
           fixpoint on purpose (see below).  Never a candidate either way.

★★★A verdict here is NECESSARY, NEVER SUFFICIENT: the length must measure the
buffer in ELEMENTS, and this tool cannot tell what a length counts.
`make_translate(desc: pointer[u32], n_pairs: size_t)` scores ADJACENT and is a
TRAP -- the body reads `*(desc + p*2)` and `*(desc + p*2 + 1)`, so the slice is
`n_pairs * 2` long and the naive conversion would halve it and trap the last
pair at exit 15.  Read the body's INDEX EXPRESSIONS before converting: an index
that is not the bare loop variable means the length is not the element count.

Evidence is the same as refout's: a BUFFER proves itself by ARITHMETIC or
INDEXING on the name.  This tool does NOT run a forwarder fixpoint -- a
forwarder has no length of its own to pair, so it cannot be a PAIRED candidate
whatever its callee says.

Exclusions this tool owes:
  * an `extern` declaration -- the C boundary, accepted shape 1.
  * a routine whose ITANIUM MANGLED NAME is spelled out as a "_Z..." literal:
    a Slice parameter re-signs the symbol, which is an undefined symbol at
    link time.  Derived from the tree, never hardcoded.
  * a `pointer[void]`/`cstring` pointee -- not an element type.
"""
import re, os, sys, collections

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'refout'))
from scan import collect, split_args, signature, strip_comment, DECL, PEELED

import importlib.util as _ilu
_spec = _ilu.spec_from_file_location('refcell_scan', os.path.join(
    os.path.dirname(os.path.abspath(__file__)), '..', 'refcell', 'scan.py'))
_rc = _ilu.module_from_spec(_spec); _spec.loader.exec_module(_rc)
frozen_names = _rc.frozen_names

NON_ELEMENT = ('void', 'cstring')

# a parameter whose NAME reads as a count of something
# ★`cap`/`capacity` measure an OUTPUT buffer and are length-shaped names like
# any other -- leaving them out put `encode_utf8_bytes(chars, len, out, cap)`'s
# `out` in the LOOSE bucket, where it read as an unpairable buffer while its
# length sat right beside it.
LEN_NAME = re.compile(r'^(n|len|length|count|size|num|nchars|cnt|cap|capacity)$|'
                      r'^(n|num)_\w+$|\w+_(len|length|count|size|n|cap|capacity)$', re.I)
LEN_TYPE = ('size_t', 'int', 'u32', 'u64', 'i64', 'u16')

def pkg_of(path):
    parts = path.split(os.sep)
    return parts[parts.index('packages') + 1] if 'packages' in parts else '?'

STEM_SUFFIX = re.compile(r'_(buf|buffer|codes|chars|bytes|data|ptr|p|s|len|length|count|size|n)$')
STEM_PREFIX = re.compile(r'^(n|num|len|size|count)_')

def stem(name):
    return STEM_SUFFIX.sub('', STEM_PREFIX.sub('', name))

def shares_stem(buf, ln):
    """A length bound to THIS buffer by a shared name stem, not by position."""
    a, b = stem(buf), stem(ln)
    return bool(a) and a == b

def writes(body, name):
    """Does the body STORE through this name? Then it is an OUTPUT buffer and
    `Slice[T]`, a read-only view, cannot express it."""
    n = re.escape(name)
    return bool(re.search(rf'\bset\s+\*\(\s*{n}\s*[+\-]', body)
                or re.search(rf'\bset\s+\*{n}\b', body)
                or re.search(rf'\bset\s+{n}\s*\[', body))

def walks(body, name):
    """Does the body prove this name is a buffer -- arithmetic or indexing?"""
    n = re.escape(name)
    return bool(re.search(rf'\b{n}\s*\+', body) or re.search(rf'\b{n}\s*-\s*\w', body)
                or re.search(rf'\b{n}\s*\[', body))

def main(paths):
    routines = collect(paths)
    frozen = frozen_names('.')
    rows = []
    for r in routines:
        params = r['params']
        for pi, (pn, pt) in enumerate(params):
            m = re.match(r'pointer\[\s*(\w+)\s*\]$', pt.strip())
            if not m: continue
            elem = m.group(1)
            if elem in NON_ELEMENT: continue
            if not walks(r['body'], pn):
                rows.append(('UNWALKED', r, pn, elem, None)); continue
            if writes(r['body'], pn):
                rows.append(('WRITTEN', r, pn, elem, None)); continue
            if r['fn'] in frozen:
                rows.append(('FROZEN', r, pn, elem, None)); continue
            lens = [(qi, qn) for qi, (qn, qt) in enumerate(params)
                    if qi != pi and qn not in PEELED
                    and LEN_NAME.match(qn) and qt.strip() in LEN_TYPE]
            verdict, ln = 'LENLESS', None
            for qi, qn in lens:
                if qi == pi + 1:
                    verdict, ln = 'ADJACENT', qn; break
            if verdict == 'LENLESS':
                for qi, qn in lens:
                    if shares_stem(pn, qn):
                        verdict, ln = 'NAMED', qn; break
            if verdict == 'LENLESS' and lens:
                verdict, ln = 'LOOSE', lens[0][1]
            rows.append((verdict, r, pn, elem, ln))
    return rows

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('-')]
    verbose = '-v' in sys.argv
    rows = main(args or ['packages'])
    tally = collections.Counter()
    for v, r, pn, elem, ln in rows:
        tally[(v, pkg_of(r['file']))] += 1
        if verbose and v in ('ADJACENT', 'NAMED'):
            print(f"{v:8s} pointer[{elem}] {r['file']}:{r['line']} {r['fn']}({pn}, len={ln})")
    print('---')
    for (v, pkg), n in sorted(tally.items(), key=lambda kv: (-kv[1], kv[0])):
        print(f"{n:5d}  {pkg:8s} {v}")
    print(f"--- {sum(tally.values())} buffer-shaped pointer parameters")

# ---------------------------------------------------------------------------
# COST, MEASURED 2026-08-29 -- read before converting a hot candidate.
#
# `Slice[T]` by value does NOT arrive in registers.  Scaly lowers every
# by-value struct parameter INDIRECTLY, so the emitted signature is
#
#     define i64 @_Z9sum_slice5SliceIiE(ptr %0)          ; one pointer
#     define i64 @_Z7sum_ptrPim(ptr %0, i64 %1)          ; two registers
#
# and the call site materialises a 16-byte stack temp (`add x0, sp, #16`
# against the pointer form's `mov x0, x22 / mov x1, x20`).  Under AAPCS64 a
# 16-byte composite is a REGISTER-PAIR argument, so this is an emitter
# convention, not an ABI necessity.
#
# Where the callee INLINES the cost is zero -- SROA dissolves the struct, and
# an A/B over 192M element reads measured 0.03 s both ways.  Where it does NOT
# inline the cost is real: a deliberately un-inlinable callee over 20M calls
# measured 0.07 s (pointer) against 0.08 s (slice), reproducibly.
#
# That matters here because the ADJACENT candidates ARE the un-inlinable hot
# paths: `CodingSystem.decode_one(input, n)` runs per character and
# `*FOTBuilder.characters(s, n)` per text run.
# ---------------------------------------------------------------------------
