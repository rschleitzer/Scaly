#!/usr/bin/env python3
#
# refconv.py — convert a package's `pointer[Concept]` DECLARATIONS to `ref[T]`.
#
#   tools/refconv.py packages/<pkg>/<version>            # report only
#   tools/refconv.py packages/<pkg>/<version> --apply     # rewrite in place
#
# It is IDEMPOTENT on the end state: re-applied to a converted tree it converts
# nothing and changes no byte.  That property is the point of committing it —
# the previous pass's tool lived outside the repository and is gone, so its
# claims about opensp/dazzle can no longer be re-checked.
#
# The doctrine it implements is in the root CLAUDE.md ("DECLARE a borrowed
# concept reference as ref[T]"); the per-position table, the ABI reason behind
# it, the four shapes that stay pointers and the controls this tool must pass
# are in packages/tscaly/CLAUDE.md §3.5dj.
#
# THIS IS NOT EMISSION-NEUTRAL: `encode_type` mangles `pointer`->"P" and
# `ref`->"R", so every converted signature gets a new symbol name.
#
# ★A nullable RETURN converts to `ref[T]?` since 2026-08-24.  It used to stay a
# `pointer` because an NPO return cost the function an implicit caller page;
# that page is now decided by the body, not by the return type, so the reason is
# gone.  What did NOT change: a nullable return is still the position where a
# WRONG conversion is expensive, because the value flows on -- the locals bound
# to such a call change type with it, and the parameters and fields those locals
# reach have to move in the SAME pass.  That is why this tool converts whole
# DECLARATION sets rather than signatures; a sed over `returns pointer[X]` alone
# produces `function not found` and `no matching initializer` (measured on
# opensp: 75 rewritten returns, 5 hard diagnostics).  Convert a
# package in ONE go.  A PORT package cannot move the compiler image (cycle.sh
# compiles packages/scaly and packages/scalyc only), so no seed is owed for one.
#
"""pointer[Concept] -> ref[Concept] over a Scaly package, one pass.  (v2)

Doctrine (root CLAUDE.md, 2026-08-23): declare a borrowed concept reference as
`ref[T]`; `ref[T]?` for a nullable PARAMETER, FIELD or LOCAL; `pointer[T]` for a
nullable RETURN (an `Option[ref[T]]` return acquires an implicit caller page, and
a region frame is contagious across the whole function) and for the four shapes
that stay pointers for a reason: the C boundary, a container whose value the
callee owns, a cell whose ADDRESS is taken, and a hand-walked buffer plus the
accessor that hands it out.  A generic ARGUMENT stays (`Array[pointer[X]]` — the
converted ports hold ZERO `[ref[...]]` -- an OBSERVATION, not a proof, and the
one class still held back by a blanket rule) and an `as pointer[X]` cast stays.

★The OPAQUE-SLOT ROUND TRIP converts too (`cast_from_opaque`): a value read
back out of a `pointer[void]` slot -- a union payload reused for a second
purpose, a VM stack cell, a NIC record smuggled through a generic field -- is a
borrowed reference the moment the cast lands, so its target is `ref[X]`.  The
`host.allocate(sizeof X, alignof X) as pointer[X]` idiom is EXCLUDED although
`Page.allocate` also answers `pointer[void]`: that cast is where raw memory
becomes a typed object, the escape hatch's home position, not a round trip.
The source stays `pointer[void]` in every case -- only the target moves.

★A nullable LOCAL is often spelled as a CAST -- `var x null as pointer[X]` --
and the occurrence then sits after an `as`, so `position` reads it as a cast and
the class is invisible.  It is the largest single one left after class A (194
sites, 148 of them eligible), and `NULLCAST` in the main loop rewrites the whole
binding to the annotated form `var x: ref[X]? null`.  The two are equivalent at
run time (measured); the annotated one says what the SLOT is instead of what a
null was cast to, which is what lets the field and parameter rules see it.

★A PARAMETER that only FORWARDS its value to a nullable parameter of another
routine is itself nullable, and this tool cannot see it: `nulled()` reads the
routine's OWN body, and a forwarder tests nothing.  Measured on
`dazzle_cli.load_stylesheet`, whose two `Array[String]` parameters go straight
into opensp's `Parser.parse_simple_cat(... ref[Array[String]]?, ...)` while its
own callers pass `null` -- the conversion gave them the non-nullable spelling
and the call stopped resolving (`function not found: load_stylesheet`, i.e.
LOUD).  Fixed by hand; teaching the tool would mean following a call across a
PACKAGE boundary.

Positions are decided by what PRECEDES an occurrence; the inner type is read
BRACKET-BALANCED; and every name scan is aimed at the BODY that declares the
name — a file-wide scan poisons short names across function boundaries.

FIELD nullability cannot be read off `.field = null`: this port sets an optional
slot from a null-initialised LOCAL at the CONSTRUCTION site, so the record's
positional call sites are the evidence, and `ast.scaly` says so in prose ("The
expression is OPTIONAL — a bare `return;` has none").
"""
import os, re, sys, collections

# A head whose `pointer[...]` stays a pointer, grouped by the REASON it stays.
# The groups exist because the previous shape of this set -- one flat list --
# held two classes back by BLANKET RULE rather than by a measured reason, and a
# reason nobody can name is a reason nobody can retire.

# A pointee that is not a concept at all: a `pointer[char]` is a buffer or a C
# boundary, never a borrowed reference to an object.
STAY_PRIMITIVE = {
    'void','char','const_char','bool','int','size_t','float','double',
    'u8','u16','u32','u64','i8','i16','i32','i64',
}
# The out-param CELL: the outer pointer is what makes the cell writable, so the
# inner one has to match (root CLAUDE.md, the four shapes that stay pointers).
STAY_INDIRECTION = {'pointer','ref'}
# The RBMM substrate.  `pointer[Page]` is not a taste: the Modeler PEELS a first
# parameter named page/rp typed exactly that into the implicit caller-page slot,
# and `T^name(...)` requires a `pointer[Page]`.
STAY_RUNTIME = {'Page','PageList','StackBucketHeader','PageNode'}
# A generic parameter's pointee -- `pointer[T]` inside a generic body, where the
# planner's `is_npo_option_generic_pointee` fast path reads the DECLARED type.
STAY_GENERIC_PARAM = {'T'}
# ★`Array` LEFT this set on 2026-08-25 (class A).  It had never been measured --
# every container name simply sat here -- and the doctrine's exemption is for a
# container whose VALUE the callee owns, which is a statement about the element
# and about `get_buffer()`, not about a borrowed reference TO the container.
# What stands in its place is not a blanket rule but the hazard detectors, which
# apply to Array exactly as they do to any other concept: a subscripted,
# address-taken or arithmetic-walked declaration still stays a pointer, decided
# per site.  The heads still listed here are NOT proven to belong: they are
# ten sites across all three ports (4 `pointer[String]` in opensp, 4
# `pointer[StringBuilder]` and 2 `pointer[Vector]` in tscaly), too few to be
# worth a measuring round, so they stay unmeasured rather than justified.
STAY_CONTAINER = {
    'Vector','String','StringBuilder','List','Slice','HashMap',
    'HashMapBuilder','HashSet','HashSetBuilder','BuilderList','Node',
    'KeyValuePair','Iterator',
}
STAY_HEADS = (STAY_PRIMITIVE | STAY_INDIRECTION | STAY_RUNTIME
              | STAY_GENERIC_PARAM | STAY_CONTAINER)

# Class B is opt-in (`--elements`): it changes container INSTANTIATIONS, so it
# is not emission-neutral and wants its own validation round.
CLASS_B = '--elements' in sys.argv
ROUTINE = re.compile(r'^(\s*)(function|procedure|operator|init)\b')
LOCALDECL = re.compile(r"^\s*(let|var)\s+('[^']+'|[A-Za-z_][A-Za-z0-9_]*)\s*:")
NULLCAST  = re.compile(r"^(\s*)(let|var)\s+('[^']+'|[A-Za-z_][A-Za-z0-9_]*)\s+null\s+as\s+(?=pointer\[)")
PROP = re.compile(r"^\s*([a-z_][A-Za-z0-9_]*)\s*:\s*\S")

def code_of(line):
    out, i, n = [], 0, len(line)
    while i < n:
        c = line[i]
        if c == '"':
            i += 1
            while i < n:
                if line[i] == '\\': i += 2; continue
                if line[i] == '"':  i += 1; break
                i += 1
            continue
        if c == "'":
            i += 1
            while i < n and line[i] != "'": i += 1
            i += 1
            continue
        if c == ';':
            break
        out.append(c); i += 1
    return ''.join(out)

def is_comment(line): return line.lstrip().startswith(';')

def inner_of(s, i):
    d, j = 0, i + len('pointer')
    for k in range(j, len(s)):
        if s[k] == '[': d += 1
        elif s[k] == ']':
            d -= 1
            if d == 0: return s[j+1:k], k+1
    return None, None

def head_of(inner):
    m = re.match(r'\s*([A-Za-z_][A-Za-z0-9_]*)', inner or '')
    return m.group(1) if m else ''

def position(line, i):
    b = line[:i].rstrip()
    if b.endswith(':'):             return 'decl'
    if re.search(r'\breturns$', b): return 'returns'
    if re.search(r'\bas$', b):      return 'cast'
    if b.endswith('[') or b.endswith(',') or b.endswith('('): return 'generic'
    return 'other'

def split_args(s):
    """Split a top-level argument list (already without the outer parens)."""
    out, d, cur = [], 0, ''
    for c in s:
        if c in '([': d += 1
        elif c in ')]': d -= 1
        if c == ',' and d == 0:
            out.append(cur.strip()); cur = ''
        else:
            cur += c
    if cur.strip(): out.append(cur.strip())
    return out

def paren_span(s, i):
    """s[i] == '('; return the index just past the matching ')'."""
    d = 0
    for k in range(i, len(s)):
        if s[k] == '(': d += 1
        elif s[k] == ')':
            d -= 1
            if d == 0: return k
    return -1

class File:
    def __init__(self, path):
        self.path  = path
        self.lines = open(path, encoding='utf-8').read().split('\n')
        self.sig   = {}    # sig line -> (name, body_lo, body_hi)
        self.owner = {}    # body line -> sig line
        self.record_of = {}  # property line -> record name
        self.index()

    def index(self):
        L, n = self.lines, len(self.lines)
        for i, line in enumerate(L):
            if is_comment(line) or not ROUTINE.match(line):
                continue
            m = re.search(r'\b(function|procedure|operator|init)\s+(\'[^\']+\'|[A-Za-z_][A-Za-z0-9_]*)', line)
            name = m.group(2).strip("'") if m else 'init'
            j = i + 1
            while j < n and (not L[j].strip() or is_comment(L[j])): j += 1
            if j >= n:
                lo = hi = i
            elif L[j].strip().startswith('{'):
                d, k = 0, j
                while k < n:
                    d += code_of(L[k]).count('{') - code_of(L[k]).count('}')
                    if d == 0 and k > j - 1 and '{' in code_of(L[k]): break
                    if d == 0 and k > j: break
                    k += 1
                lo, hi = j, min(k, n - 1)
            else:
                lo = hi = j
            self.sig[i] = (name, lo, hi)
            for k in range(lo, hi + 1):
                self.owner[k] = i
        # records: `define NAME` + a parenthesised property list
        for i, line in enumerate(L):
            m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)', line)
            if is_comment(line) or not m:
                continue
            rec = m.group(2)
            rest = line[m.end():]
            if '(' in rest:                       # one-liner
                continue
            j = i + 1
            while j < n and (not L[j].strip() or is_comment(L[j])): j += 1
            if j < n and L[j].strip() == '(':
                k = j + 1
                while k < n and L[k].strip() != ')':
                    if not is_comment(L[k]) and PROP.match(L[k]):
                        self.record_of[k] = rec
                    k += 1

    def body(self, sigline, with_sig=True):
        name, lo, hi = self.sig[sigline]
        t = '\n'.join(self.lines[lo:hi + 1])
        return (self.lines[sigline] + '\n' + t) if with_sig else t

# ---------- global index ----------------------------------------------------

def consume_group(L, i, op, cl):
    """From line i, skip blank/comment lines; if the next code character opens a
    group, return the line index where that group closes, else return i."""
    n, j = len(L), i
    first = True
    d = 0
    while j < n:
        c = code_of(L[j])
        if first:
            c = c[c.index(op) + 1:] if (j == i and op in c) else c
        for ch in c:
            if ch == op: d += 1
            elif ch == cl: d -= 1
        if j == i and op in code_of(L[i]):
            d += 1                      # the opener on the define line itself
        if d > 0:
            first = False
            j += 1
            continue
        if not first and d <= 0:
            return j
        # nothing opened yet: only blanks/comments may separate the define from
        # its group; anything else means this record has no such group.
        nxt = L[j].strip() if j > i else ''
        if j > i and nxt and not is_comment(L[j]):
            if nxt.startswith(op):
                d = 0
                first = False
                continue
            return i if j == i + 1 else j - 1
        j += 1
    return min(j, n - 1)

def record_texts(files):
    """recname -> the text of that record's OWN `define` block (property list +
    the brace body that follows), plus the set of names reached DOTTED with a
    receiver other than `this` anywhere in the package.

    Why this exists: the WALK evidence for a field used to be a package-wide
    scan of the BARE name, and `\b` does not exclude a preceding dot -- so
    `this.ranges + this.ri * 3` in CharsetRegistry held `ISet.ranges`, a
    different record's field that happens to share a name.  Measured
    2026-08-25: of the 35 (file, name) pairs the walk held, **not one** had a
    subscript or an arithmetic use in its own declaring file.  A name is not
    an identity.
    """
    texts, dotted_foreign = {}, set()
    for f in files:
        L, n = f.lines, len(f.lines)
        for i, line in enumerate(L):
            if is_comment(line):
                continue
            m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)', line)
            if not m:
                continue
            rec = m.group(2)
            # A record is `define NAME` + an optional PROPERTY LIST in parens +
            # an optional BODY in braces, and BOTH have to be consumed.  The
            # first draft counted parens and braces in one depth and therefore
            # stopped at the property list's `)` -- so the record's own methods
            # were outside its "own text" and every walk inside them was
            # invisible.  It released `Syntax.names_v`, a hand-walked StringC
            # buffer that this file's own dot_hazard comment names as the case
            # that must stay a pointer.  An instrument whose output looks
            # plausible is not a working instrument.
            k = consume_group(L, i, '(', ')')
            k = consume_group(L, k, '{', '}')
            texts.setdefault(rec, []).append('\n'.join(L[i:min(k, n - 1) + 1]))
    for f in files:
        for line in f.lines:
            if is_comment(line):
                continue
            for m in re.finditer(r'([A-Za-z_][A-Za-z0-9_]*)\s*\.\s*([a-z_][A-Za-z0-9_]*)',
                                 code_of(line)):
                if m.group(1) != 'this':
                    dotted_foreign.add(m.group(2))
    return {k: '\n'.join(v) for k, v in texts.items()}, dotted_foreign

def load(root):
    files = []
    for dp, dn, fns in os.walk(root):
        dn[:] = [d for d in dn if d not in ('_submodules', 'out', 'tests')]
        for fn in sorted(fns):
            if fn.endswith('.scaly'):
                files.append(File(os.path.join(dp, fn)))
    return files

def returns_null(text):
    return (re.search(r'\breturn\s+null\b', text) is not None or
            re.search(r'^\s*null\s*$', text, re.M) is not None)

def nullable_returns(files):
    """Routine name -> can it answer null?  A FIXPOINT, because this port lets a
    forwarding routine hand null on without writing `return null` itself
    (`parse_property_name` -> `parse_property_name_worker` -> a literal parse),
    and because the CALLER's own null test is the port's clearest statement that
    a routine is nullable.  Name-only resolution: a collision errs toward
    `pointer`, which is the conservative direction."""
    bodies, direct = {}, set()
    for f in files:
        for s in f.sig:
            name, lo, hi = f.sig[s]
            body = f.body(s, False)
            bodies.setdefault(name, []).append(body)
            if returns_null(body):
                direct.add(name)
    # signal C: a caller binds the result and tests it against null
    called_nullable = set()
    for f in files:
        for s in f.sig:
            body = f.body(s, False)
            lines = body.split('\n')
            for k, line in enumerate(lines):
                m = re.match(r"\s*(?:let|var)\s+([A-Za-z_][A-Za-z0-9_]*)\s+(.*)$", code_of(line))
                if not m:
                    continue
                nm, init = m.group(1), m.group(2)
                mm = re.search(r'\b([a-z_][A-Za-z0-9_]*)\s*\(', init)
                if not mm:
                    continue
                if re.search(r'\b' + re.escape(nm) + r'\b\s*(?:<>|=)\s*null\b', body) or \
                   re.search(r'\bset\s+' + re.escape(nm) + r'\s*:\s*null\b', body):
                    called_nullable.add(mm.group(1))
            for m in re.finditer(r'\bif\s+(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*\([^\n]*\)\s*(?:<>|=)\s*null\b', body):
                called_nullable.add(m.group(1))
    nullable = set(direct) | called_nullable
    # signal B: a returned CALL propagates nullability, to a fixpoint
    changed = True
    while changed:
        changed = False
        for name, blist in bodies.items():
            if name in nullable:
                continue
            for body in blist:
                hits = re.findall(r'\breturn\s+(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*\(', body)
                lines = [l for l in body.split('\n') if l.strip() and not is_comment(l)]
                tail = lines[-2] if len(lines) >= 2 and lines[-1].strip() == '}' else (lines[-1] if lines else '')
                mt = re.match(r'\s*(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*\(', code_of(tail))
                if mt:
                    hits.append(mt.group(1))
                if any(h in nullable for h in hits):
                    nullable.add(name); changed = True; break
    return nullable

def record_props(files):
    """record name -> ordered property names (positional construction order)."""
    props = collections.defaultdict(list)
    for f in files:
        n = len(f.lines)
        for i, line in enumerate(f.lines):
            if is_comment(line): continue
            m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\((.*)$', line)
            if m:                                  # one-liner define R (a: T, b: U)
                rec, rest = m.group(2), m.group(3)
                end = rest.rfind(')')
                for a in split_args(rest[:end] if end >= 0 else rest):
                    mm = re.match(r'([a-z_][A-Za-z0-9_]*)\s*:', a)
                    if mm: props[rec].append(mm.group(1))
                continue
        for ln, rec in sorted(f.record_of.items()):
            mm = PROP.match(f.lines[ln])
            if mm: props[rec].append(mm.group(1))
    return props

def nullable_locals(body):
    """Names in this body that hold null at some point."""
    out = set()
    for m in re.finditer(r"^\s*var\s+([A-Za-z_][A-Za-z0-9_]*)\s+null\b", body, re.M):
        out.add(m.group(1))
    for m in re.finditer(r"^\s*var\s+([A-Za-z_][A-Za-z0-9_]*)\s*:[^\n]*?\bnull\s*$", body, re.M):
        out.add(m.group(1))
    for m in re.finditer(r"\bset\s+([A-Za-z_][A-Za-z0-9_]*)\s*:\s*null\b", body):
        out.add(m.group(1))
    return out

def guarded_before(lines, lo, site, nm, site_indent):
    """True when `if NM = null` + an exit stands BEFORE `site`, at an indentation
    that DOMINATES the construction (same level or shallower).  Narrowing too
    eagerly would turn a nullable slot into a plain `ref`, i.e. a lie, so a guard
    nested deeper than the construction does not count."""
    pat = re.compile(r'^(\s*)if\s+' + re.escape(nm) + r'\s*=\s*null\b(.*)$')
    for k in range(lo, site):
        m = pat.match(lines[k])
        if not m:
            continue
        if len(m.group(1)) > site_indent:
            continue
        tail = m.group(2).strip()
        if tail.startswith(('return', 'break', 'continue', 'throw')):
            return True
        j = k + 1
        while j < site and (not lines[j].strip() or is_comment(lines[j])):
            j += 1
        if j < site and lines[j].strip().startswith(('return', 'break', 'continue', 'throw')):
            return True
    return False

def field_nullability(files, props, retnull):
    """(record, prop) -> True when a CONSTRUCTION site can pass null there."""
    nullable = set()
    # which routines can answer null?
    ret_null = {n: True for n in retnull}
    for f in files:
        for i, line in enumerate(f.lines):
            if is_comment(line): continue
            code = code_of(line)
            for m in re.finditer(r'\b([A-Z][A-Za-z0-9_]*)\s*\(', code):
                rec = m.group(1)
                if rec not in props: continue
                if re.match(r'^\s*define\b', code): continue
                end = paren_span(code, m.end() - 1)
                if end < 0: continue
                args = split_args(code[m.end():end])
                plist = props[rec]
                if len(args) != len(plist): continue
                sig = f.owner.get(i)
                body = f.body(sig) if sig is not None else ''
                nl = nullable_locals(body)
                lo = f.sig[sig][1] if sig is not None else i
                indent = len(line) - len(line.lstrip())
                for a, p in zip(args, plist):
                    bare = a.strip()
                    if bare == 'null':
                        nullable.add((rec, p)); continue
                    if re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', bare) and bare in nl:
                        if not guarded_before(f.lines, lo, i, bare, indent):
                            nullable.add((rec, p))
                        continue
                    mm = re.match(r'(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*\(', bare)
                    if mm and ret_null.get(mm.group(1)):
                        nullable.add((rec, p))
    return nullable

def field_verdicts(files, all_text, fnull, rectexts, dotted_foreign):
    """Decide every record property ONCE, keyed (record, field), so the FIELD
    branch and the init-parameter mirror below read the SAME answer.  Reading the
    field's verdict twice from two predicates is what let the mirror rule miss
    `ExpandoAssignmentInfo.container`: the field was nullable through the DOT
    scan while the mirror asked only the construction-site evidence."""
    out = {}
    for f in files:
        for li, line in enumerate(f.lines):
            if is_comment(line):
                continue
            rec = f.record_of.get(li)
            if rec is None:
                m = re.match(r'^\s*define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', line)
                rec = m.group(1) if m else None
            if rec is None or li in f.sig or li in f.owner:
                continue
            i = 0
            while True:
                j = line.find('pointer[', i)
                if j < 0:
                    break
                inner, end = inner_of(line, j)
                head, pos = head_of(inner), position(line, j)
                nm = name_before(line, j) if pos == 'decl' else None
                if nm and head not in STAY_HEADS and pos == 'decl':
                    addr, walk, dnull = dot_hazard(
                        nm, all_text, scan_scope(nm, rec, rectexts, dotted_foreign))
                    if addr:                            v = ('pointer', 'field-addr-taken')
                    elif walk:                          v = ('pointer', 'field-walked')
                    elif dnull or (rec, nm) in fnull:   v = ('ref?', 'field-nullable')
                    else:                               v = ('ref', 'field')
                    out[(rec, nm)] = v
                i = end
    return out

def init_param_nullability(files, fverdict):
    """An `init` parameter must mirror the NULLABILITY of the field it is stored
    into.  Measured, and it is a silent class: giving
    `ExpandoAssignmentInfo.container` the honest `ref[AstNode]?` while its
    `init` kept a plain `ref[AstNode]` made `check_initializer_match` reject the
    init, so the construction fell through to the POSITIONAL-TUPLE path meant
    for data classes that declare none -- rc 0, no diagnostic, and correct here
    only because that init assigns in field order.  Root CLAUDE.md: at matching
    arity argument i lands in field i, so an init whose parameters are in a
    different order than the fields writes the WRONG ones.

    Returns two sets of (file_path, sig_line, param_name): those that must carry
    the `?`, and those that must stay a POINTER because the field does."""
    out, ptr = set(), set()
    for f in files:
        # enclosing concept of each routine line
        enclosing = {}
        stack = []
        for i, line in enumerate(f.lines):
            if is_comment(line):
                continue
            m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)', line)
            if m:
                stack = [(len(m.group(1)), m.group(2))]
            if i in f.sig:
                enclosing[i] = stack[-1][1] if stack else None
        for i, rec in enclosing.items():
            name, lo, hi = f.sig[i]
            if name != 'init' or rec is None:
                continue
            body = f.body(i, False)
            # `set this.locs: locs` is the same store as `set locs: locs` and the
            # tree writes both; without the optional `this.` the mirror missed
            # every init that qualifies its target (Text's does).
            for m in re.finditer(r'\bset\s+(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*:\s*([A-Za-z_][A-Za-z0-9_]*)\s*$',
                                 body, re.M):
                field, param = m.group(1), m.group(2)
                v = fverdict.get((rec, field), ('', ''))[0]
                if v == 'ref?':
                    out.add((f.path, i, param))
                elif v == 'pointer':
                    # The field stays a POINTER (a hand-walked buffer, an
                    # address-taken cell), so the init parameter that fills it
                    # must too -- `Text.init(str, locs: ref[Location])` against a
                    # `locs: pointer[Location]` field is the same disagreement as
                    # the nullability one above, one property further along.
                    ptr.add((f.path, i, param))
    return out, ptr

def walked_returns(files):
    """Routine name -> is its RESULT walked by a caller?  The `returns` position
    carries no name, and the buffer it hands out may be walked in a DIFFERENT
    file than the one that declares the field -- `InternalEntity.def_locs` is
    stored plainly and only `get_def_locs()`'s result is stepped through, so
    neither the field scan nor returns_field_verdicts can see it.  Ask the call
    sites instead: a local bound to a call and then walked marks that callee.

    Name-only resolution, like nullable_returns: a collision errs toward
    `pointer`, the conservative direction."""
    out = set()
    # The RECEIVER of the call is an arbitrary expression, not a dotted
    # identifier chain: `let dl (*entp).internal_def_locs()` is the shape that
    # actually occurs, and a regex anchored on `name.name(` misses it. So take
    # the whole initializer and mark EVERY routine it names -- an
    # over-approximation, which is the conservative direction here (more
    # pointers, never fewer).
    bind = re.compile(r'^\s*(?:let|var)\s+([A-Za-z_][A-Za-z0-9_]*)'
                      r'(?:\s*:[^\n]*?)?\s+(.*)$', re.M)
    for f in files:
        for s_ in f.sig:
            body = f.body(s_, False)
            for m in bind.finditer(body):
                local, init = m.group(1), m.group(2)
                if walked(local, body):
                    for c in re.finditer(r'([A-Za-z_][A-Za-z0-9_]*)\s*\(', init):
                        out.add(c.group(1))
        # a result walked without a binding: `*(e.get_def_locs() + i)`
        text = '\n'.join(f.lines)
        for m in re.finditer(r'\*\s*\(\s*(?:[A-Za-z_][A-Za-z0-9_]*\.)*'
                             r'([A-Za-z_][A-Za-z0-9_]*)\s*\([^()]*\)\s*[+\-]', text):
            out.add(m.group(1))
    return out

def returns_field_verdicts(files, fverdict):
    """A `returns` carries no NAME, so no name rule can reach it -- and that is
    exactly the position where a hand-walked buffer escapes the classification:
    `Text.locs` stays a pointer while `get_locs()` did not, and every caller
    then inherited a ref it walks (root CLAUDE.md names this pair).  Link the
    two by the BODY: an accessor whose whole body is a bare field read hands out
    that field, so it inherits the field's verdict.

    Returns {(file_path, sig_line): verdict} for the accessors it can decide."""
    out = {}
    for f in files:
        enclosing, stack = {}, []
        for i, line in enumerate(f.lines):
            if is_comment(line):
                continue
            m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)', line)
            if m:
                stack = [(len(m.group(1)), m.group(2))]
            if i in f.sig:
                enclosing[i] = stack[-1][1] if stack else None
        for i, rec in enclosing.items():
            if rec is None:
                continue
            body = f.body(i, False)
            names = set()
            for m in re.finditer(r'^\s*(?:return\s+)?(?:this\.)?([a-z_][A-Za-z0-9_]*)\s*$',
                                 body, re.M):
                names.add(m.group(1))
            for nm in names:
                v = fverdict.get((rec, nm), ('', ''))[0]
                if v == 'pointer':
                    out[(f.path, i)] = ('pointer', 'returns-walked-field')
    return out

# ---------- the C boundary, hand-allocated buffers, forwarders --------------

# An `extern` declaration IS the C boundary -- the first of the four shapes that
# stay pointers -- and it is decided by the LINE, with no judgement: one seed
# serves every target, so a declaration's spelling has to match the real
# prototype and cannot become a Scaly reference.  It was worth a rule of its own
# because a converted extern is not merely wrong, it is INVISIBLY wrong: nothing
# checks a call against a declaration (`emit_call` synthesizes the callee's type
# from the ARGUMENTS), and `tests/abi/run.sh`'s consistency check compares the
# LLVM shape, where `ref` and `pointer` are both `ptr`.  Found on
# `scalyls/worker.scaly`'s `poll`, whose own comment says the declaration must
# stay type-identical to `scaly/fiber.scaly`'s.
EXTERN = re.compile(r'\bextern\s*$')
def is_extern(line): return bool(EXTERN.search(code_of(line).rstrip()))

# malloc's answer is a hand-allocated BUFFER, the fourth shape that stays a
# pointer, and the allocation is the proof -- no arithmetic has to appear in the
# same body for the value to be one.  `free` is the same fact from the other
# side, and it reaches the position no name rule does: a `destroy(p)` whose only
# statement is `free(p as pointer[void])` walks nothing, tests p against null,
# and was read as a nullable reference.
ALLOC = r'(?:malloc|calloc|realloc|aligned_alloc|scaly_aligned_alloc)'
FREE  = r'(?:free|scaly_aligned_free)'
def heap_buffer(nm, text):
    """★The name must be the WHOLE thing allocated or freed, never the BASE of a
    dotted chain.  The first draft asked `\bfree\s*\(\s*nm\b` and matched
    `free(s.text_data as pointer[void])`, reading a single borrowed slot as the
    buffer because a FIELD of it is one -- seven sites in scalyls, every one of
    them a correct `ref`.  A `\b` is not a terminator; the following character
    is."""
    e = re.escape(nm)
    return bool(
        # bound or assigned FROM an allocator: `let p malloc(n) as pointer[X]`
        re.search(r'^\s*(?:let|var)\s+' + e + r'\b(?:\s*:[^\n]*?)?\s[^\n]*?\b'
                  + ALLOC + r'\s*\(', text, re.M)
        or re.search(r'^\s*set\s+' + e + r'\s*:[^\n]*?\b' + ALLOC + r'\s*\(',
                     text, re.M)
        # handed WHOLE to a deallocator: `free(p)` / `free(p as pointer[void])`
        or re.search(r'\b' + FREE + r'\s*\(\s*' + e + r'\s*(?:as\b|\)|,)', text))

def alloc_returns(files):
    """Routine names that hand out storage they allocated themselves.  The
    ACCESSOR that hands a buffer out is named in the doctrine beside the buffer,
    and a `returns` carries no name for `heap_buffer` to test -- so ask the body
    whether a local bound from an allocator leaves it."""
    out = set()
    for f in files:
        for i, (name, _, _) in f.sig.items():
            body = f.body(i, False)
            for m in re.finditer(r'^\s*(?:let|var)\s+([A-Za-z_][A-Za-z0-9_]*)'
                                 r'(?:\s*:[^\n]*?)?\s+[^\n]*?\b' + ALLOC + r'\s*\(',
                                 body, re.M):
                local = m.group(1)
                if re.search(r'^\s*(?:return\s+)?' + re.escape(local) + r'\s*$',
                             body, re.M):
                    out.add(name)
    return out

def split_args(s):
    """Split a call's argument text at depth 0.  Returns [] for an empty list."""
    out, depth, cur = [], 0, []
    for ch in s:
        if ch in '([': depth += 1
        elif ch in ')]': depth -= 1
        if ch == ',' and depth == 0:
            out.append(''.join(cur)); cur = []
        else:
            cur.append(ch)
    tail = ''.join(cur).strip()
    if tail or out:
        out.append(tail)
    return [a.strip() for a in out]

def sig_params(line):
    """Parameter NAMES of a routine signature, in declaration order.  The peel
    matters: a first parameter named `this`, `rp` or `page` is supplied by the
    caller implicitly, so a CALL SITE's argument 0 is the routine's parameter 1
    -- which is what lets a sink be recorded as an ARGUMENT index and compared
    across the dotted and undotted spellings alike."""
    c = code_of(line)
    j = c.find('(')
    if j < 0:
        return None, 0
    depth, k = 0, j
    while k < len(c):
        if c[k] == '(': depth += 1
        elif c[k] == ')':
            depth -= 1
            if depth == 0: break
        k += 1
    names = []
    for a in split_args(c[j + 1:k]):
        m = re.match(r"^('[^']+'|[A-Za-z_][A-Za-z0-9_]*)", a)
        names.append(m.group(1).strip("'") if m else '')
    off = 1 if names[:1] and names[0] in ('this', 'rp', 'page') else 0
    return names, off

def pointer_sinks(files):
    """Call-ARGUMENT indices that must stay pointers, propagated to a fixpoint.

    ★This is the class the tool used to name as unreachable ("a PARAMETER that
    only FORWARDS its value ... this tool cannot see it: `nulled()` reads the
    routine's OWN body, and a forwarder tests nothing").  That was true only of
    a forward across a PACKAGE boundary.  Inside one package the callee is right
    there, so the answer is a fixpoint rather than a guess: seed the sinks with
    the parameters a body itself proves are buffers -- walked, address-taken,
    allocated or freed, or declared `extern` -- and then let every forwarder
    inherit its callee's verdict until nothing moves.

    Indices are ARGUMENT indices, so the `this`/`rp`/`page` peel is applied once
    here and a caller never has to know which spelling a callee was written in.
    Overloads merge conservatively (any overload's sink is the name's sink),
    which is the direction this whole tool errs in: more pointers, never fewer."""
    routines, sinks = [], collections.defaultdict(set)
    allocers = alloc_returns(files)
    for f in files:
        for i, (name, _, _) in f.sig.items():
            line = f.lines[i]
            names, off = sig_params(line)
            if names is None:
                continue
            body = f.body(i, False)
            ptr = set()
            for k, nm in enumerate(names):
                if k < off or not nm:
                    continue
                # is THIS parameter declared pointer[NonStay]?
                if not re.search(re.escape(nm) + r'\s*:\s*pointer\[', code_of(line)):
                    continue
                if (is_extern(line) or walked(nm, body) or addr_taken(nm, body)
                        or heap_buffer(nm, body)):
                    sinks[name].add(k - off)
                ptr.add(k - off)
            routines.append((f, i, name, names, off, body, ptr))

    calls = re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\(')
    moved = True
    while moved:
        moved = False
        for f, i, name, names, off, body, ptr in routines:
            for m in calls.finditer(body):
                callee = m.group(1)
                if callee not in sinks:
                    continue
                depth, k = 0, m.end() - 1
                while k < len(body):
                    if body[k] == '(': depth += 1
                    elif body[k] == ')':
                        depth -= 1
                        if depth == 0: break
                    k += 1
                args = split_args(body[m.end():k])
                for a_i in sinks[callee]:
                    if a_i >= len(args):
                        continue
                    arg = args[a_i]
                    if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', arg):
                        continue
                    if arg in names:
                        p_i = names.index(arg) - off
                        if p_i >= 0 and p_i in ptr and p_i not in sinks[name]:
                            sinks[name].add(p_i); moved = True
    return sinks, allocers

# ---------- hazards ---------------------------------------------------------

def addr_taken(nm, text): return re.search(r'&\s*' + re.escape(nm) + r'\b', text) is not None
# ★For a CONTAINER head the subscript arm below is a false positive -- `a[i]` on
# an Array is the container's `operator []`, not pointer arithmetic -- and it is
# what holds 44 of the 1136 class-A sites at `pointer`.  Left conservative on
# purpose, because the arm is right for every other head and because the two
# spellings genuinely differ: measured 2026-08-25, `a[i]` on a
# `pointer[Array[X]]` is `*(a + i)` with an ARRAY-sized stride (a fixture reading
# `let e a[i]` then `*e` reported `member not found: Array.v` -- it had indexed
# an array OF arrays), while on a `ref[Array[X]]` it dispatches to
# `Array.operator[]` and answers the element, because a ref cannot do
# arithmetic.  The conversion therefore makes the subscript MEAN what a reader
# expects.  No port site is affected: zero `.field[i]` on a `pointer[Array]`
# declaration exists across all three (checked by name).
def walked(nm, text):
    return bool(re.search(r'\b' + re.escape(nm) + r'\s*\[', text)
                or re.search(r'\*\s*\(\s*' + re.escape(nm) + r'\s*[+\-]', text)
                or re.search(r'\b' + re.escape(nm) + r'\s*[+\-]\s+\w', text))
def nulled(nm, text):
    return bool(re.search(r'\bset\s+' + re.escape(nm) + r'\s*:\s*null\b', text)
                or re.search(r'\b' + re.escape(nm) + r'\b\s*(?:<>|=)\s*null\b', text)
                or re.search(r'\bnull\s*(?:<>|=)\s*' + re.escape(nm) + r'\b', text))

def opaque_index(files):
    """Names whose declared type is UNANIMOUSLY `pointer[void]`: routine names by
    their `returns`, and property names by their declaration.  Unanimity is the
    point -- a name declared two ways anywhere in the package is skipped, so an
    overloaded or reused name can never be read as opaque on the strength of one
    declaration.  Used only to recognise the OPAQUE-SLOT ROUND TRIP below."""
    rets, props = collections.defaultdict(set), collections.defaultdict(set)
    for f in files:
        for line in f.lines:
            if is_comment(line):
                continue
            c = code_of(line)
            m = re.search(r'\b(?:function|procedure)\s+([A-Za-z_][A-Za-z0-9_]*)\b'
                          r'.*\breturns\s+([A-Za-z_][A-Za-z0-9_\[\]\?]*)', c)
            if m:
                rets[m.group(1)].add(m.group(2))
            for m in re.finditer(r'\b([a-z_][A-Za-z0-9_]*)\s*:\s*'
                                 r'([A-Za-z_][A-Za-z0-9_\[\]\?]*)', c):
                props[m.group(1)].add(m.group(2))
    void = lambda d: {k for k, v in d.items() if v == {'pointer[void]'}}
    return void(rets), void(props)

def cast_from_opaque(line, j, f, li, void_rets, void_props):
    """Is the value being cast at line[j] read out of a `pointer[void]` slot?

    This is the OPAQUE-SLOT ROUND TRIP -- a slot whose real type the language
    cannot state at that point (a union payload reused for a second purpose, a
    VM stack cell, a NIC record smuggled through a generic field), read back as
    what it is.  Measured 2026-08-25: 242 of the 284 remaining `as pointer[X]`
    casts in the three ports have this shape, and NOT ONE of their results is
    walked.  Spelling the target `ref[X]` is emission-neutral (both map to
    `ptr`, and a cast type is never mangled -- only a DECLARED signature is)
    and buys a guard: `*(r + 1)` on the result becomes a hard rc-4 instead of
    silently reading an array that is not there.

    ★The `host.allocate(sizeof X, alignof X) as pointer[X]` idiom is EXCLUDED
    even though `Page.allocate` also answers `pointer[void]`: that cast is
    where raw memory becomes a typed object, which is the escape hatch's home
    position, not a round trip through a slot.
    """
    pre = code_of(line)[:j].rstrip()
    if not pre.endswith('as'):
        return False
    pre = pre[:-2].rstrip()
    if re.search(r'\.\s*allocate\s*\(', pre):
        return False
    m = re.search(r'\.\s*([a-z_][A-Za-z0-9_]*)\s*\([^()]*\)$', pre)   # method result
    if m:
        return m.group(1) in void_rets
    m = re.search(r'\.\s*([a-z_][A-Za-z0-9_]*)$', pre)                  # field read
    if m:
        return m.group(1) in void_props
    # A DEREF of a cell holding opaque pointers: `*(buf + i)` / `*p`, where the
    # walked thing is a `pointer[pointer[void]]`.  The buffer itself stays a
    # pointer (it is walked); the element read out of it is the opaque value.
    if pre.lstrip().startswith('*'):
        m = re.search(r'\*\s*\(?\s*([A-Za-z_][A-Za-z0-9_]*)', pre)
        return bool(m) and name_declares(m.group(1), f, li, void_rets, void_props, 0) \
               == 'pointer[pointer[void]]'
    m = re.search(r'(?<![.*\w])([A-Za-z_][A-Za-z0-9_]*)$', pre)          # plain name
    if not m:
        return False
    return name_declares(m.group(1), f, li, void_rets, void_props, 0) == 'pointer[void]'

def name_declares(nm, f, li, void_rets, void_props, depth):
    """The declared type of `nm` as seen from line li, or ''.

    ★It follows ONE initializer hop, and that is what the rule needs: this
    port's ordinary shape is `let ntv x.get_notation()` / `if ntv <> null` /
    `let nt ntv as pointer[Notation]` -- an UNTYPED local bound from a call, so
    a search for `let ntv:` finds nothing and 88 sites read as unresolved.  The
    hop asks the initializer instead.  Depth stops at 2: a longer chain is not
    something to guess at lexically.
    """
    if depth > 2:
        return ''
    for k in range(li, max(0, li - 400), -1):
        src = f.lines[k]
        if is_comment(src):
            continue
        c = code_of(src)
        for pat in (r'\b(?:let|var)\s+' + re.escape(nm) + r'\s*:\s*([A-Za-z_][A-Za-z0-9_\[\]\?]*)',
                    r'[(,]\s*'          + re.escape(nm) + r'\s*:\s*([A-Za-z_][A-Za-z0-9_\[\]\?]*)',
                    r'^\s*'             + re.escape(nm) + r'\s*:\s*([A-Za-z_][A-Za-z0-9_\[\]\?]*)'):
            d = re.search(pat, c)
            if d:
                return d.group(1)
        d = re.search(r'\b(?:let|var)\s+' + re.escape(nm) + r'\s+(\S.*)$', c)
        if d:                                       # untyped: ask the initializer
            init = d.group(1).strip()
            mm = re.search(r'\.\s*([a-z_][A-Za-z0-9_]*)\s*\([^()]*\)$', init)
            if mm:
                return 'pointer[void]' if mm.group(1) in void_rets else ''
            mm = re.search(r'\.\s*([a-z_][A-Za-z0-9_]*)$', init)
            if mm:
                return 'pointer[void]' if mm.group(1) in void_props else ''
            mm = re.match(r'([A-Za-z_][A-Za-z0-9_]*)$', init)
            if mm:
                return name_declares(mm.group(1), f, k, void_rets, void_props, depth + 1)
            return ''
    return 'pointer[void]' if nm in void_props else ''

# Container heads whose GENERIC ARGUMENT is an element (or a mapped value).
# `pointer[X]` there is a container OF references, and the faithful spelling is
# the NULLABLE one: a pointer element can be null, `ref[X]?` can be null, and
# the two have the same 8-byte layout (measured).  Uniformly `?`, never a
# per-container guess -- deciding which containers can hold a null would need
# evidence from every reader and writer of that container, and choosing the
# NON-optional form wrongly is silent (a null-valued variable stored into a
# `ref[X]` element compiles rc 0 and the element is null at run time).
ELEMENT_HEADS = {
    'Array', 'Vector', 'Slice', 'List', 'HashMap', 'HashMapBuilder',
    'HashSet', 'HashSetBuilder', 'BuilderList',
}

def code_part(line):
    """(code, tail) split at the first comment/quote-safe `;`, so a trailing
    comment is never rewritten."""
    i, n = 0, len(line)
    while i < n:
        c = line[i]
        if c == '"':
            i += 1
            while i < n:
                if line[i] == '\\': i += 2; continue
                if line[i] == '"':  i += 1; break
                i += 1
            continue
        if c == "'":
            i += 1
            while i < n and line[i] != "'": i += 1
            i += 1
            continue
        if c == ';':
            return line[:i], line[i:]
        i += 1
    return line, ''

def _balanced(s, i):
    """s[i] == '['; index just past the matching ']', or -1."""
    d = 0
    for k in range(i, len(s)):
        if s[k] == '[': d += 1
        elif s[k] == ']':
            d -= 1
            if d == 0: return k
    return -1

def rewrite_elements(line):
    """`Array[pointer[X]]` -> `Array[ref[X]?]`, recursively, in the CODE part.

    Class B (2026-08-25).  A generic ARGUMENT used to stay a pointer on the
    strength of an observation -- "the converted ports hold ZERO [ref[...]]" --
    which is not a reason.  Measured before converting: `Array[ref[X]?]`
    accepts every idiom these ports use on such a container (a bare `null`
    literal, a null-valued variable, `set *(buf + i): null` to clear a slot,
    `put`, `get`, the null test, a field read through the Option, and an
    element handed to a NON-optional `ref` parameter), at the same 8 bytes.

    ★A `pointer[void]` element is NOT converted: `ref[void]?` is not a
    reference to anything.  Neither is any other STAY head.
    """
    code, tail = code_part(line)
    out, i, n = [], 0, len(code)
    while i < n:
        m = re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\[').search(code, i)
        if not m:
            out.append(code[i:]); break
        head = m.group(1)
        open_i = m.end() - 1
        close_i = _balanced(code, open_i)
        if head not in ELEMENT_HEADS or close_i < 0:
            out.append(code[i:m.end()]); i = m.end(); continue
        inner = code[open_i + 1:close_i]
        args, rebuilt = split_args(inner), []
        for a in args:
            a = rewrite_elements(a)                       # nested containers
            st = a.strip()
            if st.startswith('pointer['):
                pi, pend = inner_of(st, 0)
                if pend == len(st) and pi is not None and head_of(pi) not in STAY_HEADS:
                    a = a.replace('pointer[' + pi + ']', 'ref[' + pi + ']?', 1)
            rebuilt.append(a)
        sep = ', ' if len(args) > 1 else ''
        out.append(code[i:open_i + 1] + sep.join(rebuilt) + ']')
        i = close_i + 1
    return ''.join(out) + tail

def scan_scope(field, rec, rectexts, dotted_foreign):
    """The text a field's WALK evidence may be read from: its own record's block
    when nothing reaches the name through a foreign receiver, else None (which
    means the whole package, the conservative answer).  A field reached as
    `x.f` from elsewhere cannot be judged from its own block, and this tool
    cannot tell WHICH record such an `x` is -- so it does not guess."""
    if field in dotted_foreign:
        return None
    return rectexts.get(rec)

def dot_hazard(field, all_text, own_text=None):
    """`own_text` is the DECLARING record's own block.  When the field is never
    reached dotted from a foreign receiver, that block is the complete evidence
    and the scan narrows to it -- see record_texts for what the package-wide
    bare-name scan cost.  Passing None keeps the old package-wide behaviour,
    which is what a field reached as `x.f` from elsewhere still gets."""
    scan = all_text if own_text is None else own_text
    f = re.escape(field)
    addr = re.search(r'&\s*[A-Za-z_][A-Za-z0-9_.]*\.' + f + r'\b', all_text)
    # A hand-walked buffer FIELD is not always written with a dot: inside its
    # own record's methods it is reached through an implicit `this`, so
    # `names_v + rni` and `this.names_v + (i as size_t)` are the same buffer and
    # only the second carries one.  Nor is it always dereferenced on the spot --
    # `let slot this.names_v + (i as size_t)` walks it by plain arithmetic and
    # stores through the local.  Both shapes went unseen and turned four
    # `StringC` buffers in opensp's Syntax into refs; the compiler caught it
    # (18x "arithmetic on a reference"), which is the only reason this was not
    # a silent byte-stride miscompile.  `walked` carries the bare-name forms.
    walk = (re.search(r'\.' + f + r'\s*\[', scan) or
            re.search(r'\*\s*\(\s*[A-Za-z_][A-Za-z0-9_.]*\.' + f + r'\s*[+\-]', scan) or
            re.search(r'\.' + f + r'\s*[+\-]\s+\w', scan) or
            walked(field, scan))
    null = (re.search(r'\.' + f + r'\b\s*(?:<>|=)\s*null\b', all_text) or
            re.search(r'\bset\s+[^\n]*\.' + f + r'\s*:\s*null\b', all_text))
    return bool(addr), bool(walk), bool(null)

def name_before(line, i):
    b = line[:i].rstrip()
    if not b.endswith(':'): return None
    m = re.search(r"('[^']+'|[A-Za-z_][A-Za-z0-9_]*)\s*:$", b)
    return m.group(1).strip("'") if m else None

# ---------- main ------------------------------------------------------------

def main(root, apply=False):
    files = load(root)
    all_text = '\n'.join('\n'.join(f.lines) for f in files)
    props    = record_props(files)
    retnull  = nullable_returns(files)
    fnull    = field_nullability(files, props, retnull)
    rectexts, dotted_foreign = record_texts(files)
    void_rets, void_props = opaque_index(files)
    fverdict = field_verdicts(files, all_text, fnull, rectexts, dotted_foreign)
    initnull, initptr = init_param_nullability(files, fverdict)
    retfield = returns_field_verdicts(files, fverdict)
    retwalk  = walked_returns(files)
    sinks, allocers = pointer_sinks(files)
    tally, decisions = collections.Counter(), []

    for f in files:
        for li, line in enumerate(f.lines):
            if is_comment(line): continue
            extern = is_extern(line)

            # ★A nullable LOCAL spelled as a CAST -- `var x null as pointer[X]`.
            # The doctrine names this position by hand (a nullable PARAMETER,
            # FIELD or LOCAL is `ref[T]?`), but the occurrence sits after an
            # `as`, so `position` reads it as a cast and the whole class -- 194
            # sites across the three ports, the largest single one left -- was
            # invisible to every pass.  The two spellings are equivalent
            # (measured: same reads, same null tests, same PASS); the
            # annotated one says what the slot IS instead of what a null was
            # cast to, which is what lets the field/param rules see it at all.
            # Class B: container ELEMENTS.  Runs before the pointer scan so a
            # converted element is never revisited by it, and it is its own
            # recursion, so `Array[pointer[Array[pointer[X]]]]` settles in one
            # pass -- the pointer scan skips a nested occurrence (`i = end`),
            # which would otherwise leave the inner one for a SECOND run and
            # break idempotence.
            if CLASS_B:
                rewritten = rewrite_elements(line)
                if rewritten != line:
                    nconv = line.count('pointer[') - rewritten.count('pointer[')
                    tally[('ref?', 'container-element')] += nconv
                    for _ in range(nconv):
                        decisions.append((f.path, li + 1, 'ref?', 'container-element', '', ''))
                    f.lines[li] = rewritten
                    line = rewritten

            m = NULLCAST.match(line)
            if m and li in f.owner:
                inner, end = inner_of(line, line.index('pointer[', m.end(3)))
                head = head_of(inner)
                nm   = m.group(3).strip("'")
                rest = line[end:] if end else ''
                b    = f.body(f.owner[li])
                if (head not in STAY_HEADS and inner is not None
                        and (not rest.strip() or rest.lstrip().startswith(';'))
                        and not addr_taken(nm, b) and not walked(nm, b)):
                    f.lines[li] = f'{m.group(1)}{m.group(2)} {m.group(3)}: ref[{inner}]? null{rest}'
                    tally[('ref?', 'local-null-cast')] += 1
                    decisions.append((f.path, li + 1, 'ref?', 'local-null-cast', head, nm))
                    continue

            out, i, changed = [], 0, False
            while True:
                j = line.find('pointer[', i)
                if j < 0:
                    out.append(line[i:]); break
                inner, end = inner_of(line, j)
                head, pos  = head_of(inner), position(line, j)
                verdict = why = None
                nm = name_before(line, j) if pos == 'decl' else None
                if head in STAY_HEADS:   verdict, why = 'pointer', 'stay-type'
                elif extern:             verdict, why = 'pointer', 'extern-decl'
                elif pos == 'cast':
                    # The opaque-slot round trip -- see cast_from_opaque.  The
                    # target follows the SAME nullability rule a local gets,
                    # because an untyped binding takes its type from the cast:
                    # a result the body tests against null is `ref[X]?`.
                    verdict, why = 'pointer', 'cast'
                    if cast_from_opaque(line, j, f, li, void_rets, void_props):
                        b  = f.body(f.owner[li]) if li in f.owner else ''
                        bm = re.match(r'^\s*(?:let|var)\s+(\'[^\']+\'|[A-Za-z_][A-Za-z0-9_]*)\s', line)
                        bn = bm.group(1).strip("'") if bm else None
                        if bn and (addr_taken(bn, b) or walked(bn, b)):
                            verdict, why = 'pointer', 'opaque-cast-walked'
                        elif bn and nulled(bn, b):
                            verdict, why = 'ref?', 'opaque-cast'
                        else:
                            verdict, why = 'ref', 'opaque-cast'
                elif pos == 'generic':   verdict, why = 'pointer', 'generic-arg'
                elif pos == 'other':     verdict, why = 'pointer', 'unclassified'
                elif pos == 'returns':
                    if li not in f.sig:  verdict, why = 'pointer', 'returns-no-body'
                    elif (f.path, li) in retfield:
                        verdict, why = retfield[(f.path, li)]
                    elif f.sig[li][0] in retwalk:
                        verdict, why = 'pointer', 'returns-walked'
                    elif f.sig[li][0] in allocers:
                        verdict, why = 'pointer', 'returns-allocated'
                    elif f.sig[li][0] in retnull:
                        # A nullable RETURN was held at `pointer` for an ABI
                        # reason that no longer exists: `ref[T]?` is
                        # Option[ref[T]], and a function returning one used to
                        # acquire an implicit caller page (contagious across the
                        # whole function -- it is what broke the JIT's ABI-pinned
                        # `jit_*` helpers).  Since 2026-08-24 that page is
                        # decided by the BODY (Planner.body_may_allocate), so a
                        # routine that only hands back storage which already
                        # exists -- the shape nearly every nullable accessor has
                        # -- pays nothing for the honest spelling.
                        verdict, why = 'ref?', 'nullable-return'
                    else:                verdict, why = 'ref', 'return'
                elif nm is None:         verdict, why = 'pointer', 'no-name'
                elif li in f.sig:                                   # PARAMETER
                    b = f.body(li)
                    if (f.path, li, nm) in initptr:
                        verdict, why = 'pointer', 'init-param-mirrors-field'
                    elif addr_taken(nm, b):  verdict, why = 'pointer', 'param-addr-taken'
                    elif walked(nm, b):    verdict, why = 'pointer', 'param-walked'
                    elif heap_buffer(nm, b): verdict, why = 'pointer', 'param-heap-buffer'
                    elif (nm in sig_params(f.lines[li])[0]
                          and (sig_params(f.lines[li])[0].index(nm)
                               - sig_params(f.lines[li])[1]) in sinks.get(f.sig[li][0], ())):
                        verdict, why = 'pointer', 'param-forwarded-to-buffer'
                    elif (f.path, li, nm) in initnull:
                        verdict, why = 'ref?', 'init-param-mirrors-field'
                    elif nulled(nm, b):    verdict, why = 'ref?', 'param-nullable'
                    else:                  verdict, why = 'ref', 'param'
                elif li in f.owner and LOCALDECL.match(line):       # LOCAL
                    b = f.body(f.owner[li])
                    init = line[end:].strip() if end else ''
                    if addr_taken(nm, b):  verdict, why = 'pointer', 'local-addr-taken'
                    elif walked(nm, b):    verdict, why = 'pointer', 'local-walked'
                    elif heap_buffer(nm, b): verdict, why = 'pointer', 'local-heap-buffer'
                    elif init.startswith('null') or nulled(nm, b):
                        verdict, why = 'ref?', 'local-nullable'
                    else:                  verdict, why = 'ref', 'local'
                else:                                               # FIELD
                    rec = f.record_of.get(li)
                    if rec is None:
                        m = re.match(r'^(\s*)define\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', line)
                        rec = m.group(2) if m else None
                    if (rec, nm) in fverdict:
                        verdict, why = fverdict[(rec, nm)]
                    else:
                        addr, walk, dnull = dot_hazard(
                            nm, all_text, scan_scope(nm, rec, rectexts, dotted_foreign))
                        if addr:                 verdict, why = 'pointer', 'field-addr-taken'
                        elif walk:               verdict, why = 'pointer', 'field-walked'
                        elif dnull:              verdict, why = 'ref?', 'field-nullable'
                        else:                    verdict, why = 'ref', 'field-no-record'
                tally[(verdict, why)] += 1
                decisions.append((f.path, li + 1, verdict, why, head, nm or ''))
                if verdict == 'pointer':
                    out.append(line[i:end])
                else:
                    out.append(line[i:j])
                    out.append('ref[' + inner + (']?' if verdict == 'ref?' else ']'))
                    changed = True
                i = end
            if changed:
                f.lines[li] = ''.join(out)

    for k, v in sorted(tally.items(), key=lambda kv: -kv[1]):
        print(f'{v:6d}  {k[0]:8s} {k[1]}')
    print(f'{sum(v for (vd, _), v in tally.items() if vd != "pointer")} sites converted')
    print(f'{len(props)} records, {len(fnull)} nullable (record, property) pairs')
    if apply:
        for f in files:
            open(f.path, 'w', encoding='utf-8').write('\n'.join(f.lines))
        print('APPLIED')
    with open(os.environ.get('REFCONV_LOG', '/tmp/refconv-decisions.txt'), 'w') as fh:
        for d in decisions:
            fh.write('\t'.join(str(x) for x in d) + '\n')

CLASS_B = '--elements' in sys.argv
main(sys.argv[1], apply='--apply' in sys.argv)
