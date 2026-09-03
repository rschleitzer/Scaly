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


# ★★★ Which values may MOVE, and the first answer was too weak.
#
# The tempting rule is "a value without a call is pure, so it may move".  It is
# WRONG: a member read is call-free and still observes state, so hoisting
# `b: this.counter` in FRONT of `a: bump(this)` reads a different value.  A
# name can be rebound, a member can be written, a global can be mutable -- and
# nothing in a line-shaped tool can see which.
#
# So the rule is the one that needs no analysis: **only a LITERAL may move.**
# A literal has no evaluation at all -- a number, `true`/`false`, `null`, a
# string or char literal, and arithmetic over those (`0 - 1`).  Everything else
# keeps its relative position, and a permutation that disturbs that order is
# not converted.
#
# ★ Measured over the tree's 26 REORDER sites this is not a theoretical
# tightening: it is what the opensp `Event` family needs and gets (the swapped
# pairs there are `false, false` against `null, null`), and it is what refuses
# the MifFOTBuilder records, whose moved values are `String()` constructions.
LITERAL = re.compile(r"""^(?:\d+|0[xX][0-9a-fA-F]+|true|false|null|"[^"]*"|\'[^\']*\')$""")


def is_literal(v):
    """No evaluation whatsoever: a literal, or arithmetic over literals."""
    v = v.strip()
    if v.startswith('(') and v.endswith(')'): v = v[1:-1].strip()
    parts = [p for p in re.split(r'\s*[-+*/%]\s*', v) if p]
    return bool(parts) and all(LITERAL.match(p) for p in parts)


NAME = re.compile(r'^[a-z_]\w*$')
CONSTS = set()          # module-level `define NAME: T v` -- compile-time constant
MUTABLE = set()         # `mutable` / `shared` -- a call may write it


def read_globals(files):
    """★ A `define` constant and a `mutable` global read IDENTICALLY at a use
    site, and they answer the movability question in opposite directions: the
    first is fixed at compile time and may travel anywhere, the second is a
    cell any sibling call can write.  Only the DECLARATION separates them, so
    it is looked up rather than guessed from the name's shape."""
    for f in files:
        for c in S.load(f):
            m = re.match(r'define\s+([A-Za-z_]\w*)\s*:', c)
            if m: CONSTS.add(m.group(1)); continue
            m = re.match(r'(?:mutable|shared)\s+([A-Za-z_]\w*)\s*:', c)
            if m: MUTABLE.add(m.group(1))


def is_movable(v, body):
    """May this value change position without changing what the program reads?

    A LITERAL may, trivially.  ★ So may a BARE LOCAL NAME -- a parameter or a
    `let`/`var` -- because a stack slot can only be written from elsewhere if
    its ADDRESS escapes, and that is spelled `&name` and nothing else.  So the
    condition is mechanical: the enclosing body contains no `&name`.

    ★★★ A MEMBER READ is deliberately NOT movable even though it is call-free.
    `mc.s_black` reads through a reference, and any sibling call holding that
    reference may write the field between the two positions.  That is the
    distinction the first version of this rule got wrong by asking "does it
    contain a call" -- purity of the EXPRESSION says nothing about the
    stability of what it READS.
    """
    v = v.strip()
    if is_literal(v): return True
    if v in CONSTS and v not in MUTABLE: return True
    if not NAME.match(v): return False
    if v in MUTABLE: return False
    return not re.search(r'&\s*' + re.escape(v) + r'\b', body)


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
    # ★ A run that stops at the first non-`set` line UNDERSTATES the site: the
    # big initialisers interleave value-producing statements between their
    # field assignments, and asking only about the leading run reports the rest
    # as "unset without default" -- 32 sites read that way, and in 31 of them
    # every field IS set, just further down.  So re-collect over the WHOLE
    # block and let hoist_verdict# decide whether the construction can move to
    # where the last assignment is.
    whole, others = [], []
    for k in range(i + 1, min(end + 1, len(lines))):
        c = lines[k]
        if not c.strip(): continue
        m = SET.match(c)
        if m and m.group(1) == name:
            whole.append((m.group(2), m.group(3), k)); continue
        others.append(k)
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
    # ★★★ CONTIGUITY IS ASKED FIRST, AND FOR BOTH ORDERS.  Getting this wrong
    # is what a REORDER branch invites: the set fields being a PERMUTATION and
    # their being a PREFIX are different questions, and `Event.scaly`'s
    # `make_start_element_no_aux` is exactly the pair that separates them -- it
    # sets nine fields in a swapped order and SKIPS `aux`, so a check that asks
    # about order alone calls it REORDER and the positional path then reports
    # `property has_markup would be left uninitialized`.
    if sorted(idx) != list(range(len(idx))) or [f for f, hd in decl[len(idx):] if not hd]:
        pay, why = hoist_verdict(name, decl, whole, others, lines, i, end)
        if pay: return 'HOIST', pay, why
        if sorted(idx) != list(range(len(idx))):
            return 'GAP', None, 'Luecke: %s' % why
        return 'NODEFAULT', None, why
    if idx != sorted(idx):
        # ★ The permutation is not the question -- EVALUATION ORDER is.  A
        # reorder is safe exactly when no two values that CAN have a side
        # effect swap relative position; pure values (a literal, a name, a
        # member chain) may move freely.
        # ★ the `&name` search must cover the WHOLE FILE, not the block below
        # the allocation: an address taken EARLIER in the same function is
        # exactly as escaping, and a block-scoped search would not see it.
        # Over-conservative on purpose -- a false hold costs a site, a false
        # move costs a wrong read.
        body = '\n'.join(lines)
        evald = [k for k, (_, v, _) in enumerate(got) if not is_movable(v, body)]
        moved = [idx[k] for k in evald]
        safe = moved == sorted(moved)
        # the payload is the argument list in DECLARATION order -- that is what
        # the positional path fills, and the whole point of the verdict
        ordered = [g for _, g in sorted(zip(idx, got), key=lambda x: x[0])]
        return ('REORDER-LIT' if safe else 'REORDER-EVAL'), (ordered, last), \
               ('nur Literale/ungebundene Namen bewegen sich (%d ortsfeste Werte '
                'behalten ihre Reihenfolge)' % len(evald)) if safe else \
               ('ausgewertete Werte tauschen die Reihenfolge: %s vs. %s'
                % (seen, order[:len(seen)]))
    return 'ORDERED', (got, last), '%d Felder, Praefix der Deklaration (%d)' % (len(got), len(decl))



def hoist_verdict(name, decl, whole, others, lines, i, end):
    """Can the construction move DOWN to where the last assignment is?

    Conditions, each of which failed on a real site while this was written:

      * every declared field set EXACTLY once, or unset ones carry a default
        (a field set twice has no single argument);
      * no statement between the allocation and the last assignment may MENTION
        the bound name -- otherwise the object is read before it exists;
      * ★ and the evaluation rule of the REORDER pass, now applied to the
        interleaved statements as well: a value that must travel PAST anything
        that is itself evaluated has to be movable (a literal, or a bare local
        whose address is never taken).  The value AT the last assignment never
        travels and is therefore free.
    """
    seen = [f for f, _, _ in whole]
    if len(set(seen)) != len(seen):
        return None, 'ein Feld wird mehrfach gesetzt'
    order = [f for f, _ in decl]
    if any(f not in order for f in seen):
        return None, 'gesetztes Feld nicht in der Deklaration'
    idx = [order.index(f) for f in seen]
    if sorted(idx) != list(range(len(idx))):
        return None, 'auch ueber den ganzen Block keine Praefix-Ueberdeckung'
    nodef = [f for f, hd in decl[len(idx):] if not hd]
    if nodef:
        return None, 'ungesetzt ohne Default: %s' % ', '.join(nodef)
    last = max(k for _, _, k in whole)
    body = '\n'.join(lines)
    for k in others:
        if k > last: continue
        if re.search(r'\b%s\b' % re.escape(name), lines[k]):
            return None, 'Zeile %d nennt `%s` vor der Konstruktion' % (k + 1, name)
    barrier = sorted([k for k in others if k < last] +
                     [k for _, v, k in whole if k < last and not is_movable(v, body)])
    for f, v, k in whole:
        if k == last: continue
        if any(b > k for b in barrier) and not is_movable(v, body):
            return None, 'ortsfester Wert `%s` muesste an einer Auswertung vorbei' % f
    ordered = [g for _, g in sorted(zip(idx, whole), key=lambda x: x[0])]
    return (ordered, last), '%d Felder ueber %d Zwischenzeilen eingesammelt' % (
        len(whole), len([k for k in others if k < last]))


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
    read_globals(allf)
    sites = [s for s in S.scan(files, inits, table, False) if s['verdict'] == 'FIELDWISE']
    out = []
    for s in sites:
        lines = S.load(s['f'])
        v, payload, note = analyse(s, lines, fields)
        out.append(dict(s, fw=v, payload=payload, fwnote=note))
    return out


JOURNAL = []
JOURNAL_PATH = 'tools/caretctor/.journal.json'

ORDER = ['ORDERED', 'REORDER-LIT', 'HOIST', 'REORDER-EVAL', 'GAP', 'NODEFAULT', 'INTERLEAVED', 'SELFREF', 'NOFIELDS']


def rewrite(sites, apply_, reorder_eval_ok=False):
    ok = ['ORDERED', 'REORDER-LIT', 'HOIST']
    # ★ `--reorder-eval-ok` is a READER's verdict, not a widening of the rule.
    # The tool holds a REORDER site whenever any value it cannot prove pure
    # changes relative position, and an `A | B` fold of two module-level
    # `define`s is exactly such a value -- pure in fact, opaque to the scan.
    # Measured over the tree's four REORDER-EVAL sites, all in MifFOTBuilder:
    # the ONLY field out of declaration order is `set_props`, its value is that
    # constant fold, and it crosses nothing evaluated in two of them and one
    # call in the other two -- with no second call to swap against.  The flag
    # exists so that judgement is written down and re-checked per run, the way
    # convert.py's `--no-init-ok` is.
    if reorder_eval_ok: ok.append('REORDER-EVAL')
    todo = collections.defaultdict(list)
    for s in sites:
        if s['fw'] not in ok: continue
        raw = open(s['f'], encoding='utf-8').read().split('\n')
        code = S.load(s['f'])
        i = s['n'] - 1
        if not code[i].strip().startswith('let '): continue
        got, last = s['payload']
        if s['fw'] == 'HOIST' and any(k > last for _, _, k in got): continue
        m = S.SITE.match(code[i])
        if not m: continue
        page = m.group(4)          # FIELDWISE sites carry no 'page' -- scan.py
        indent = m.group(1)        # classifies and continues before setting it
        # ★★★ THE VALUE MUST COME FROM THE RAW LINE, NOT FROM `code()`.
        # scan.code# replaces an escape INSIDE a string by two spaces so that a
        # `;` in a literal cannot read as a comment -- length-preserving, which
        # is what makes it safe to scan and fatal to write back.  Measured: it
        # turned `String^host("<$pagenum\\>")` into `String^host("<$pagenum  >")`
        # in MifFOTBuilder, silently, and the file still compiled.  The offsets
        # agree because the normalisation preserves length, so the fix is to
        # slice `raw` at the span the match found in `code`.
        vals = []
        for fld, v, k in got:
            mk = SET.match(code[k])
            vals.append(raw[k][mk.start(3):mk.end(3)] if mk else v)
        args = ', '.join(vals)
        ty = s['ty'].strip()
        new = '%slet %s &%s^%s(%s)' % (indent, s['name'], ty, page, args)
        # carry any trailing comments of the replaced lines
        tail = []
        for k in [i] + [k for _, _, k in got]:
            c = raw[k]
            if len(c) > len(code[k]) and ';' in c[len(code[k]):]:
                tail.append(c[len(code[k]):].strip())
        # ★ HOIST is not a contiguous run: the value-producing lines BETWEEN the
        # assignments must stay where they are.  So the splice is not
        # `raw[i:last+1] = [new]` -- it deletes the allocation and every
        # assignment line, and puts the construction where the LAST one was.
        if s['fw'] == 'HOIST':
            kill = [i] + [k for _, _, k in got if k != last]
            todo[s['f']].append((last, last, new, tail, kill))
        else:
            todo[s['f']].append((i, last, new, tail, []))
    for f, items in sorted(todo.items()):
        raw = open(f, encoding='utf-8').read().split('\n')
        orig = list(raw)
        for i, last, new, tail, kill in sorted(items, reverse=True):
            block = [new] + ['%s; %s' % (new[:len(new) - len(new.lstrip())], t.lstrip('; ')) for t in tail]
            raw[i:last + 1] = block
            for k in sorted(kill, reverse=True):
                del raw[k]
        if apply_:
            open(f, 'w', encoding='utf-8').write('\n'.join(raw))
            # ★ The journal is what makes a revert EXACT.  retry.py used to find
            # the original block by matching indent + binding name in
            # `git show HEAD:<file>` and disambiguating by LINE NUMBER -- in a
            # file whose lines have already shifted, and where `Event.scaly`
            # has fourteen `let ev host.allocate(...)` at one indent.  It
            # spliced a block back into the WRONG function, and the tell was a
            # cascade of `function not found: src.data_ptr` -- a name that was
            # never in scope there.  Match by CONTENT, never by position.
            for i, last, new, tail, kill in items:
                JOURNAL.append(dict(f=f, new=new, orig=orig[i:last + 1]))
    if apply_ and JOURNAL:
        import json
        with open(JOURNAL_PATH, 'w', encoding='utf-8') as fh:
            json.dump(JOURNAL, fh, indent=1)
    return todo


def main():
    roots = [a for a in sys.argv[1:] if not a.startswith('--')] or ['packages']
    only = None
    for a in sys.argv[1:]:
        if a.startswith('--verdict='): only = a.split('=', 1)[1]
    sites = collect(roots)
    apply_ = '--apply' in sys.argv
    reorder_eval_ok = '--reorder-eval-ok' in sys.argv
    show = '--apply' in sys.argv or '--show' in sys.argv or only
    if show:
        for v in ORDER:
            rows = [s for s in sites if s['fw'] == v]
            if not rows or (only and v != only): continue
            print('=== %s: %d ===' % (v, len(rows)))
            for s in rows:
                print('  %s:%d  [%s]  -> %s' % (s['f'], s['n'], s['ty'], s['fwnote']))
            print()
    todo = rewrite(sites, apply_, reorder_eval_ok)
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
