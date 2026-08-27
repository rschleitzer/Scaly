#!/usr/bin/env python3
"""Sweep the pointer deref off a REFERENCE.

A `ref[T]` reads and writes as its pointee, so `*` on one is the pointer idiom
it replaces.  Two passes, verified differently:

  --store   `set *r: v` -> `set r: v`               byte-identical emission
  --read    the read positions the planner supports  byte-identical emission
  --chain   `(*r).m()` -> `r.m()`, `choose *r` -> `choose r`
                                                    emission CHANGES (a copy
                                                    goes away); gates, not bytes

★ SCOPED to the enclosing routine's non-optional `ref[T]` PARAMETERS and its
own `ref[T]`-annotated locals, and the first draft was not: a file-wide "names
declared `ref[` somewhere" set swept `set *a:` where `a` was a let-bound
POINTER from `host.allocate(...) as pointer[ElementType]`, because a different
routine in the same file has a parameter named `a: ref[...]`.  A `set` on a
let-bound pointer is silently dropped -- 811 lines out of opensp's root,
caught only by the byte-identity check.  A name-keyed set answers about the
wrong name.

★ A body that rebinds the name with its own `let`/`var` gives it up.

★ `ref[T]?` is never swept.  It is `Option[ref[T]]`, the language has no
transparent reading for an Option, and `*opt` stays the spelling.
"""
import re, os, sys, collections
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'refout'))
from scan import DECL, strip_comment, signature, split_params, body_lines, SCALARS

def peel(t):
    """The pointee of a non-optional `ref[T]`, else None."""
    t = t.strip()
    if not t.startswith('ref[') or not t.endswith(']'):
        return None
    return t[4:-1].strip()

def scoped_names(lines, i):
    """(names -> pointee) for one routine, plus the body's line range."""
    m = DECL.match(strip_comment(lines[i]))
    sig, end = signature(lines, i)
    if re.search(r'\bextern\b', sig):
        return None, end
    names = {}
    for prm in split_params(sig):
        pm = re.match(r'\s*(\w+)\s*:\s*(.+?)\s*$', prm)
        if pm:
            inner = peel(pm.group(2))
            if inner is not None: names[pm.group(1)] = inner
    body = body_lines(lines, end, m.group(1))
    for l in body:
        c = strip_comment(l)
        lm = re.match(r'\s*(?:let|var)\s+(\w+)\s*:\s*(\S.*?)\s*$', c)
        if lm:
            inner = peel(lm.group(2))
            if inner is not None: names[lm.group(1)] = inner
    for l in body:
        c = strip_comment(l)
        bm = re.match(r'\s*(?:let|var)\s+(\w+)\s', c)
        if bm and bm.group(1) in names and not re.match(r'\s*(?:let|var)\s+\w+\s*:\s*ref\[', c):
            del names[bm.group(1)]
    return names, end, len(body)

def rewrite(head, nm, pointee, mode):
    """Return the rewritten code half of a line, or None."""
    e = re.escape(nm)
    star = r'\(\s*\*%s\s*\)|\*%s\b' % (e, e)
    if mode == 'store':
        m = re.match(r'^(\s*set\s+)\*(%s)(\s*:)' % e, head)
        return m.group(1) + nm + m.group(3) + head[m.end():] if m else None
    if mode == 'read':
        # whole-line shapes, any pointee
        for pat, rep in (
            (r'^(\s*if\s+)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), r'\g<1>' + nm),
            (r'^(\s*return\s+)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), r'\g<1>' + nm),
            (r'^(\s*set\s+[\w.]+\s*:\s*)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), r'\g<1>' + nm),
            (r'^(\s*(?:let|var)\s+\w+\s*:\s*\S+\s+)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), r'\g<1>' + nm),
            (r'^(\s*)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), r'\g<1>' + nm),
        ):
            new, n = re.subn(pat, rep, head)
            if n: return new
        # an UNTYPED binding COPIES where the bare name would ALIAS, so it
        # gains the annotation rather than losing the star
        m = re.match(r'^(\s*(?:let|var)\s+)(\w+)(\s+)(?:\(\s*\*%s\s*\)|\*%s)\s*$' % (e, e), head)
        if m:
            return '%s%s: %s %s' % (m.group(1), m.group(2), pointee, nm)
        # anything else only for a SCALAR pointee: argument, comparison,
        # arithmetic -- the positions the planner loads at
        if pointee in SCALARS and not re.match(r'^\s*set\s+\*%s\s*:' % e, head):
            # ★ Strip the PARENS only where they are the author's grouping, not
            # a call's argument list: `contains(*ch)` and `(*ch)` differ by the
            # one character before the paren, and treating them alike rewrote
            # `sgset.contains(*ch)` into `sgset.containsch`.
            new, n = re.subn(r'(?<![\w)\]])\(\s*\*%s\s*\)' % e, nm, head)
            new, n2 = re.subn(r'\*%s\b' % e, nm, new)
            if n or n2: return new
        return None
    if mode == 'chain':
        for pat, rep in (
            (r'^(\s*choose\s+)\*(%s)\b' % e, r'\g<1>' + nm),
        ):
            new, n = re.subn(pat, rep, head)
            if n: return new
        new, n = re.subn(r'\(\s*\*%s\s*\)\.' % e, nm + '.', head)
        if n: return new
        return None
    return None

def sweep(path, mode, dry=False):
    lines = open(path, encoding='utf8').read().split('\n')
    changed = 0
    i = 0
    while i < len(lines):
        if not DECL.match(strip_comment(lines[i])):
            i += 1; continue
        got = scoped_names(lines, i)
        if got[0] is None:
            i = got[1] + 1; continue
        names, end, span = got
        for k in range(end + 1, min(end + 1 + span, len(lines))):
            cut = len(lines[k].split(';')[0])
            head, tail = lines[k][:cut], lines[k][cut:]
            for nm, pointee in names.items():
                if '*' + nm not in head.replace(' ', '') and '*%s' % nm not in head:
                    if not re.search(r'\*\s*%s\b' % re.escape(nm), head):
                        continue
                new = rewrite(head, nm, pointee, mode)
                if new is not None and new != head:
                    lines[k] = new + tail
                    head = new
                    changed += 1
        i = end + 1
    if changed and not dry:
        open(path, 'w', encoding='utf8').write('\n'.join(lines))
    return changed

if __name__ == '__main__':
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    dry = '--dry' in sys.argv
    mode = 'store'
    for m in ('store', 'read', 'chain'):
        if '--' + m in sys.argv: mode = m
    per = collections.Counter()
    for root in args or ['packages']:
        for d, dirs, files in os.walk(root):
            dirs[:] = [x for x in dirs if x not in ('tests', 'out')]
            for f in sorted(files):
                if f.endswith('.scaly'):
                    p = os.path.join(d, f)
                    c = sweep(p, mode, dry)
                    if c: per[p] = c
    for p, c in sorted(per.items()): print(f'{c:5}  {p}')
    print(sum(per.values()), mode, 'sites' + (' (dry run)' if dry else ''))
