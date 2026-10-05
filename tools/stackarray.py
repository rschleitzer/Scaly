#!/usr/bin/env python3
"""Walk a FIXED-SIZE STACK ARRAY by SUBSCRIPT instead of through a pointer cast:

    var modes int[28]                     var modes int[28]
    let mp modes as pointer[int]
    ... *(mp + i) ...             ->      ... modes[i] ...
    ... set *(mp + i): v ...              ... set modes[i]: v ...

A `var x T[N]` local is an `[N x T]` VALUE, and the rule for the
const-array form says to index it IN PLACE -- it cannot be passed to a
`pointer[T]` parameter.  The ports learned the second half of that rule and
not the first, so they cast the array to a pointer and walk it, and 312 of the
tree's remaining raw `*(p + i)` sites are that one idiom.  Both halves are
PROBED, compiled and run:

    var modes int[8]
    let mp modes as pointer[int]
    set *(mp + 3): 42        modes[3] -> 42     (the two alias, as expected)
    set modes[2]: 7          modes[2] ->  7     (the direct write works)

★★★AND THE WRITE IS `set base[i]: v` HERE, WHICH IS THE OPPOSITE OF THE
CONTAINER RULE.  On an `Array`/`Vector`/`Slice` a subscript answers T BY VALUE,
there is no slot behind it, and `set a[i]: v` is a hard rc-4
(report_subscript_assignment#) -- the write is `put`.  On a fixed array (and on
a pointer) the subscript is a GEP, the store is real, and `put` does not exist.
So the same source line means different things by receiver, and a tool that
learns one rule and applies it to the other receiver either drops the store or
fails to compile.

★★★NO `Slice` HERE, DELIBERATELY.  `Slice[T](n, base as pointer[T])` would be
a receiver CONSTRUCTED IN THE CALLING ROUTINE, which is exactly the shape that
empties a generic concept's `operator []` (tools/opzero/scan.py) -- the
subscript would answer 0 with nothing said. The array's own subscript has no
such path, and it removes the cast as well as the arithmetic.

★The element type must MATCH: `u8[16] as pointer[LLVMValueRef]` is a
reinterpreting cast over a scratch buffer, where the array's own subscript has
the wrong stride. 17 such sites are skipped by that test alone.

★The binding is dropped only when NO use of the local survives -- a cast local
handed to a `pointer[T]` callee is why the cast exists at all, and that use
cannot be spelled with the array.

Usage: tools/stackarray.py [--apply] <file.scaly>...
"""
import re, sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from subscript_local import routine_spans, strip_comment, deref_spans, other_use
from subscript import set_source_start

CAST = re.compile(r'^(\s*)(?:let|var)\s+([A-Za-z_][\w\']*)\s+'
                  r'([A-Za-z_][\w\']*)\s+as\s+pointer\[\s*(\w+)\s*\]\s*$')
ARR = re.compile(r'^\s*(?:let|var)\s+([A-Za-z_][\w\']*)\s+(\w+)\['
                 r'(?:\d+|[A-Z_][A-Z0-9_]*)\]\s*$')

def rewrite(text, local, base):
    out = text
    for s, e, op, ix in reversed(deref_spans(strip_comment(text), local)):
        out = out[:s] + f'{base}[{ix}]' + out[e:]
    return out

def convert(lines):
    changed = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        arrs = {}
        for l in codes:
            m = ARR.match(l)
            if m: arrs[m.group(1)] = m.group(2)
        for k in range(len(body)):
            m = CAST.match(codes[k])
            if not m: continue
            indent, local, base, elem = m.groups()
            if arrs.get(base) != elem: continue
            hits, ok = [], True
            for j in range(k + 1, len(body)):
                spans = deref_spans(codes[j], local)
                if any(op == '-' for _, _, op, _ in spans): ok = False; break
                if spans: hits.append(j)
            if not ok or not hits: continue
            for j in hits:
                line = body[j]
                src = set_source_start(line)
                if src and line.lstrip().startswith('set '):
                    # ★The TARGET half converts too -- on a fixed array the
                    # subscript is a real store. Both halves, one rule.
                    n = len(deref_spans(strip_comment(line), local))
                    body[j] = rewrite(line, local, base)
                else:
                    n = len(deref_spans(strip_comment(line), local))
                    body[j] = rewrite(line, local, base)
                changed += n
            codes = [strip_comment(l) if l is not None else '' for l in body]
            if not any(re.search(rf'\b{re.escape(local)}\b', codes[j])
                       for j in range(len(body)) if j != k):
                body[k] = None
        lines[a:b] = body
    # ★★★THE DROPPED BINDINGS COME OUT AT THE END, NOT IN THE LOOP. The routine
    # spans are computed once, up front; deleting a line inside the loop shifts
    # every later span by one and the pass then rewrites the WRONG ROUTINE's
    # code. Measured: `set *(fa + 0): 0` in `sd_fill_features`, where `fa` is a
    # PARAMETER, became `set feat_args[0]: 0` -- a name from the CALLER 180
    # lines earlier. The compiler caught it (`unknown assignment target`), but
    # only because that name happened not to exist there.
    return [l for l in lines if l is not None], changed

def main():
    apply = '--apply' in sys.argv
    total = 0
    for p in [x for x in sys.argv[1:] if not x.startswith('--')]:
        lines = open(p, encoding='utf8').read().split('\n')
        new, c = convert(list(lines))
        if c:
            print('%5d  %s' % (c, p))
            if apply: open(p, 'w', encoding='utf8').write('\n'.join(new))
        total += c
    print('TOTAL:', total, '(applied)' if apply else '(counted only)')

if __name__ == '__main__':
    main()
