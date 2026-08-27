#!/usr/bin/env python3
# nullarg.py -- a `null` ARGUMENT handed to a NON-OPTIONAL `ref[T]` parameter.
#
# The four DECLARED-TYPE positions are gated (report_null_into_ref#) and an
# Option into a plain ref is refused (type_conformance_accepted# class 2).  The
# ARGUMENT position is neither: `null` plans as `pointer[void]`, which
# types_compatible# accepts for every pointer and ref parameter by intent.  So
# this is where null still reaches a reference that declares it cannot be null.
#
# A CONSTRUCTION is judged too: its declaration is the record's `init`, or --
# when the record declares none of matching arity -- its positional FIELD list.
#
# ★★★THREE READING BUGS, and each one HID FINDINGS WHILE THE TOOL READ AS
# WORKING -- the count it printed was well-formed every time.  They are written
# at their sites below; what they share is the shape worth remembering: a
# text-reading instrument fails by being SILENTLY NARROW, never by erroring.
#   (a) `code()` deleted string literals instead of leaving a PLACEHOLDER, so a
#       bare `"void"` argument vanished and every position after it shifted --
#       169 PlannedType constructions read as 5-argument calls.
#   (b) `DECL` demanded `\s+` after the keyword, which `init(` does not have,
#       so the init index was EMPTY and every construction fell back to the
#       record's FIELD list -- a different declaration (`InputSource` declares
#       field `origin: ref[Origin]?` and init parameter `origin: ref[Origin]`).
#   (c) The call regex's lookbehind was `(?<![\w.])`, which excludes a name
#       preceded by a DOT -- so every method call and every qualified static
#       was invisible.  This one alone was the difference between 4 findings
#       and 85.
# ★The METHOD that surfaced all three: watch the `no-declaration` residue, not
# the finding count.  A finding count of 4 looked like a closed class; 604
# unresolved call heads said the tool could not see the tree.
#
# ★★A name-keyed index cannot resolve a RECEIVER, so a call to an overloaded
# bare name lands in `ambiguous-overload` rather than in the findings.  That
# residue is not noise to be tuned away -- READ it.  Of 37 such rows (almost
# all of them the name `make`), two were real findings the tool had attributed
# to a namesake: `ElementType.set_map` and `AttributeValue.make_tokenized`.
import re, glob, sys, collections

ROOTS = [a for a in sys.argv[1:] if not a.startswith('-')] or [
    'packages/scaly/0.1.0', 'packages/scalyc/0.1.0', 'packages/scalyls/0.1.0',
    'packages/scalygpu/0.1.0', 'packages/opensp/0.1.0', 'packages/dazzle/0.1.0',
    'packages/tscaly/0.1.0']
FILES = []
for r in ROOTS:
    FILES += glob.glob(r + '/**/*.scaly', recursive=True)
SRC = {p: open(p, encoding='utf-8').read().split('\n') for p in FILES}

def code(l):
    out, i, n = [], 0, len(l)
    while i < n:
        c = l[i]
        if c == '"':
            # ★A string literal must leave a PLACEHOLDER behind, not a hole:
            # deleting it drops a bare `"void"` argument and shifts every
            # position after it, so the parameter a null is compared against
            # is the wrong one.  (Measured: 169 PlannedType constructions read
            # as 5-argument calls.)
            i += 1
            while i < n:
                if l[i] == '\\': i += 2; continue
                if l[i] == '"': i += 1; break
                i += 1
            out.append('_str_')
            continue
        if c == "'":
            # A string IDENTIFIER is a name -- keep it verbatim, the
            # declaration index matches on it.
            j = i + 1
            while j < n and l[j] != "'": j += 1
            out.append(l[i:j+1]); i = j + 1
            continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

def split_args(s, nl=False):
    out, d, cur = [], 0, ''
    for c in s:
        if c in '([': d += 1
        elif c in ')]': d -= 1
        if c == ',' and d == 0:
            out.append(cur.strip()); cur = ''
        elif nl and c == '\n' and d == 0:
            if cur.strip(): out.append(cur.strip())
            cur = ''
        else: cur += c
    if cur.strip(): out.append(cur.strip())
    return out

def balanced(txt, o):
    d = 0
    for k in range(o, len(txt)):
        if txt[k] == '(': d += 1
        elif txt[k] == ')':
            d -= 1
            if d == 0: return k
    return None

def params_of(txt, o):
    """Parse a parameter list starting at the '(' at index o."""
    e = balanced(txt, o)
    if e is None: return None
    out = []
    for a in split_args(txt[o+1:e]):
        if ':' in a:
            pn, pt = a.split(':', 1)
            out.append((pn.strip(), pt.strip()))
        else:
            out.append((a.strip(), ''))
    return out

def joined(src, i):
    """The logical line starting at i, continued while parens are unbalanced."""
    txt, k = code(src[i]), i
    while txt.count('(') > txt.count(')') and k + 1 < len(src) and k - i < 40:
        k += 1; txt += ' ' + code(src[k])
    return txt

# ★`\s+` after the keyword misses `init(` -- which is how every init in the
# tree is written, so the whole init index came back EMPTY and every
# construction fell through to the record's FIELD list.  A field list is not a
# declaration of the init's parameters: `InputSource` declares its field
# `origin: ref[Origin]?` and its init's parameter `origin: ref[Origin]`.
DECL = re.compile(r'^\s*(function|procedure|operator|init)\b\s*(\'[^\']+\'|[A-Za-z_][A-Za-z0-9_]*)?\s*\(')
RECORD = re.compile(r'^\s*define\s+([A-Z][A-Za-z0-9_]*)\b')

# ---- 1. routines by name, record inits and field lists by record
sigs = collections.defaultdict(list)      # routine name -> [(path, line, params, has_this)]
recinit = collections.defaultdict(list)   # record -> [(path, line, params)]
recfields = {}                            # record -> (path, line, params)
cur_rec = [None, 0]

for p, src in SRC.items():
    stack = []
    for i, l in enumerate(src):
        c = code(l)
        mr = RECORD.match(c)
        if mr:
            cur_rec = [mr.group(1), i]
            # the field list may open on a LATER line and separate on NEWLINES
            blob, k = '', i
            while k < len(src) and k - i < 200:
                blob += code(src[k]) + '\n'
                st = blob.find('(')
                if st >= 0 and balanced(blob, st) is not None: break
                if st < 0 and re.search(r'\{|\bunion\b', blob[len(code(src[i])):]): st = -2; break
                k += 1
            st = blob.find('(')
            if st >= 0:
                e = balanced(blob, st)
                if e is not None:
                    ps = []
                    for a in split_args(blob[st+1:e], nl=True):
                        if ':' in a:
                            pn, pt = a.split(':', 1); ps.append((pn.strip(), pt.strip()))
                        else: ps.append((a.strip(), ''))
                    if mr.group(1) not in recfields:
                        recfields[mr.group(1)] = (p, i + 1, ps)
        m = DECL.match(c)
        if not m: continue
        kind, nm = m.group(1), m.group(2)
        txt = joined(src, i)
        o = txt.index('(')
        ps = params_of(txt, o)
        if ps is None: continue
        has_this = bool(ps) and ps[0][0] == 'this'
        if kind == 'init':
            if cur_rec[0]: recinit[cur_rec[0]].append((p, i + 1, ps))
        else:
            if nm: sigs[nm.strip("'")].append((p, i + 1, ps, has_this))

def is_plain_ref(t):
    t = t.strip()
    return t.startswith('ref[') and t.endswith(']') and not t.endswith(']?')

NULLARG = re.compile(r'^null(\s+as\s+pointer\[.*\])?$')
SHOW = '--all' in sys.argv

findings, skipped = [], collections.Counter()
for p, src in SRC.items():
    for li, l in enumerate(src):
        c = code(l)
        if DECL.match(c) or RECORD.match(c): continue
        txt = joined(src, li)
        # ★The lookbehind must NOT exclude a dot: `ELObj.make_style(...)` and
        # every method call is dotted, and rejecting them made the whole
        # receiver half of the tree invisible.  Dotted-ness is READ here
        # instead, because it decides whether the receiver fills the callee's
        # `this` slot and every argument index shifts by one.
        for m in re.finditer(r'(?<![\w])([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]\[]*\])?\s*(?:\^[A-Za-z_][A-Za-z0-9_]*|#)?\s*\(', txt):
            nm = m.group(1)
            dotted = bool(re.search(r'[\w\)\]]\s*\.\s*$', txt[:m.start(1)]))
            o = m.end() - 1
            e = balanced(txt, o)
            if e is None: continue
            args = split_args(txt[o+1:e])
            hits = [j for j, a in enumerate(args) if NULLARG.match(a)]
            if not hits: continue
            # candidate declarations
            cands = []
            if nm[0].isupper():
                inits = [(dp, dl, ps) for (dp, dl, ps) in recinit.get(nm, [])
                         if len([x for x in ps if x[0] != 'this']) == len(args)]
                if inits:
                    cands = [(dp, dl, ps, ps and ps[0][0] == 'this') for dp, dl, ps in inits]
                elif nm in recfields and len(recfields[nm][2]) == len(args):
                    dp, dl, ps = recfields[nm]
                    cands = [(dp, dl, ps, False)]
            else:
                cands = sigs.get(nm, [])
            if not cands:
                skipped['no-declaration'] += len(hits); continue
            # An overload set is scoped by ARITY first -- a candidate whose
            # parameter list cannot take this call says nothing about it.
            fitted = []
            for (dp, dl, ps, has_this) in cands:
                qs = ps[1:] if (dotted and has_this) else [x for x in ps if x[0] != 'this']
                if len(qs) == len(args): fitted.append((dp, dl, qs))
            if not fitted:
                skipped['arity'] += len(hits); continue
            for j in hits:
                verdicts, where = set(), []
                for (dp, dl, ps) in fitted:
                    if j >= len(ps): verdicts.add('arity'); continue
                    pn, pt = ps[j]
                    if is_plain_ref(pt):
                        verdicts.add('ref'); where.append(f'{dp}:{dl} {pn}: {pt}')
                    else:
                        verdicts.add('other')
                if verdicts == {'ref'}:
                    findings.append((p, li + 1, nm, j, args[j], where))
                elif 'ref' in verdicts: skipped['ambiguous-overload'] += 1
                elif verdicts == {'arity'}: skipped['arity'] += 1
                else: skipped['param-not-plain-ref'] += 1

for f in sorted(findings):
    print(f'{f[0]}:{f[1]}\t{f[2]}  arg {f[3]}  `{f[4]}`\t-> {f[5][0]}')
print(f'\n{len(findings)} findings', dict(skipped))
