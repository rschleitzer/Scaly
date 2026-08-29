# -*- coding: utf-8 -*-
"""Rewrite `allocate` + `set *p:` into the one-line `&T^page(...)` construction.

    let p host.allocate(sizeof T, alignof T) as pointer[T]
    set *p: T(args)
        ->
    let p &T^host(args)

Measured equivalent and strictly better IR -- the `^page` form constructs
straight into the page allocation while the old form emits an `alloca` plus a
full struct copy:

    %struct.region = call @Page.allocate(%forced_page, sizeof, alignof)
    call @ThingC1(ptr %struct.region, ...)
    ret ptr %struct.region

★ THIS IS NOT EMISSION-NEUTRAL and must not be checked as if it were.  The gate
is `tools/opcheck.py` old-against-new: exactly the enclosing functions may
differ, nothing may be unpaired, and no symbol may be renamed -- a rename here
would mean a signature moved, which this rewrite never does.

★★ What the converter REFUSES, each for a reason that cost something elsewhere:

  * anything the scanner does not call CONVERT -- a `HAZARD`, a `COPY` (no init
    runs), a `FIELDWISE` (a hand-built constructor, a different job);
  * a store that is not the IMMEDIATELY following line -- the lines between may
    use or re-store the name, and a sweeper that is exact about WHICH SITES is
    still blind to what sits between them;
  * a `var` binding -- it can be rebound below, and `&t` is not a slot;
  * a name that is DEREFERENCED, walked or subscripted below.  `&t` yields
    `ref[T]`, and `*r` on a non-optional ref is a hard rc-4 since the fifth
    gate; those sites read the value at a call argument and are a hand edit.
  * an unbalanced store line, so a construction wrapped over two lines is never
    half-rewritten.

Usage:  python3 tools/caretctor/convert.py packages/opensp          # dry run
        python3 tools/caretctor/convert.py packages/opensp --apply
"""
import sys, glob, collections
sys.path.insert(0, __file__.rsplit('/', 1)[0])
import scan as S


def balanced(s):
    d = 0; q = None
    for c in s:
        if q:
            if c == q: q = None
            continue
        if c in '"\'': q = c; continue
        if c in '([': d += 1
        elif c in ')]': d -= 1
    return d == 0


def plan(roots):
    files = sorted({f for r in roots
                    for f in ([r] if r.endswith('.scaly') else
                              glob.glob(r + '/**/*.scaly', recursive=True))})
    allf  = sorted(glob.glob('packages/**/*.scaly', recursive=True))
    inits = S.read_inits(allf)
    table = S.hazard_table(inits)
    bad = S.selftest(table)
    if bad:
        print('SELFTEST FAILED -- refusing to convert'); sys.exit(2)
    sites = S.scan(files, inits, table)

    todo = collections.defaultdict(list); held = collections.Counter()
    for s in sites:
        if s['verdict'] != 'CONVERT':
            held[s['verdict']] += 1; continue
        raw  = open(s['f'], encoding='utf-8').read().split('\n')
        code = S.load(s['f'])
        i = s['n'] - 1
        if not code[i].strip().startswith('let '):
            held['var-Bindung'] += 1; continue
        if i + 1 >= len(code) or code[i + 1].strip() != s['store']:
            held['store nicht in der naechsten Zeile'] += 1; continue
        u = s.get('uses') or {}
        if u.get('deref') or u.get('arith') or u.get('subscript'):
            held['Deref/Arithmetik/Subscript auf dem Namen'] += 1; continue
        if not balanced(code[i + 1]):
            held['Store-Zeile nicht geklammert'] += 1; continue
        # the construction, verbatim from the store's RHS
        rhs = s['store'].split(':', 1)[1].strip()
        m = S.CALL.match(rhs)
        args = S.args_of(rhs, m.end() - 1)
        if args is None:
            held['Argumente nicht lesbar'] += 1; continue
        indent = code[i][:len(code[i]) - len(code[i].lstrip())]
        ty = (m.group(1) or '') + (m.group(2) or '')
        new = '%slet %s &%s^%s(%s)' % (indent, s['name'], ty, s['page'], args.strip())
        # a trailing comment on either original line is carried onto the result
        tail = ''
        for k in (i, i + 1):
            c = raw[k]
            if len(c) > len(code[k]) and ';' in c[len(code[k]):]:
                tail += '  ' + c[len(code[k]):].strip()
        todo[s['f']].append((i, new + tail))
    return todo, held


def main():
    roots = [a for a in sys.argv[1:] if not a.startswith('--')] or ['packages']
    apply_ = '--apply' in sys.argv
    todo, held = plan(roots)
    n = sum(len(v) for v in todo.values())
    for f in sorted(todo):
        raw = open(f, encoding='utf-8').read().split('\n')
        for i, new in sorted(todo[f]):
            print('  %s:%d' % (f, i + 1))
            print('     - %s' % raw[i].strip())
            print('     - %s' % raw[i + 1].strip())
            print('     + %s' % new.strip())
        if apply_:
            drop = set()
            for i, new in todo[f]:
                raw[i] = new; drop.add(i + 1)
            open(f, 'w', encoding='utf-8').write(
                '\n'.join(l for k, l in enumerate(raw) if k not in drop))
    print()
    print('%d Sites %s' % (n, 'GESCHRIEBEN' if apply_ else 'im Trockenlauf (--apply zum Schreiben)'))
    for k, v in held.most_common():
        print('  %5d zurueckgehalten: %s' % (v, k))


if __name__ == '__main__':
    main()
