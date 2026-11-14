#!/usr/bin/env python3
# tests/math/check.py <function> <count> <seed> <bits-binary> [limit-ulp]
# The function of scaly/math.scaly on <count> random numbers of its domain
# (and the special values), checked against the exact value (Python's
# decimal, 60 digits, rounded to the nearest double): the largest error in
# units in the last place must stay below the limit; how many results are
# not the nearest double is said. Writes the inputs to $MATH_INPUTS (for a
# comparison of builds), prints one line, exits 1 on a failure.
import math, os, random, struct, subprocess, sys
from decimal import Decimal, getcontext
getcontext().prec = 60
fn, count, seed, binary = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), sys.argv[4]
limit = float(sys.argv[5]) if len(sys.argv) > 5 else 0.51
b2f = lambda b: struct.unpack('<d', struct.pack('<Q', b))[0]
f2b = lambda f: struct.unpack('<Q', struct.pack('<d', f))[0]
random.seed(seed)
inf, nan = float('inf'), float('nan')
def series_exp_m1(x):
    d = Decimal(x)
    if abs(x) > 1e-5:
        return d.exp() - 1
    return d + d * d / 2 + d ** 3 / 6 + d ** 4 / 24 + d ** 5 / 120

def series_log_1p(x):
    d = Decimal(x)
    if abs(x) > 1e-5:
        return (1 + d).ln()
    return d - d * d / 2 + d ** 3 / 3 - d ** 4 / 4 + d ** 5 / 5

log_xs = lambda: [2.0 ** random.uniform(-1074, 1024) for _ in range(count)] + [random.uniform(0.95, 1.05) for _ in range(count // 4)]
if fn == 'exp':
    special = [(0.0, 1.0), (-0.0, 1.0), (inf, inf), (-inf, 0.0), (nan, nan), (710.0, inf), (-746.0, 0.0)]
    xs = [random.uniform(-745.0, 709.7) for _ in range(count)] + [random.uniform(-1.0, 1.0) for _ in range(count // 4)]
    exact = lambda x: Decimal(x).exp()
elif fn == 'exp2':
    special = [(0.0, 1.0), (1.0, 2.0), (10.0, 1024.0), (-1074.0, 5e-324), (inf, inf), (-inf, 0.0), (nan, nan), (1024.0, inf), (-1076.0, 0.0)]
    xs = [random.uniform(-1075.0, 1023.9) for _ in range(count)] + [random.uniform(-1.0, 1.0) for _ in range(count // 4)]
    exact = lambda x: Decimal(2) ** Decimal(x)
elif fn == 'expm1':
    special = [(0.0, 0.0), (inf, inf), (-inf, -1.0), (nan, nan), (-40.0, -1.0), (710.0, inf)]
    xs = [random.uniform(-40.0, 709.7) for _ in range(count)] + [random.uniform(-1.0, 1.0) for _ in range(count // 4)] + [2.0 ** random.uniform(-60, -1) * random.choice([1, -1]) for _ in range(count // 4)]
    exact = series_exp_m1
elif fn == 'log1p':
    special = [(0.0, 0.0), (-1.0, -inf), (-2.0, nan), (inf, inf), (-inf, nan), (nan, nan)]
    xs = [random.uniform(-0.999, 4.0) for _ in range(count)] + [2.0 ** random.uniform(-60, 1000) for _ in range(count // 4)] + [2.0 ** random.uniform(-60, -1) * random.choice([1, -1]) for _ in range(count // 4)]
    exact = series_log_1p
else:
    special = [(1.0, 0.0), (0.0, -inf), (-0.0, -inf), (-1.0, nan), (inf, inf), (-inf, nan), (nan, nan)]
    if fn == 'log2':
        special += [(2.0, 1.0), (1024.0, 10.0), (0.5, -1.0), (5e-324, -1074.0)]
        exact = lambda x: Decimal(x).ln() / Decimal(2).ln()
    elif fn == 'log10':
        special += [(10.0, 1.0), (1000.0, 3.0), (1e-5, -5.0)]
        exact = lambda x: Decimal(x).log10()
    else:
        exact = lambda x: Decimal(x).ln()
    xs = [x for x in log_xs() if 0 < x < inf]
allx = [s[0] for s in special] + xs
path = os.environ.get('MATH_INPUTS', '/tmp/math_inputs.txt')
open(path, 'w').write('\n'.join(str(f2b(x)) for x in allx) + '\n')
out = subprocess.run([binary, fn, path], capture_output=True, text=True).stdout.split()
fails = []
if len(out) != len(allx):
    fails.append('%d results for %d numbers' % (len(out), len(allx)))
ys = [b2f(int(o)) for o in out[:len(allx)]]
for (x, want), y in zip(special, ys):
    same = (math.isnan(want) and math.isnan(y)) or y == want
    if not same:
        fails.append('%s(%r) = %r, not %r' % (fn, x, y, want))
worst, worst_x, wrong = 0.0, None, 0
for x, y in zip(xs, ys[len(special):]):
    e = exact(x)
    r = float(e)
    if r == 0.0 or math.isinf(r):
        if y != r:
            fails.append('%s(%r) = %r, not %r' % (fn, x, y, r))
        continue
    if f2b(y) != f2b(r):
        wrong += 1
    err = float(abs((Decimal(y) - e) / Decimal(math.ulp(r))))
    if err > worst:
        worst, worst_x = err, x
if worst > limit:
    fails.append('%.4f ulp at %r (limit %.2f)' % (worst, worst_x, limit))
print('%s: %d numbers, %d not the nearest double, at most %.4f ulp%s' %
      (fn, len(xs), wrong, worst, ('; FAIL ' + '; '.join(fails[:5])) if fails else ''))
sys.exit(1 if fails else 0)
