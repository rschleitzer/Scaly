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

def pi_decimal(digits):
    from decimal import localcontext
    with localcontext() as ctx:
        ctx.prec = digits + 10
        def atan_inv(n):
            x = Decimal(1) / n
            x2, s, t, k = x * x, Decimal(0), x, 1
            while abs(t) > Decimal(10) ** -(digits + 8):
                s += t / k
                t = -t * x2
                k += 2
            return s
        return 16 * atan_inv(5) - 4 * atan_inv(239)

PI400 = None
def trig_exact(x, cosine):
    """sin x or cos x: x reduced modulo pi/2 with pi to 400 digits (a
    double is below 2^1024, 309 digits), then the series"""
    global PI400
    from decimal import localcontext
    if PI400 is None:
        PI400 = pi_decimal(400)
    with localcontext() as ctx:
        ctx.prec = 400
        d = Decimal(x)
        k = int((d / (PI400 / 2)).to_integral_value())
        r = d - k * (PI400 / 2)
    r = +r
    q = (k + (1 if cosine else 0)) % 4
    want_cos = q % 2 == 1
    x2, t, s, n = r * r, (Decimal(1) if want_cos else r), Decimal(0), (0 if want_cos else 1)
    while t != 0 and abs(t) > Decimal(10) ** -70 * (abs(s) + Decimal(10) ** -400):
        s += t
        t = -t * x2 / ((n + 1) * (n + 2))
        n += 2
    return -s if q >= 2 else s

def near_multiples(count):
    """doubles next to multiples of pi/2, the hard cases of the reduction"""
    xs = []
    p = pi_decimal(60)
    for _ in range(count):
        k = random.randint(1, 1 << random.randint(1, 60))
        x = float(Decimal(k) * p / 2)
        xs.append(x * random.choice([1, -1]))
    return xs

def atan_exact(d):
    """atan of a Decimal: beyond 1 by pi/2 - atan(1/d), then halved
    (atan x = 2 atan(x / (1 + sqrt(1 + x^2)))) below 0.15, then the series"""
    from decimal import localcontext
    global PI400
    if PI400 is None:
        PI400 = pi_decimal(400)
    if d < 0:
        return -atan_exact(-d)
    if d > 1:
        return PI400 / 2 - atan_exact(1 / d)
    with localcontext() as ctx:
        ctx.prec = 70
        k = 0
        while d > Decimal('0.15'):
            d = d / (1 + (1 + d * d).sqrt())
            k += 1
        s, t, n, d2 = Decimal(0), d, 1, d * d
        while t != 0 and abs(t) > Decimal(10) ** -72 * abs(s + d):
            s += t / n
            t = -t * d2
            n += 2
        r = s * 2 ** k
    return +r

def asin_exact(x):
    d = Decimal(x)
    if abs(d) == 1:
        return d * PI400 / 2 if PI400 is not None else d * pi_decimal(70) / 2
    from decimal import localcontext
    with localcontext() as ctx:
        ctx.prec = 80
        r = d / (1 - d * d).sqrt()
    return atan_exact(r)

def atan2_exact(y, x):
    global PI400
    if PI400 is None:
        PI400 = pi_decimal(400)
    dy, dx = Decimal(y), Decimal(x)
    from decimal import localcontext
    with localcontext() as ctx:
        ctx.prec = 80
        if dx == 0:
            return (PI400 / 2) * (1 if dy > 0 else -1)
        a = atan_exact(dy / dx)
        if dx > 0:
            return +a
        return +(a + PI400) if dy >= 0 else +(a - PI400)

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
elif fn == 'atan':
    special = [(0.0, 0.0), (inf, math.pi / 2), (-inf, -math.pi / 2), (nan, nan), (1.0, math.pi / 4), (-1.0, -math.pi / 4)]
    xs = [random.uniform(-5.0, 5.0) for _ in range(count)]
    xs += [2.0 ** random.uniform(-40, 1023) * random.choice([1, -1]) for _ in range(count // 4)]
    xs += [5e-324, 1e-300, 1.7976931348623157e308]
    exact = lambda x: atan_exact(Decimal(x))
elif fn in ('asin', 'acos'):
    if fn == 'asin':
        special = [(0.0, 0.0), (1.0, math.pi / 2), (-1.0, -math.pi / 2), (2.0, nan), (-inf, nan), (nan, nan)]
        exact = asin_exact
    else:
        special = [(1.0, 0.0), (-1.0, math.pi), (0.0, math.pi / 2), (2.0, nan), (inf, nan), (nan, nan)]
        exact = lambda x: pi_decimal(70) / 2 - asin_exact(x)
    xs = [random.uniform(-1.0, 1.0) for _ in range(count)]
    xs += [(1 - 2.0 ** -random.uniform(1, 53)) * random.choice([1, -1]) for _ in range(count // 4)]
    xs += [2.0 ** random.uniform(-60, -1) * random.choice([1, -1]) for _ in range(count // 4)]
    xs += [0.7071067811865476, 0.7071067811865475, 5e-324]
elif fn == 'atan2':
    pio2 = math.pi / 2
    special = [((0.0, 1.0), 0.0), ((0.0, -1.0), math.pi), ((-0.0, -1.0), -math.pi), ((1.0, 0.0), pio2), ((-1.0, 0.0), -pio2),
               ((1.0, inf), 0.0), ((1.0, -inf), math.pi), ((inf, 1.0), pio2), ((inf, inf), math.pi / 4), ((inf, -inf), 3 * math.pi / 4),
               ((nan, 1.0), nan), ((1.0, nan), nan), ((0.0, 0.0), 0.0), ((0.0, -0.0), math.pi)]
    xs = [(random.uniform(-10.0, 10.0), random.uniform(-10.0, 10.0)) for _ in range(count)]
    xs += [(2.0 ** random.uniform(-200, 200) * random.choice([1, -1]), 2.0 ** random.uniform(-200, 200) * random.choice([1, -1])) for _ in range(count // 4)]
    xs += [(1.0, 1.0), (-1.0, -1.0), (1e-300, 1e300), (1e300, 1e-300)]
    exact = lambda p: atan2_exact(*p)
elif fn == 'tan':
    special = [(0.0, 0.0), (inf, nan), (-inf, nan), (nan, nan)]
    xs = [random.uniform(-10.0, 10.0) for _ in range(count)]
    xs += [2.0 ** random.uniform(-40, 1023) * random.choice([1, -1]) for _ in range(count // 4)]
    xs += near_multiples(count // 8)
    xs += [6381956970095103.0 * 2.0 ** 797, 5e-324, -5e-324, 1e-300, 1.5707963267948966, -1.5707963267948966]
    exact = lambda x: trig_exact(x, False) / trig_exact(x, True)
elif fn in ('sin', 'cos'):
    if fn == 'sin':
        special = [(0.0, 0.0), (inf, nan), (-inf, nan), (nan, nan)]
    else:
        special = [(0.0, 1.0), (-0.0, 1.0), (inf, nan), (-inf, nan), (nan, nan)]
    xs = [random.uniform(-10.0, 10.0) for _ in range(count)]
    xs += [2.0 ** random.uniform(-40, 1023) * random.choice([1, -1]) for _ in range(count // 4)]
    xs += near_multiples(count // 8)
    xs += [6381956970095103.0 * 2.0 ** 797, 5e-324, -5e-324, 1e-300, 2.0 ** 20, 2.0 ** 20 - 1e-10, 0.7853981633974483, 0.7853981633974484]
    exact = lambda x: trig_exact(x, fn == 'cos')
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
line = lambda x: ('%d %d' % (f2b(x[0]), f2b(x[1]))) if isinstance(x, tuple) else str(f2b(x))
open(path, 'w').write('\n'.join(line(x) for x in allx) + '\n')
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
