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

★★★THE GUARD THAT IS LOAD-BEARING is the one listed as ACTIVE: a
`Slice[T]` is ACCEPTED where a `pointer[T]` is declared, silently, and
arithmetic on one compiles.  So EVERY use of the local must be a deref of the
form `*(b + i)` or `set *(b + i): v`.  A local also passed to a callee, bound
into an element address (`let e b + k`), walked backwards (`*(b - 1)`) or
dereferenced bare (`*b`) is left alone -- converting it would hand a slice
STRUCT to a pointer parameter with nothing said at any stage.


★★★THE SECOND MODE HOLDS THE VIEW ONE LEVEL UP, and it exists because
`tools/subscript_driver.py` says in so many words that this file is the answer
for a `String` receiver "but it does not claim them by itself".  The
driver reverted 28 of 29 proposals on 2026-09-04 for one structural reason --
the ports walk STRING bytes and `String` has NO `operator []` -- and every one
of those reverted sites is this shape:

    let pn this.prog_name                 let pn this.prog_name.as_slice()
    while pi < pn.get_length()      ->    while pi < pn.length
        ... *(pn.get_buffer() + pi) ...       ... pn[pi] ...

The local is bound to the CONTAINER, not to its buffer, so the first mode
(which keys on `let b <recv>.get_buffer()`) never saw it, and `subscript.py`
proposed `pn[pi]` on the String itself, which does not exist.  The view is
taken ONCE at the binding -- on a `String` that matters beyond taste, because
`get_buffer()` decodes the varint length prefix by walking bytes and every
inline site re-walked it.

The guards are the first mode's: every use of the local must be a walk
`*(x.get_buffer() + i)` or a `x.get_length()`, a MINUS offset refuses, a walk
in the TARGET half of a `set` refuses (a `String` has no `put`, and the
container half is `Slice.put`, which the first mode already writes), and the
RHS must be a plain name/member chain or one call -- `let bl b as ref[Array[
ref[Type]?]]` would otherwise get `.as_slice()` appended INSIDE the cast.

Usage: tools/sliceview.py [--apply] <file.scaly>...
"""
import re, sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from subscript_local import (BIND, routine_spans, strip_comment, deref_spans,
                             other_use, REBIND)
from subscript import set_source_start, balanced_index

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
            # ★★★STOP AT A REBINDING OF THE SAME NAME -- `tools/subscript_local.py`
            # carries the rule and this file was written without it, which made
            # the tool REFUSE ITS OWN EARLIER WORK: two `when` arms of
            # `Modeler.handle_literal` each hoist `let buf <lit>.value` and walk
            # it, the hex arm was converted first, and its `let buf ...as_slice()`
            # then read as a non-deref USE of the integer arm's `buf` -- so the
            # integer arm was reported unconvertible for as long as its sibling
            # was converted. A scan that runs past a rebinding also answers
            # about the WRONG receiver; the sibling tool records that measurement.
            end = len(body)
            for j in range(k + 1, len(body)):
                if REBIND(local).match(codes[j]): end = j; break
            hits, ok = [], True
            for j in range(k + 1, end):
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
            for j in range(k + 1, end):
                if deref_spans(strip_comment(body[j]), local):
                    raise SystemExit(
                        f'sliceview: raw deref of {local!r} survives the '
                        f'conversion -- refusing: {body[j].strip()!r}')
        lines[a:b] = body
    return lines, changed


# ---------------------------------------------------------------- second mode
LET = re.compile(r"^(\s*)((?:let|var)\s+([A-Za-z_][A-Za-z0-9_']*)\s+)(.+?)\s*$")
# a name/member chain, optionally ONE trailing call -- nothing with an `as`,
# an operator or a sigil, where appending `.as_slice()` would rebind the dot.
PLAIN = re.compile(r"^[A-Za-z_][A-Za-z0-9_.']*(\(.*\))?$")


def view_spans(code, name):
    """Every `*( name.get_buffer() [<+> IDX] )`: (start, end, index).

    None when the walk is BACKWARDS -- `*(x.get_buffer() - 1)` has no subscript
    reading, the same refusal the first mode makes.
    """
    out, pos = [], 0
    n = re.escape(name)
    head = re.compile(rf'\*\(\s*{n}\.get_buffer\s*\(\s*\)\s*')
    while True:
        m = head.search(code, pos)
        if not m: return out
        rest = code[m.end():]
        if rest.startswith(')'):
            out.append((m.start(), m.end() + 1, '0')); pos = m.end() + 1; continue
        mo = re.match(r'([+\-])\s*', rest)
        if not mo: pos = m.end(); continue
        if mo.group(1) == '-': return None
        inner = balanced_index(code, m.end() + mo.end())
        if inner is None: pos = m.end(); continue
        idx, after = inner
        out.append((m.start(), after, idx.strip())); pos = after


def rewrite_view(line, local):
    """The walks become subscripts and `get_length()` becomes the field."""
    spans = view_spans(strip_comment(line), local)
    out = line
    for s, e, ix in reversed(spans):
        out = out[:s] + f'{local}[{ix}]' + out[e:]
    return re.sub(rf'\b{re.escape(local)}\.get_length\s*\(\s*\)',
                  f'{local}.length', out)


def convert_view(lines):
    changed = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k in range(len(body)):
            m = LET.match(codes[k])
            if not m: continue
            indent, decl, local, rhs = m.groups()
            if not PLAIN.match(rhs): continue
            if rhs.endswith('.get_buffer()') or rhs.endswith('.as_slice()'): continue
            end = len(body)
            for j in range(k + 1, len(body)):
                if REBIND(local).match(codes[j]): end = j; break
            hits, ok, walks = [], True, 0
            for j in range(k + 1, end):
                code = codes[j]
                if not re.search(rf'\b{re.escape(local)}\b', code): continue
                spans = view_spans(code, local)
                if spans is None: ok = False; break
                # ★A walk in the TARGET half of a `set` is a WRITE, and this
                # mode has no write form: the receiver is typically a `String`,
                # which carries no `put` at all. The first mode writes
                # `Slice.put` for a container; here the site is left alone.
                src = set_source_start(code)
                if src and any(s < src - 1 for s, _, _ in spans): ok = False; break
                rest = code
                for s, e, _ in reversed(spans): rest = rest[:s] + ' ' * (e - s) + rest[e:]
                rest = re.sub(rf'\b{re.escape(local)}\.get_length\s*\(\s*\)', ' ', rest)
                if re.search(rf'\b{re.escape(local)}\b', rest): ok = False; break
                walks += len(spans)
                hits.append(j)
            if not ok or not walks: continue
            for j in hits:
                body[j] = rewrite_view(body[j], local)
            body[k] = f'{indent}{decl}{rhs}.as_slice()'
            changed += walks
            codes = [strip_comment(l) for l in body]
        lines[a:b] = body
    return lines, changed


def main():
    apply = '--apply' in sys.argv
    total = 0
    for p in [x for x in sys.argv[1:] if not x.startswith('--')]:
        lines = open(p, encoding='utf8').read().split('\n')
        new, c = convert(list(lines))
        new, c2 = convert_view(new)
        c += c2
        if c:
            print('%5d  %s' % (c, p))
            if apply: open(p, 'w', encoding='utf8').write('\n'.join(new))
        total += c
    print('TOTAL:', total, '(applied)' if apply else '(counted only)')

if __name__ == '__main__':
    main()
