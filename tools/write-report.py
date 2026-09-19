#!/usr/bin/env python3
"""Summarise `scalyc --write-report` over several roots (tools/write-report.sh).

Usage: write-report.py <union-out> <root-report>...

A body appears once per root that plans it and, for a generic, once per
instantiation: the union keys a body by file:line:col and keeps the WORST
verdict (a body one instantiation of which writes is a writing body).
"""
import re
import sys
from collections import Counter, defaultdict

LINE = re.compile(r'^write-report: (?P<loc>[^ ]+:\d+:\d+): (?P<rest>.*)$')
# A parameter list may carry spaces (`this:HashMapBuilder[String, int]`): it ends
# at the verdict, never at the first blank.
BODY = re.compile(r'^(?P<kind>fn|proc|init) (?P<name>\S+) (?P<verdict>clean|writes (?P<params>.*?)(?: (?P<how>direct|transitive))?)$')
SITE = re.compile(r'^(?P<cls>[a-z]-[a-z]+) (?P<name>\S+) writes (?P<params>.+?)(?: (?P<detail>(?:->|\^|region).*|locked))?$')


def package_of(loc):
    m = re.match(r'(?:.*/)?packages/([^/]+)/', loc)
    return m.group(1) if m else '(other)'


def split_params(params):
    """`this:Map[K, V],host:ref[Page]` -> the two entries (commas inside [] stay)."""
    out, depth, cur = [], 0, ''
    for ch in params:
        if ch == '[':
            depth += 1
        elif ch == ']':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur)
            cur = ''
        else:
            cur += ch
    out.append(cur)
    return out


def param_kinds(params):
    """Classify the written parameters: this / page / other param / global."""
    kinds = set()
    for p in split_params(params):
        if not p:
            continue
        if p == 'global':
            kinds.add('global')
        elif p.startswith('this:'):
            kinds.add('this')
        elif re.search(r'Page\]$', p) or p.endswith(':Page'):
            kinds.add('page')
        else:
            kinds.add('param')
    return kinds


def main():
    out = sys.argv[1]
    bodies = {}
    sites = {}
    for path in sys.argv[2:]:
        for raw in open(path, errors='replace'):
            m = LINE.match(raw.rstrip('\n'))
            if not m:
                continue
            loc, rest = m.group('loc'), m.group('rest')
            b = BODY.match(rest)
            if b:
                old = bodies.get(loc)
                if old is None or (old['verdict'] == 'clean' and b.group('verdict') != 'clean') \
                        or (old.get('how') == 'transitive' and b.group('how') == 'direct'):
                    bodies[loc] = b.groupdict()
                continue
            s = SITE.match(rest)
            if s:
                sites[(loc, s.group('cls'))] = s.groupdict()
    with open(out, 'w') as f:
        for loc in sorted(bodies):
            b = bodies[loc]
            f.write(f"{loc}: {b['kind']} {b['name']} {b['verdict']}\n")
        for (loc, cls) in sorted(sites):
            s = sites[(loc, cls)]
            f.write(f"{loc}: {cls} {s['name']} writes {s['params']} {s['detail'] or ''}\n")

    per = defaultdict(Counter)
    for loc, b in bodies.items():
        pkg = package_of(loc)
        k = b['kind']
        per[pkg][k] += 1
        if b['verdict'] == 'clean':
            per[pkg][k + '-clean'] += 1
            continue
        per[pkg][k + '-writes'] += 1
        if k == 'fn':
            per[pkg]['fn-' + (b['how'] or '?')] += 1
            kinds = param_kinds(b['params'] or '')
            if kinds <= {'page'}:
                per[pkg]['fn-only-page'] += 1
            elif kinds <= {'this', 'page'}:
                per[pkg]['fn-only-this'] += 1
            else:
                per[pkg]['fn-other'] += 1
    print(f"union: {len(bodies)} bodies, {len(sites)} sites -> {out}")
    hdr = ('package', 'fn', 'clean', 'writes', 'direct', 'transit', 'only-pg', 'only-this', 'other',
           'proc', 'p-writes', 'init', 'i-writes')
    print('%-10s' % hdr[0] + ''.join('%9s' % h for h in hdr[1:]))
    tot = Counter()
    for pkg in sorted(per, key=lambda p: -per[p]['fn']):
        c = per[pkg]
        row = (c['fn'], c['fn-clean'], c['fn-writes'], c['fn-direct'], c['fn-transitive'],
               c['fn-only-page'], c['fn-only-this'], c['fn-other'],
               c['proc'], c['proc-writes'], c['init'], c['init-writes'])
        tot.update(c)
        print('%-10s' % pkg + ''.join('%9d' % v for v in row))
    row = (tot['fn'], tot['fn-clean'], tot['fn-writes'], tot['fn-direct'], tot['fn-transitive'],
           tot['fn-only-page'], tot['fn-only-this'], tot['fn-other'],
           tot['proc'], tot['proc-writes'], tot['init'], tot['init-writes'])
    print('%-10s' % 'total' + ''.join('%9d' % v for v in row))

    print('-- function sites by class')
    cls = Counter(s['cls'] for s in sites.values())
    for k, v in cls.most_common():
        print(f'{v:7d}  {k}')
    print('-- callees that make a function write (w-call), top 40')
    callees = Counter()
    for (loc, c), s in sites.items():
        if c == 'w-call' and s['detail']:
            callees[s['detail'].replace('-> ', '')] += 1
    for k, v in callees.most_common(40):
        print(f'{v:7d}  {k}')
    print('-- externs written through (w-extern), top 20')
    ext = Counter()
    for (loc, c), s in sites.items():
        if c == 'w-extern' and s['detail']:
            ext[s['detail'].replace('-> ', '')] += 1
    for k, v in ext.most_common(20):
        print(f'{v:7d}  {k}')


if __name__ == '__main__':
    main()
