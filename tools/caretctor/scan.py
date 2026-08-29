# -*- coding: utf-8 -*-
"""Find `allocate(sizeof T, alignof T) as pointer[T]` + `set *p: T(...)` sites
that are really `let t T^host(...)` + `&t`.

WHY THIS EXISTS.  `packages/opensp/.../ContentToken.scaly` recorded the reason
the idiom spread across three packages:

    a `^host` construction yields a VALUE, so build a page slot and store the
    constructed ElementType through it

That is false.  `^host` places the OBJECT on host, and `&` on a let-bound
construction is the IDENTITY there -- measured, `Page.get(&t)` answers host and
the reference survives the return.  The IR is the argument: the `^host` form
constructs straight into the page allocation, the allocate form builds a stack
TEMPORARY and copies it into the slot.

★ And the temporary is not merely wasteful.  Two failure modes, both
reproduced:

  (a) On a concept the residency inference treats as heap-backed
      (`StringBuilder`), `set *p: T()` emits `store ptr` -- the ADDRESS of a
      second object -- into the slot, and every later `p.method()` runs on the
      slot.  rc 138.

  (b) On a plain struct whose init carries `^this`, the init's guts land on a
      frame page that is released and reused; four records built in a row all
      read back the fourth one's string.  rc 0, no diagnostic.

★★ But a count is not a verdict, and the tree is CLEAN on (a)/(b) today: its
types use only EMPTY `String()`/`StringC()`/`Array[X]()` in their inits, which
allocate nothing, and dodge the rest by hand with the 244 fieldwise
`set p.f: X^host(...)` sites.  So this tool RANKS rather than alarms -- read
the list, do not count it.

★ The init is matched BY ARGUMENT COUNT.  `StringC()` allocates nothing and
`StringC(chars, n)` allocates through `Page.get(this)`; a scan that keys on the
type name alone reports 63 hazards where there are none.

Usage:  python3 tools/caretctor/scan.py [packages | packages/opensp ...]
        python3 tools/caretctor/scan.py --verdict HAZARD
"""
import re, sys, glob, collections

# ---------------------------------------------------------------- lexing ----
def code(line):
    """The code half of a line: a `;` outside a string starts a comment.

    A hazard scan reads CODE -- a comment in this tree once held 45 live-looking
    declarations, and a prose `get(buf)` reads exactly like a call.
    """
    out = []; i = 0; q = None
    while i < len(line):
        c = line[i]
        if q:
            if c == '\\': i += 2; out.append('  '); continue
            if c == q: q = None
            out.append(c); i += 1; continue
        if c in '"\'': q = c; out.append(c); i += 1; continue
        if c == ';': break
        out.append(c); i += 1
    return ''.join(out)

def load(f):
    return [code(l).rstrip() for l in open(f, encoding='utf-8', errors='replace')]

def block(lines, i):
    """The Allman body starting at or after line i; returns (body, last index)."""
    k = i + 1
    while k < len(lines) and lines[k].strip() == '': k += 1
    if k < len(lines) and lines[k].strip().startswith('{'):
        d = 0; body = []
        while k < len(lines):
            body.append(lines[k]); d += lines[k].count('{') - lines[k].count('}')
            if d <= 0: return body, k
            k += 1
        return body, k
    return ([lines[k]], k) if k < len(lines) else ([], i)

def argc(s):
    """Argument count of the text between the outermost parentheses."""
    s = s.strip()
    if not s: return 0
    d = 0; n = 1; q = None
    for c in s:
        if q:
            if c == q: q = None
            continue
        if c in '"\'': q = c; continue
        if c in '([': d += 1
        elif c in ')]': d -= 1
        elif c == ',' and d == 0: n += 1
    return n

def args_of(text, open_at):
    """The argument text of a call whose '(' sits at open_at, or None."""
    d = 0; q = None
    for k in range(open_at, len(text)):
        c = text[k]
        if q:
            if c == q: q = None
            continue
        if c in '"\'': q = c; continue
        if c == '(': d += 1
        elif c == ')':
            d -= 1
            if d == 0: return text[open_at + 1:k]
    return None

# --------------------------------------------------- concept init records ----
HAZ  = re.compile(r'\^this|Page\.get\s*\(\s*this\b')
CALL = re.compile(r'\b([A-Z]\w*)(\[[^\]]*\])?(\^\w+(?:\.\w+)*)?\s*\(')

def read_inits(files):
    """{package: {concept: {arity: (direct_hazard, [(type, arity), ...])}}}"""
    out = collections.defaultdict(lambda: collections.defaultdict(dict))
    for f in files:
        pkg = f.split('/')[1] if f.startswith('packages/') else '?'
        lines = load(f); cur = None; i = 0
        while i < len(lines):
            c = lines[i]
            m = re.match(r'\s*define\s+(\w+)', c)
            if m: cur = m.group(1)
            m = re.match(r'\s*init\s*(\(.*\))?\s*$', c)
            if cur and m:
                params = m.group(1) or '()'
                body, i = block(lines, i)
                text = '\n'.join(body)
                calls = []
                for mm in CALL.finditer(text):
                    if mm.group(3): continue          # explicit ^page: placed by hand
                    a = args_of(text, mm.end() - 1)
                    if a is None: continue
                    calls.append((mm.group(1), argc(a)))
                out[pkg][cur][argc(params[1:-1])] = (bool(HAZ.search(text)), calls)
            i += 1
    return out

def init_scope(inits, pkg):
    """The concepts visible in `pkg`: its own plus the stdlib (prelude)."""
    scope = dict(inits['scaly']); scope.update(inits[pkg]); return scope


def hazard_table(inits):
    """{package: {(concept, arity): bool}} -- does THIS init allocate on `this`?

    The stdlib is visible everywhere through the prelude, so every package
    inherits `scaly`'s concepts.  Resolution is by arity: `StringC()` allocates
    nothing, `StringC(chars, n)` allocates through `Page.get(this)`.
    """
    table = {}
    for pkg in inits:
        scope = dict(inits['scaly']); scope.update(inits[pkg])
        verdict = {}
        for cname, arities in scope.items():
            for a, (direct, _) in arities.items():
                verdict[(cname, a)] = direct
        changed = True
        while changed:
            changed = False
            for cname, arities in scope.items():
                for a, (_, calls) in arities.items():
                    if verdict[(cname, a)]: continue
                    for t, ta in calls:
                        if verdict.get((t, ta)):
                            verdict[(cname, a)] = True; changed = True; break
        table[pkg] = verdict
    return table

# ------------------------------------------------------------- the sites ----
SITE = re.compile(r'^(\s*)(let|var)\s+(\w+)\s+([\w.]+)\.allocate\s*\('
                  r'\s*sizeof\s+(.+?)\s*,\s*alignof\s+(.+?)\s*\)\s+as\s+pointer\[(.+)\]\s*$')

def enclosing_end(lines, i):
    """Last line of the brace block containing line i (crude but bounded)."""
    d = 0
    for k in range(i, len(lines)):
        d += lines[k].count('{') - lines[k].count('}')
        if d < 0: return k
    return len(lines) - 1

def scan(files, inits, table):
    sites = []
    for f in files:
        pkg = f.split('/')[1] if f.startswith('packages/') else '?'
        scope = init_scope(inits, pkg if pkg in inits else 'scaly')
        # an out-of-tree fixture still gets the stdlib verdicts, which is what
        # makes the HAZARD arm demonstrable (see fixture/ -- a refuter that
        # cannot fire is worse than none).
        haz = table.get(pkg) or table.get('scaly', {})
        lines = load(f)
        for i, c in enumerate(lines):
            m = SITE.match(c)
            if not m: continue
            indent, kw, name, page, sz, al, ptr = m.groups()
            # the store
            store = None; store_at = None
            for k in range(i + 1, min(i + 9, len(lines))):
                if re.search(r'\bset\s+\*' + re.escape(name) + r'\s*:', lines[k]):
                    store = lines[k].strip(); store_at = k; break
                if re.search(r'\bset\s+' + re.escape(name) + r'\s*\.', lines[k]):
                    store = '@FIELDWISE'; store_at = k; break
            if store is None:
                sites.append(dict(f=f, n=i+1, pkg=pkg, verdict='NO-STORE', name=name,
                                  ty=ptr, note='kein `set *%s:` in 8 Zeilen' % name,
                                  site=c.strip(), store='')); continue
            if store == '@FIELDWISE':
                sites.append(dict(f=f, n=i+1, pkg=pkg, verdict='FIELDWISE', name=name,
                                  ty=ptr, note='handgebauter Konstruktor (%d Felder folgen)'
                                  % sum(1 for k in range(store_at, enclosing_end(lines, i))
                                        if re.search(r'\bset\s+' + re.escape(name) + r'\s*\.', lines[k])),
                                  site=c.strip(), store=lines[store_at].strip())); continue
            rhs = store.split(':', 1)[1].strip()
            mm = CALL.match(rhs)
            if not mm:
                sites.append(dict(f=f, n=i+1, pkg=pkg, verdict='COPY', name=name, ty=ptr,
                                  note='Wertkopie, kein init laeuft', site=c.strip(),
                                  store=store)); continue
            base, gen, sigil = mm.group(1), mm.group(2) or '', mm.group(3)
            a = args_of(rhs, mm.end() - 1)
            n_args = argc(a) if a is not None else 0
            ctor = base + gen
            if sz.strip() != al.strip() or sz.strip() != ptr.strip():
                note = 'sizeof/alignof/pointer nennen nicht denselben Typ'
                sites.append(dict(f=f, n=i+1, pkg=pkg, verdict='SKIP', name=name, ty=ptr,
                                  note=note, site=c.strip(), store=store)); continue
            if ctor != ptr.strip():
                sites.append(dict(f=f, n=i+1, pkg=pkg, verdict='SKIP', name=name, ty=ptr,
                                  note='konstruierter Typ %s <> Slot-Typ %s' % (ctor, ptr),
                                  site=c.strip(), store=store)); continue
            uses = use_profile(lines, store_at, name)
            if sigil:
                v, note = 'SIGIL', 'Konstruktion traegt schon %s' % sigil
            elif n_args not in scope.get(base, {}):
                # ★ MEASURED, and it is a COMPILER defect, not a style question:
                # `&T^page()` on a concept with no matching `init` takes the
                # POSITIONAL-TUPLE path, which DROPS the sigil --
                #     %tuple = alloca %NoInit
                #     store %NoInit zeroinitializer, ptr %tuple
                #     ret ptr %tuple
                # a pointer into the dead frame, rc 0, nothing said.  The
                # `allocate` + `set *p:` form copies that tuple onto the page
                # and is CORRECT.  opensp's `EventAux` ("a data class with no
                # init of its own") is exactly this shape and took the unit
                # suite to rc 139 in `Lpd.get_name`.
                v, note = 'NO-INIT', ('%s hat keinen init/%d -- die Tupel-Bahn '
                                      'verliert das ^page-Sigil (Compiler-Defekt)'
                                      % (base, n_args))
            elif haz.get((base, n_args)):
                v, note = 'HAZARD', ('init %s/%d allokiert ueber `this` -- das Temporary '
                                     'ist hier nicht nur Verschwendung' % (base, n_args))
            else:
                v, note = 'CONVERT', 'init %s/%d allokiert nichts ueber `this`' % (base, n_args)
            sites.append(dict(f=f, n=i+1, pkg=pkg, verdict=v, name=name, ty=ptr, page=page,
                              note=note, site=c.strip(), store=store, uses=uses))
    return sites

DEREF = None
def use_profile(lines, store_at, name):
    """How the bound name is used below -- what a rewrite would have to survive.

    `&t` yields `ref[T]`, so a site whose pointer is DEREFERENCED or handed to a
    `pointer[T]` position is not a free swap; `*p` on a non-optional ref is a
    hard rc-4 since the fifth gate landed.
    """
    end = enclosing_end(lines, store_at)
    w = re.escape(name)
    prof = collections.Counter()
    for k in range(store_at + 1, end + 1):
        c = lines[k]
        if not re.search(r'\b' + w + r'\b', c): continue
        if re.search(r'\*\s*' + w + r'\b', c):            prof['deref'] += 1
        if re.search(r'\b' + w + r'\s*\+', c):            prof['arith'] += 1
        if re.search(r'\b' + w + r'\s*\[', c):            prof['subscript'] += 1
        if re.search(r'\b' + w + r'\s*\.', c):            prof['member'] += 1
        if re.search(r'\breturn\s+' + w + r'\b', c):      prof['return'] += 1
        prof['mentions'] += 1
    return prof

SELFTEST = [(('StringBuilder', 0), True,  'init() does `Array[char]^this()`'),
            (('String',        1), True,  'String(cstring) allocates via Page.get(this)'),
            (('StringC',       0), False, 'the EMPTY StringC allocates nothing'),
            (('StringC',       2), True,  'StringC(chars, n) allocates via Page.get(this)'),
            (('Array',         0), False, 'the EMPTY Array allocates nothing')]

def selftest(table):
    """A checker that cannot fail is worse than none -- prove the arity split."""
    ref = dict(table.get('opensp', {}))
    ref.update({k: v for k, v in table.get('scaly', {}).items() if k not in ref})
    bad = []
    for key, want, why in SELFTEST:
        got = ref.get(key)
        if got is None or bool(got) != want: bad.append((key, want, got, why))
    return bad


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    only = None
    for a in sys.argv[1:]:
        if a.startswith('--verdict'):
            only = a.split('=', 1)[1] if '=' in a else sys.argv[sys.argv.index(a) + 1]
    roots = args or ['packages']
    files = sorted({f for r in roots
                    for f in ([r] if r.endswith('.scaly') else
                              glob.glob(r + '/**/*.scaly', recursive=True))})
    allf  = sorted(glob.glob('packages/**/*.scaly', recursive=True))
    inits = read_inits(allf)
    table = hazard_table(inits)
    bad = selftest(table)
    if bad:
        print('SELFTEST FAILED -- the arity split is not working, every verdict below is void:')
        for key, want, got, why in bad:
            print('   %s/%d  erwartet %s, bekommen %s  (%s)' % (key[0], key[1], want, got, why))
        sys.exit(2)
    sites = scan(files, inits, table)

    order = ['CONVERT', 'NO-INIT', 'HAZARD', 'FIELDWISE', 'COPY', 'SIGIL', 'NO-STORE', 'SKIP']
    for v in order:
        rows = [s for s in sites if s['verdict'] == v]
        if not rows or (only and v != only): continue
        print('=== %s: %d ===' % (v, len(rows)))
        for s in rows:
            u = s.get('uses') or {}
            flag = ' '.join('%s:%d' % (k, u[k]) for k in ('deref', 'arith', 'subscript') if u.get(k))
            print('  %s:%d  [%s]%s' % (s['f'], s['n'], s['ty'], ('  !! ' + flag) if flag else ''))
            print('      %s' % s['site'])
            if s['store']: print('      %s' % s['store'])
            print('      -> %s' % s['note'])
        print()
    print('--- %d Sites' % len(sites))
    for v in order:
        n = sum(1 for s in sites if s['verdict'] == v)
        if n: print('  %6d  %s' % (n, v))
    free = [s for s in sites if s['verdict'] == 'CONVERT' and not (s.get('uses') or {}).get('deref')
            and not (s.get('uses') or {}).get('arith') and not (s.get('uses') or {}).get('subscript')]
    print('  %6d  davon CONVERT ohne Deref/Arithmetik/Subscript auf dem Namen' % len(free))

if __name__ == '__main__':
    main()
