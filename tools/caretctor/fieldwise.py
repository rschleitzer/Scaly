#!/usr/bin/env python3
"""FIELDWISE -> the IMPLICIT init: `&T^host(v1, v2, ...)`.

`tools/caretctor/scan.py`'s FIELDWISE verdict is a DEAD END -- line 206
classifies and does `continue`, so it never asked whether the target type has
an `init`.  Measured 2026-09-01 over all 243 sites: **235 have none at all**,
which is why the CONVERT campaign stood at 5 and not at 248.

★★★ The answer is NOT to write 235 inits.  A `define T (f1: A, f2: B, ...)` IS
the constructor: a construction matching no `init#` is filled POSITIONALLY in
DECLARATION ORDER, `Emitter.region_alloc_tuple#` has honoured the `^page`
sigil on that path since 2026-08-29, and `Planner.report_component_type_
mismatch#` has type-checked the components since 2026-08-30.  So

    let p host.allocate(sizeof T, alignof T) as pointer[T]
    set p.f1: a
    set p.f2: b

is exactly `let p &T^host(a, b)`, and the IR is the same allocate + GEP + store
per field -- no temporary either way.  What the conversion BUYS is the
component type check, which the field-wise form does not get, and one
expression instead of N+1 statements.

★ What it COSTS is the thing to keep in view: the positional form binds to
FIELD ORDER.  Reordering the `define` silently re-aims every positional
construction whose neighbouring fields happen to share a type.  That risk is
the language's, not this tool's -- it already applies to every `COPY` site and
every existing positional construction -- but it is the reason this tool
refuses to REORDER arguments itself (see below).

## The verdicts

  ORDERED     the sets are a contiguous PREFIX of the declaration, each field
              once, in declaration order, nothing else touching the name in
              between, and any unset TRAILING fields all carry defaults.
              The one convertible class: the arguments are the written values
              in the written order, so evaluation order is unchanged.

  REORDER     the same fields, but not in declaration order.  NOT converted.
              Reordering the arguments reorders EVALUATION, and a value here
              can be a call (`Array[int]^host()`, `AstNode.pos_of(r)`).  The
              reader decides; the tool must not.

  GAP         a field in the middle is left unset.  Positional filling cannot
              skip, and `fill_default_components#` only fills a TRAILING run.

  NODEFAULT   an unset trailing field with no default in the `define`.

  INTERLEAVED a statement between the sets that is not one of them.

  SELFREF     a value that mentions the bound name -- the object must exist
              first, so there is no single construction.

  NOFIELDS    the `define` for the type was not found, or declares no field
              list (a body-only concept).

Usage:
    python3 tools/caretctor/fieldwise.py [packages | packages/opensp ...]
    python3 tools/caretctor/fieldwise.py --verdict=ORDERED
    python3 tools/caretctor/fieldwise.py packages/opensp --apply
"""
import sys, os, re, glob, collections
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import scan as S


def split_top(text, seps=',\n'):
    """Split at depth 0 with respect to () and []."""
    out, cur, d = [], [], 0
    for c in text:
        if c in '([': d += 1
        elif c in ')]': d -= 1
        if d == 0 and c in seps:
            out.append(''.join(cur)); cur = []; continue
        cur.append(c)
    out.append(''.join(cur))
    return [x.strip() for x in out if x.strip()]


def read_fields(files):
    """{package: {concept: [(name, has_default), ...]}} from `define T (...)`."""
    out = collections.defaultdict(dict)
    for f in files:
        pkg = f.split('/')[1] if f.startswith('packages/') else '?'
        lines = S.load(f)
        for i, c in enumerate(lines):
            m = re.match(r'\s*define\s+(\w+)\s*(.*)$', c)
            if not m: continue
            name, rest = m.group(1), m.group(2)
            # gather the balanced (...) starting on this line or the next ones
            text, d, started = [], 0, False
            k, seg = i, rest
            while k < len(lines) and k < i + 400:
                for ch in seg:
                    if ch == '(':
                        d += 1; started = True
                        if d == 1: continue
                    elif ch == ')':
                        d -= 1
                        if d == 0: break
                    if started and d >= 1: text.append(ch)
                if started and d == 0: break
                if not started and seg.strip() and not seg.strip().startswith('('):
                    break                      # a body-only concept: `define T` + `{`
                text.append('\n')
                k += 1
                seg = lines[k] if k < len(lines) else ''
            if not started:
                out[pkg].setdefault(name, None); continue
            fields = []
            for ent in split_top(''.join(text)):
                mm = re.match(r'^(\w+)\s*:\s*(.+)$', ent)
                if not mm: continue
                ty = mm.group(2).strip()
                # a default is a trailing (...) after a complete type
                has_def = bool(re.search(r'\)\s*$', ty) and not ty.endswith(']'))
                if has_def:
                    # `ref[X]?(null)` -> default;  `HashMap[K, V]` -> not
                    base = re.sub(r'\([^()]*\)\s*$', '', ty)
                    has_def = base.strip() != '' and not base.strip().endswith(',')
                fields.append((mm.group(1), has_def))
            out[pkg][name] = fields
    return out


SET = re.compile(r'^\s*set\s+(\w+)\.(\w+)\s*:\s*(.+?)\s*$')


def analyse(site, lines, fields_by_pkg):
    name, ty = site['name'], site['ty'].strip()
    base = re.sub(r'\[.*', '', ty)
    pkg = site['pkg']
    decl = (fields_by_pkg.get(pkg) or {}).get(base)
    if decl is None:
        for p in fields_by_pkg:                      # a type from another package
            if fields_by_pkg[p].get(base): decl = fields_by_pkg[p][base]; break
    if not decl:
        return 'NOFIELDS', None, 'kein Feldliste-`define` fuer %s gefunden' % base

    i = site['n'] - 1
    end = S.enclosing_end(lines, i)
    got, last = [], i
    for k in range(i + 1, min(end + 1, len(lines))):
        c = lines[k]
        if not c.strip(): continue
        m = SET.match(c)
        if m and m.group(1) == name:
            got.append((m.group(2), m.group(3), k)); last = k; continue
        if re.search(r'\b%s\b' % re.escape(name), c):
            # the first use of the name after the sets ends the run
            break
        if got: break                                 # INTERLEAVED, judged below
        break
    if not got:
        return 'NOFIELDS', None, 'keine `set %s.feld:` gefunden' % name
    # interleaving: the sets must be consecutive non-blank lines
    ks = [k for _, _, k in got]
    for a, b in zip(ks, ks[1:]):
        for k in range(a + 1, b):
            if lines[k].strip():
                return 'INTERLEAVED', None, 'Anweisung in Zeile %d zwischen den Zuweisungen' % (k + 1)
    seen = [g[0] for g in got]
    if len(set(seen)) != len(seen):
        return 'INTERLEAVED', None, 'ein Feld wird mehrfach gesetzt'
    for _, v, _ in got:
        if re.search(r'\b%s\b' % re.escape(name), v):
            return 'SELFREF', None, 'ein Wert nennt `%s` selbst' % name
    order = [f for f, _ in decl]
    idx = [order.index(f) for f in seen if f in order]
    if len(idx) != len(seen):
        miss = [f for f in seen if f not in order]
        return 'NOFIELDS', None, 'gesetztes Feld nicht in der Deklaration: %s' % ', '.join(miss)
    if idx != sorted(idx):
        return 'REORDER', None, 'Reihenfolge %s vs. Deklaration %s' % (seen, order[:len(seen)])
    if idx != list(range(len(idx))):
        return 'GAP', None, 'Luecke: gesetzt %s, Deklaration %s' % (seen, order[:max(idx) + 1])
    rest = decl[len(idx):]
    nodef = [f for f, hd in rest if not hd]
    if nodef:
        return 'NODEFAULT', None, 'ungesetzt ohne Default: %s' % ', '.join(nodef)
    return 'ORDERED', (got, last), '%d Felder, Praefix der Deklaration (%d)' % (len(got), len(decl))


def collect(roots):
    files = sorted({f for r in roots
                    for f in ([r] if r.endswith('.scaly') else
                              glob.glob(r + '/**/*.scaly', recursive=True))})
    allf = sorted(glob.glob('packages/**/*.scaly', recursive=True))
    inits = S.read_inits(allf); table = S.hazard_table(allf and inits)
    bad = S.selftest(table)
    if bad:
        print('SELFTEST FAILED -- refusing to report'); sys.exit(2)
    fields = read_fields(allf)
    sites = [s for s in S.scan(files, inits, table, False) if s['verdict'] == 'FIELDWISE']
    out = []
    for s in sites:
        lines = S.load(s['f'])
        v, payload, note = analyse(s, lines, fields)
        out.append(dict(s, fw=v, payload=payload, fwnote=note))
    return out


ORDER = ['ORDERED', 'REORDER', 'GAP', 'NODEFAULT', 'INTERLEAVED', 'SELFREF', 'NOFIELDS']


def rewrite(sites, apply_):
    todo = collections.defaultdict(list)
    for s in sites:
        if s['fw'] != 'ORDERED': continue
        raw = open(s['f'], encoding='utf-8').read().split('\n')
        code = S.load(s['f'])
        i = s['n'] - 1
        if not code[i].strip().startswith('let '): continue
        got, last = s['payload']
        m = S.SITE.match(code[i])
        if not m: continue
        page = m.group(4)          # FIELDWISE sites carry no 'page' -- scan.py
        indent = m.group(1)        # classifies and continues before setting it
        args = ', '.join(v for _, v, _ in got)
        ty = s['ty'].strip()
        new = '%slet %s &%s^%s(%s)' % (indent, s['name'], ty, page, args)
        # carry any trailing comments of the replaced lines
        tail = []
        for k in [i] + [k for _, _, k in got]:
            c = raw[k]
            if len(c) > len(code[k]) and ';' in c[len(code[k]):]:
                tail.append(c[len(code[k]):].strip())
        todo[s['f']].append((i, last, new, tail))
    for f, items in sorted(todo.items()):
        raw = open(f, encoding='utf-8').read().split('\n')
        for i, last, new, tail in sorted(items, reverse=True):
            block = [new] + ['%s; %s' % (new[:len(new) - len(new.lstrip())], t.lstrip('; ')) for t in tail]
            raw[i:last + 1] = block
        if apply_: open(f, 'w', encoding='utf-8').write('\n'.join(raw))
    return todo


def main():
    roots = [a for a in sys.argv[1:] if not a.startswith('--')] or ['packages']
    only = None
    for a in sys.argv[1:]:
        if a.startswith('--verdict='): only = a.split('=', 1)[1]
    sites = collect(roots)
    apply_ = '--apply' in sys.argv
    show = '--apply' in sys.argv or '--show' in sys.argv or only
    if show:
        for v in ORDER:
            rows = [s for s in sites if s['fw'] == v]
            if not rows or (only and v != only): continue
            print('=== %s: %d ===' % (v, len(rows)))
            for s in rows:
                print('  %s:%d  [%s]  -> %s' % (s['f'], s['n'], s['ty'], s['fwnote']))
            print()
    todo = rewrite(sites, apply_)
    n = sum(len(v) for v in todo.values())
    print('--- %d FIELDWISE-Sites' % len(sites))
    for v in ORDER:
        k = sum(1 for s in sites if s['fw'] == v)
        if k: print('  %6d  %s' % (k, v))
    per = collections.Counter(s['pkg'] for s in sites if s['fw'] == 'ORDERED')
    if per: print('  ORDERED je Paket: ' + ', '.join('%s %d' % kv for kv in per.most_common()))
    print('%d Sites %s' % (n, 'GESCHRIEBEN' if apply_ else 'umschreibbar (--apply zum Schreiben)'))


if __name__ == '__main__':
    main()
