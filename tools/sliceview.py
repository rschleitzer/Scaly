#!/usr/bin/env python3
"""Turn a hoisted RAW BUFFER local into a borrowed SLICE VIEW:

    let b container.get_buffer()          let b container.as_slice()
    ... *(b + i) ...              ->      ... b[i] ...
    ... set *(b + i): v ...               ... b.put(i, v) ...

This is the half of the hoisted-buffer class that `tools/subscript_local.py`
must refuse, and it is the LARGER half.  That tool rewrites the walk into a
subscript ON THE CONTAINER (`container[i]`), which is only defined when the
container has an `operator []` and only correct when the container has not
moved its buffer in between.  Both conditions fail often:

  * ★★★`String` HAS `get_buffer()` AND NO `operator []`.  The ports walk
    String bytes constantly, and every one of those sites came back from the
    compiler as `unexpected operand - missing operator` (`prog[pi]`,
    `lo.name[t]`, `t4[3]`) or, at emit time and with NO LINE NUMBER, as
    `member not found: String.put`.
  * ★★★AND ON A `String` THE CONTAINER SUBSCRIPT WOULD BE A PESSIMISATION EVEN
    IF IT EXISTED: the length is a VARINT PREFIX, so `get_buffer()` decodes it
    by walking bytes -- the hoist is why the ports wrote it this way.  A
    per-element `s[i]` would re-decode per element; `as_slice()` decodes ONCE
    and hands out {length, data}.
  * a receiver that GROWS between the binding and the walk moves its buffer,
    so `container[i]` and the hoisted pointer genuinely differ.  A SLICE does
    not have that problem: it captures {length, data} at the same moment the
    pointer did, so the rewrite is semantics-preserving by construction and
    needs no mutation guard at all.
  * a `pointer[Array[T]]` receiver, where `container[i]` is pointer arithmetic
    at Array stride (silently wrong) while `container.as_slice()` auto-derefs
    through the member access and is right.

★`as_slice()` already exists on `String`, `Array` and `Vector` -- no stdlib
change and no seed is owed for this.

★★★THE GUARD THAT IS LOAD-BEARING is the one CLAUDE.md lists as ACTIVE: a
`Slice[T]` is ACCEPTED where a `pointer[T]` is declared, silently, and
arithmetic on one compiles.  So EVERY use of the local must be a deref of the
form `*(b + i)` or `set *(b + i): v`.  A local also passed to a callee, bound
into an element address (`let e b + k`), walked backwards (`*(b - 1)`) or
dereferenced bare (`*b`) is left alone -- converting it would hand a slice
STRUCT to a pointer parameter with nothing said at any stage.

Usage: tools/sliceview.py [--apply] <file.scaly>...
"""
import re, sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from subscript_local import (BIND, routine_spans, strip_comment, deref_spans,
                             other_use)
from subscript import set_source_start

def rewrite_reads(text, local):
    out = text
    for s, e, op, ix in reversed(deref_spans(strip_comment(text), local)):
        out = out[:s] + f'{local}[{ix}]' + out[e:]
    return out


def convert(lines):
    changed = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k in range(len(body)):
            m = BIND.match(codes[k])
            if not m: continue
            indent, local, recv = m.group(1), m.group(2), m.group(3)
            hits, ok = [], True
            for j in range(k + 1, len(body)):
                code = codes[j]
                if not re.search(rf'\b{re.escape(local)}\b', code): continue
                spans = deref_spans(code, local)
                if not spans or any(op == '-' for _, _, _, _ in
                                    [s for s in spans if s[2] == '-']):
                    ok = False; break
                if other_use(code, local, spans): ok = False; break
                hits.append((j, spans))
            if not ok or not hits: continue
            # every write must be a WHOLE-statement `set *(b + i): v`
            plan = []
            for j, spans in hits:
                line = body[j]
                src = set_source_start(line)
                if src and line.lstrip().startswith('set '):
                    tgt = line[:src - 1]
                    tspans = deref_spans(strip_comment(tgt), local)
                    if tspans:
                        if len(tspans) != 1 or \
                           tgt.strip()[4:].strip() != tgt[tspans[0][0]:tspans[0][1]].strip():
                            ok = False; break
                        # ★★★THE SOURCE HALF DEREFS THE SAME LOCAL, and
                        # leaving it raw is the silent trap this file's own
                        # header warns about: `set *(oec + ti): *(oec + ti) + 1`
                        # became `oec.put(ti, *(oec + ti) + 1)`, which does
                        # POINTER ARITHMETIC ON THE SLICE STRUCT. It compiled,
                        # the SGML corpus stayed at 380/380, and only
                        # tests/opensp caught it -- ContentState's element
                        # counters read garbage. The value is rewritten with
                        # the same rule as any read.
                        val = rewrite_reads(line[src:].strip(), local)
                        plan.append((j, 'put', tspans[0][3], val,
                                     len(line) - len(line.lstrip())))
                        continue
                plan.append((j, 'read', None, None, src))
            if not ok: continue
            for j, kind, idx, val, extra in plan:
                line = body[j]
                if kind == 'put':
                    body[j] = f'{" " * extra}{local}.put({idx}, {val})'
                    changed += 1
                    continue
                new = line
                for s, e, op, ix in reversed(deref_spans(strip_comment(line), local)):
                    if s < extra: continue
                    new = new[:s] + f'{local}[{ix}]' + new[e:]
                    changed += 1
                body[j] = new
            body[k] = f'{indent}let {local} {recv}.as_slice()'
            # ★Post-condition, in the tool rather than in a reviewer's eye: a
            # surviving `*(local + i)` after the binding became a Slice is a
            # silent wrong read, so the conversion is BACKED OUT if one is
            # left. `tools/refslice/convert.py` earned this rule; this file
            # had to earn it again.
            for j in range(k + 1, len(body)):
                if deref_spans(strip_comment(body[j]), local):
                    raise SystemExit(
                        f'sliceview: raw deref of {local!r} survives the '
                        f'conversion -- refusing: {body[j].strip()!r}')
        lines[a:b] = body
    return lines, changed

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
    print('SUMME:', total, '(angewendet)' if apply else '(nur gezählt)')

if __name__ == '__main__':
    main()
