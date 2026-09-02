#!/usr/bin/env python3
"""Rewrite a buffer walk that goes through a LOCAL BINDING:

    let b container.get_buffer()
    ... *(b + i) ...            ->  container[i]
    ... set *(b + i): v ...     ->  container.put(i, v)

`tools/subscript.py` rewrites the INLINE form `*(X.get_buffer() + i)` and
nothing else, so the moment a port hoisted the call into a local -- which is
what a reader does the second time a loop body needs the buffer -- the site
left every scan's population.  It is the single largest class of raw
`*(p + i)` left in the tree (271 of 2248 sites on 2026-08-30, more than the
inline form ever was), and it is the same rewrite.

★★★THE WRITE IS `put`, NEVER `set container[i]: v`.  `operator []` answers T
BY VALUE, so there is no slot behind a subscript: `set a[i]: v` on a container
is a hard rc-4 since 2026-08-29 (report_subscript_assignment#) and was a
SILENT drop before that.  This tool converts the read and the write in ONE
pass and must convert the WRITE FIRST, because the read rewrite would
otherwise turn `set *(b + i): v` into `set container[i]: v` -- exactly the
mistake `tools/refslice/convert.py` records.

★★★AND A STALE-BUFFER GUARD IS LOAD-BEARING, which the inline form does not
need at all.  `let b c.get_buffer()` captures the buffer ADDRESS; `c[i]` reads
whatever buffer `c` holds AT THE READ.  Those differ the moment `c` grows
between the binding and the use -- an `Array` reallocates -- so the hoisted
local is not merely a spelling of the subscript there, and the rewrite would
paper over (or introduce) a difference the suites might not show.  Any routine
that calls a MUTATOR on the receiver is skipped whole.

★The other guards are `tools/subscript.py`'s, and they are needed for the same
reasons: a receiver segment declared `pointer[...]` anywhere in the file is
skipped (on a `pointer[Array[T]]` the subscript is pointer arithmetic with an
Array-sized stride), and the compiler is the arbiter -- run
`tools/subscript_driver.py` afterwards, which reverts by LINE what the
compiler rejects.

Usage: tools/subscript_local.py [--apply] <file.scaly>...
"""
import re, sys

sys.path.insert(0, __file__.rsplit('/', 1)[0])
from subscript import balanced_index, pointer_names, set_source_start, RECV

BIND = re.compile(r'^(\s*)(?:let|var)\s+([A-Za-z_][A-Za-z0-9_\']*)\s+'
                  r'([A-Za-z_][A-Za-z0-9_.\']*)\.get_buffer\s*\(\s*\)\s*$')

# a call that can move the receiver's buffer, or replace the receiver itself
MUTATOR = re.compile(r'\.(add|add_all|remove|reallocate|clear|resize|reserve)\b')

HEAD = re.compile(r'^(\s*)(function|procedure|operator|init)\b')

def strip_comment(line):
    out, i, instr = [], 0, None
    while i < len(line):
        c = line[i]
        if instr:
            if c == '\\': out.append(line[i:i+2]); i += 2; continue
            if c == instr: instr = None
            out.append(c); i += 1; continue
        if c in '"\'': instr = c; out.append(c); i += 1; continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

def routine_spans(lines):
    heads = [i for i, l in enumerate(lines) if HEAD.match(strip_comment(l))]
    heads.append(len(lines))
    return [(heads[i], heads[i + 1]) for i in range(len(heads) - 1)]

def deref_spans(code, name):
    """Every `*( name <+|-> IDX )` in this line: (start, end, op, index)."""
    out, pos = [], 0
    n = re.escape(name)
    while True:
        m = re.compile(rf'\*\(\s*{n}\s*([+\-])\s*').search(code, pos)
        if not m: return out
        inner = balanced_index(code, m.end())
        if inner is None: pos = m.end(); continue
        idx, after = inner
        out.append((m.start(), after, m.group(1), idx.strip()))
        pos = after

def other_use(code, name, spans):
    """Does the name occur outside the deref spans on this line?"""
    for m in re.finditer(rf'\b{re.escape(name)}\b', code):
        if not any(a <= m.start() < b for a, b, _, _ in spans):
            return True
    return False

def REBIND(name):
    """A later `let`/`var` of the same name ENDS the binding under scan."""
    return re.compile(rf'^\s*(?:let|var)\s+{re.escape(name)}\b')


def convert(lines, skip, keep_bindings=False):
    changed, dropped = 0, 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k, l in enumerate(body):
            m = BIND.match(strip_comment(l))
            if not m: continue
            indent, local, recv = m.group(1), m.group(2), m.group(3)
            if any(seg in skip for seg in recv.split('.')): continue
            # ★Stale-buffer guard, scoped to THIS receiver: `let b c.get_buffer()`
            # captures the buffer ADDRESS, `c[i]` reads whatever buffer `c` holds
            # AT THE READ, and an Array that grows in between has moved. Asking
            # the whole ROUTINE for any mutator answers about the wrong
            # container -- an unrelated `sb.add` in the same body is not a
            # reason to keep a pointer walk. Asked of the receiver and of its
            # HEAD segment, because `this.docs.add` moves `this.docs`' buffer
            # and so does anything that re-seats `this.docs` itself.
            head = recv.split('.')[0]
            if any(re.search(rf'\b{re.escape(recv)}\s*\.\s*'
                             r'(add|add_all|remove|reallocate|clear|resize|reserve)\b', c)
                   or re.search(rf'\bset\s+{re.escape(recv)}\b', c)
                   or (head != recv and re.search(rf'\bset\s+{re.escape(head)}\b', c))
                   for c in codes):
                continue
            # every remaining use of the local, and whether all are derefs
            hits, ok, leftover = [], True, False
            for j in range(k + 1, len(body)):
                code = codes[j]
                # ★★★STOP AT A REBINDING OF THE SAME NAME.  Without this the
                # scan runs to the END OF THE ROUTINE and the FIRST binding
                # claims the sites of every later one, rewriting them with ITS
                # receiver.  Measured in dazzle's Expression.can_eval:
                # `when e: Call { let b e.args.get_buffer() ... }` followed by
                # `when e: Sequence { let b e.seq.get_buffer() ... }` turned
                # `*(b + i)` in the Sequence arm into `e.args[i]`.
                # ★It surfaced only because SequenceExpr HAS NO `args` FIELD.
                # Where both arms' concepts carry a same-named container the
                # rewrite COMPILES and reads the WRONG ARRAY -- silently.
                if REBIND(local).match(code): break
                if not re.search(rf'\b{re.escape(local)}\b', code): continue
                spans = deref_spans(code, local)
                # a MINUS-offset walk (`*(b - 1)`) has no subscript reading
                if any(op == '-' for _, _, op, _ in spans): ok = False; break
                if not spans: leftover = True; continue
                if other_use(code, local, spans): leftover = True
                hits.append((j, spans))
            if not ok or not hits: continue
            for j, spans in hits:
                line = body[j]
                src = set_source_start(line)
                stripped = line.lstrip()
                # WRITE first: a whole-line `set *(b + i): v` becomes put
                if src and stripped.startswith('set '):
                    tgt = line[:src - 1]
                    tspans = deref_spans(strip_comment(tgt), local)
                    if len(tspans) == 1 and tgt.strip() == \
                       tgt[tgt.index('set') + 3:][:0] + 'set ' + \
                       tgt.strip()[4:] and \
                       tgt.strip()[4:].strip() == tgt[tspans[0][0]:tspans[0][1]].strip():
                        idx = tspans[0][3]
                        val = line[src:].strip()
                        body[j] = f'{line[:len(line) - len(stripped)]}' \
                                  f'{recv}.put({idx}, {val})'
                        changed += 1
                        continue
                    if tspans:
                        ok = False; break
                # READ(s): rightmost first so offsets stay valid
                new = line
                for s, e, op, idx in reversed(deref_spans(strip_comment(line), local)):
                    if s < src: continue          # target half: never
                    new = new[:s] + f'{recv}[{idx}]' + new[e:]
                    changed += 1
                body[j] = new
            if ok and not leftover and not keep_bindings:
                body[k] = None; dropped += 1
        lines[a:b] = body
    return [l for l in lines if l is not None], changed, dropped

DEAD = re.compile(r'^\s*(?:let|var)\s+([A-Za-z_][A-Za-z0-9_\']*)\s+'
                  r'[A-Za-z_][A-Za-z0-9_.\']*\.get_buffer\s*\(\s*\)\s*$')

def prune(lines):
    """Phase 2: drop a `let b x.get_buffer()` whose local has no use left."""
    n = 0
    for a, b in routine_spans(lines):
        body = lines[a:b]
        codes = [strip_comment(l) for l in body]
        for k, l in enumerate(body):
            m = DEAD.match(codes[k])
            if not m: continue
            local = m.group(1)
            if any(re.search(rf'\b{re.escape(local)}\b', codes[j])
                   for j in range(len(body)) if j != k):
                continue
            body[k] = None; n += 1
        lines[a:b] = body
    return [l for l in lines if l is not None], n

def main():
    apply = '--apply' in sys.argv
    # ★Phase 1 keeps every binding so the file's LINE NUMBERING is unchanged:
    # tools/subscript_driver.py reverts what the compiler rejects BY LINE, and
    # a deleted line puts every later revert on the wrong statement. The dead
    # bindings come out in phase 2 (--prune), after the driver has converged.
    keep = '--keep-bindings' in sys.argv
    files = [x for x in sys.argv[1:] if not x.startswith('--')]
    tc = td = 0
    if '--prune' in sys.argv:
        t = 0
        for p in files:
            lines = open(p, encoding='utf8').read().split('\n')
            new, n = prune(list(lines))
            if n:
                print('%5d  %s' % (n, p))
                if apply: open(p, 'w', encoding='utf8').write('\n'.join(new))
            t += n
        print('SUMME:', t, 'dead bindings',
              '(angewendet)' if apply else '(nur gezählt)')
        return
    for p in files:
        text = open(p, encoding='utf8').read()
        lines = text.split('\n')
        new, c, d = convert(list(lines), pointer_names(text), keep)
        if c:
            print('%5d  %3d dropped  %s' % (c, d, p))
            if apply: open(p, 'w', encoding='utf8').write('\n'.join(new))
        tc += c; td += d
    print('SUMME:', tc, 'sites,', td, 'bindings dropped',
          '(angewendet)' if apply else '(nur gezählt)')

# ★Under `if __name__`, for the reason recorded in tools/subscript.py: a bare
# module-scope `main()` runs this tool with the IMPORTER's argv the moment
# tools/sliceview.py imports its helpers -- with `--apply` and the importer's
# file list.
if __name__ == '__main__':
    main()
