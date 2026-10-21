#!/usr/bin/env python3
"""compare.py <results-dir> -- per profile, the best requests per second of
every framework measured (the board's JSON per framework, as measure.sh
copies them), Scaly's rank among them and its ratio to the best other one."""
import glob, json, os, sys

d = sys.argv[1]
best = {}
for f in glob.glob(os.path.join(d, '*.json')):
    r = json.load(open(f))
    name = r.get('framework')
    for key, v in r.get('results', {}).items():
        if not isinstance(v, dict) or not v.get('rps'):
            continue
        prof = key.rsplit('-', 1)[0]
        best.setdefault(prof, {})
        best[prof][name] = max(best[prof].get(name, 0), v['rps'])
print('%-18s %10s %5s %7s  %s' % ('profile', 'scaly', 'rank', 'vs best', 'best other'))
for prof in sorted(best):
    rows = sorted(best[prof].items(), key=lambda x: -x[1])
    if 'scaly' not in best[prof]:
        continue
    s = best[prof]['scaly']
    rank = [n for n, _ in rows].index('scaly') + 1
    other = [(n, r) for n, r in rows if n != 'scaly']
    if other:
        print('%-18s %10d %2d/%-2d %6.2fx  %s %d' % (prof, s, rank, len(rows), s / other[0][1], other[0][0], other[0][1]))
    else:
        print('%-18s %10d %2d/%-2d' % (prof, s, rank, len(rows)))
