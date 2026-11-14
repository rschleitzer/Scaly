#!/usr/bin/env python3
# tools/math/tables.py -- the constants of scaly/math.scaly, computed here
# with Python's decimal module to 50 digits and rounded to the nearest
# double (float(Decimal) rounds correctly). Prints the definitions to paste;
# run it again after a change to how a constant is defined.
from decimal import Decimal, getcontext
import struct

getcontext().prec = 50

def bits(d):
    return struct.unpack('<Q', struct.pack('<d', float(d)))[0]

def hexbits(d):
    return '0x%016x' % bits(d)

def split(d, keep):
    """d as hi + lo: hi the nearest double with `keep` significant bits
    (its last 53 - keep are zero, so k * hi is exact for |k| < 2^(53 - keep)),
    lo the nearest double to the rest."""
    hi = float(d)
    m, e = __import__('math').frexp(hi)
    scale = 2 ** keep
    hi = round(m * scale) / scale * 2.0 ** e
    lo = float(d - Decimal(hi))
    return hi, lo

LN2 = Decimal(2).ln()
N = 128

def literal(v):
    """a double as a Scaly literal: its shortest decimal that reads back to
    it, where that needs no exponent (the lexer has none); else None"""
    r = repr(float(v))
    return None if 'e' in r or 'inf' in r or 'nan' in r else r

def constant(name, v):
    lit = literal(v)
    if lit is not None:
        print('define %s f64 %s' % (name, lit))
    else:
        print('define %s_BITS u64 %s' % (name, hexbits(v)))

# exp: x = k ln2/N + r, |r| <= ln2/2N; e^x = 2^(k/N) e^r, 2^(i/N) from the
# table as hi + lo (i = k mod N), e^r - 1 as r + r^2/2 + ... + r^5/120
ln2n = LN2 / N
hi, lo = split(ln2n, 33)
print('; exp: N = %d' % N)
constant('EXP_INV_LN2_N', Decimal(N) / LN2)
constant('EXP_LN2_N_HI', hi)
constant('EXP_LN2_N_LO', lo)
for k, name in [(3, 'EXP_C3'), (4, 'EXP_C4'), (5, 'EXP_C5')]:
    f = 1
    for j in range(2, k + 1):
        f *= j
    constant(name, Decimal(1) / f)
table = []
for i in range(N):
    t = (Decimal(2) ** (Decimal(i) / N))
    th = float(t)
    tl = float(t - Decimal(th))
    table += [hexbits(th), hexbits(tl)]
print('define EXP_TABLE: u64[] [%s]' % ', '.join(table))

# log: x = 2^e m, m in [OFF, 2 OFF) with OFF near 1/sqrt 2: the bits of
# x less OFF's give e (above bit 52) and the interval i of m (the next 8
# bits, 256 intervals). c is the middle of i; invc near 1/c with so few
# bits that m invc - 1 is exact in a double (a multiple of 2^-61 below
# 2^-8: invc a multiple of 2^-8 where m < 1, of 2^-9 where m >= 1); logc =
# -log(invc) exactly, as hi + lo. log x = e ln2 + logc + log1p(m invc - 1).
LN = 256
OFF = struct.unpack('<Q', struct.pack('<d', float(1 / Decimal(2).sqrt())))[0]
OFF -= OFF & ((1 << 44) - 1)        # the intervals begin on a whole multiple
print('; log: OFF = 0x%016x, N = %d' % (OFF, LN))
lhi, llo = split(LN2, 42)
constant('LOG_LN2_HI', lhi)
constant('LOG_LN2_LO', llo)
ltable = []
worst = 0
for i in range(LN):
    a = struct.unpack('<d', struct.pack('<Q', OFF + (i << 44)))[0]
    b = struct.unpack('<d', struct.pack('<Q', OFF + ((i + 1) << 44)))[0]
    c = (Decimal(a) + Decimal(b)) / 2
    step = 256 if a < 1.0 else 512
    invc = round(float(1 / c) * step) / step
    for z in (a, b):
        worst = max(worst, abs(float(Decimal(z) * Decimal(invc) - 1)))
    logc = -Decimal(invc).ln()
    # hi on a multiple of 2^-42, as ln2's high part: e ln2hi + hi is exact
    ch = float(round(logc * 2 ** 42) / Decimal(2 ** 42))
    cl = float(logc - Decimal(ch))
    ltable += [hexbits(invc), hexbits(ch), hexbits(cl)]
assert worst < 2 ** -8, worst
print('; log: |m invc - 1| < %r' % worst)
print('define LOG_TABLE: u64[] [%s]' % ', '.join(ltable))
print('; log1p coefficients, (-1)^(n+1)/n')
for n in range(2, 14):
    v = Decimal((-1) ** (n + 1)) / n
    print(';   %d: %s' % (n, literal(v)))

# exp2: 2^r - 1 = sum (r ln2)^n / n!, |r| <= 1/256; the coefficients
print('; exp2: 2^r - 1, coefficient of r^n = ln2^n / n!')
f = 1
for n in range(1, 7):
    f *= n
    print(';   %d: %s' % (n, literal(LN2 ** n / f)))

# log2, log10: 1/ln2 and 1/ln10 as hi + lo
for name, v in [('INV_LN2', 1 / LN2), ('INV_LN10', 1 / Decimal(10).ln())]:
    h = float(v)
    l = float(v - Decimal(h))
    print('; %s: hi %s, lo bits %s' % (name, literal(h), hexbits(l)))

# --- sin, cos ---------------------------------------------------------------
def pi_decimal(digits):
    """pi to `digits` digits: Machin, 16 atan(1/5) - 4 atan(1/239)"""
    from decimal import localcontext
    with localcontext() as ctx:
        ctx.prec = digits + 10
        def atan_inv(n):
            x = Decimal(1) / n
            x2 = x * x
            s, t, k = Decimal(0), x, 1
            while t != 0:
                s += t / k
                t = -t * x2
                k += 2
                if abs(t) < Decimal(10) ** -(digits + 8):
                    break
            return s
        return 16 * atan_inv(5) - 4 * atan_inv(239)

def sin_cos_decimal(x):
    """sin x and cos x for a small Decimal x, by their series"""
    x2 = x * x
    s, t, k = Decimal(0), x, 1
    while abs(t) > Decimal(10) ** -(getcontext().prec + 2):
        s += t
        t = -t * x2 / ((k + 1) * (k + 2))
        k += 2
    c, t, k = Decimal(0), Decimal(1), 0
    while abs(t) > Decimal(10) ** -(getcontext().prec + 2):
        c += t
        t = -t * x2 / ((k + 1) * (k + 2))
        k += 2
    return s, c

PI = pi_decimal(60)
PIO2 = PI / 2
print('; sin, cos: x = N pi/64 + r, pi/64 in four parts (33, 33, 33 bits, the rest)')
constant('TRIG_64_OVER_PI', 64 / PI)
rest = PIO2
parts = []
for k in range(3):
    h, _ = split(rest, 33)
    parts.append(h)
    rest = rest - Decimal(h)
parts.append(float(rest))
ph = float(PIO2)
# pi/64 is pi/2 times 2^-5: the same bits, the parts as exact
for k, p in enumerate(parts + [ph, float(PIO2 - Decimal(ph))]):
    print(';   %s: %s bits %s' % (['P1', 'P2', 'P3', 'P4', 'hi', 'lo'][k], literal(p / 32), hexbits(p / 32)))
# the series of the kernel, (sin d - d)/d^3 and (cos d - 1)/d^2 in z = d^2,
# near the best of degree 2 on |d| <= pi/128: interpolated at Chebyshev nodes
def solve(A, b):
    n = len(b)
    M = [row[:] + [b[i]] for i, row in enumerate(A)]
    for c in range(n):
        p = max(range(c, n), key=lambda r: abs(M[r][c]))
        M[c], M[p] = M[p], M[c]
        for r in range(n):
            if r != c:
                f = M[r][c] / M[c][c]
                M[r] = [x - f * y for x, y in zip(M[r], M[c])]
    return [M[i][n] / M[i][i] for i in range(n)]
def kernel_series(z, cosine):
    s, t, k = Decimal(0), Decimal(-1) / (2 if cosine else 6), (2 if cosine else 3)
    while abs(t) > Decimal(10) ** -48:
        s += t
        t = -t * z / ((k + 1) * (k + 2))
        k += 2
    return s
import math as _m
Z = (PI / 128 * Decimal('1.0001')) ** 2
nodes = [(Decimal(_m.cos(_m.pi * (2 * i + 1) / 6)) + 1) / 2 * Z for i in range(3)]
for cosine in (False, True):
    c = solve([[z ** k for k in range(3)] for z in nodes], [kernel_series(z, cosine) for z in nodes])
    print('; %s series: %s' % ('cos' if cosine else 'sin', ', '.join('%s (%s)' % (literal(v), hexbits(v)) for v in c)))
# the table: sin and cos of j pi/64, j < 128 (the whole circle), each as
# hi + lo; from the first quadrant (s_0 = 0, c_0 = 1 exactly) by symmetry,
# so that the zeros are zeros
quarter = [sin_cos_decimal(Decimal(i) * PI / 64) for i in range(32)]
tt = []
for j in range(128):
    q, i = divmod(j, 32)
    si, ci = quarter[i]
    sv, cv = [(si, ci), (ci, -si), (-si, -ci), (-ci, si)][q]
    for v in (sv, cv):
        h = float(v) + 0.0
        tt += [hexbits(h), hexbits(v - Decimal(h))]
print('define TRIG_TABLE: u64[] [%s]' % ', '.join(tt))
# 2/pi in bits for the reduction of large arguments: 64 zero bits (the
# integer part and room for a window that begins before the point), then
# 1216 bits of the fraction, 32 bits a word
from decimal import localcontext
with localcontext() as ctx:
    ctx.prec = 420
    big = pi_decimal(410)
    frac = 2 / big
    words = [0, 0]
    for k in range(38):
        frac *= 2 ** 32
        w = int(frac)
        words.append(w)
        frac -= w
print('define TRIG_TWO_OVER_PI: u64[] [%s]' % ', '.join('0x%08x' % w for w in words))

# --- atan, atan2, asin, acos --------------------------------------------------
def atan_decimal(x):
    """atan x for 0 <= x <= 1: halved twice (atan x = 2 atan(x / (1 +
    sqrt(1 + x^2)))), then the series"""
    k = 0
    while x > Decimal('0.15'):
        x = x / (1 + (1 + x * x).sqrt())
        k += 1
    s, t, n, x2 = Decimal(0), x, 1, x * x
    while abs(t) > Decimal(10) ** -(getcontext().prec + 2):
        s += t / n
        t = -t * x2
        n += 2
    return s * 2 ** k

print('; atan: t = c + ..., c = j/64, atan t = atan c + atan u, u = (t - c)/(1 + t c)')
at = []
for j in range(65):
    v = atan_decimal(Decimal(j) / 64)
    h = float(v) + 0.0
    at += [hexbits(h), hexbits(v - Decimal(h))]
print('define ATAN_TABLE: u64[] [%s]' % ', '.join(at))
for name, v in [('pi/2', PIO2), ('pi', PI)]:
    h = float(v)
    print('; %s: hi %s, lo bits %s' % (name, literal(h), hexbits(v - Decimal(h))))
def atan_series(z):
    # (atan u - u)/u^3 as a series in z = u^2
    s, t, n = Decimal(0), Decimal(-1) / 3, 3
    k = 0
    while abs(t) > Decimal(10) ** -48:
        s += t
        k += 1
        t = Decimal((-1) ** (k + 1)) * z ** k / (3 + 2 * k)
    return s
Z = (Decimal(1) / 128 * Decimal('1.001')) ** 2
nodes = [(Decimal(_m.cos(_m.pi * (2 * i + 1) / 6)) + 1) / 2 * Z for i in range(3)]
c = solve([[z ** k for k in range(3)] for z in nodes], [atan_series(z) for z in nodes])
worst = max(abs(sum(Decimal(float(ck)) * (Z * i / 400) ** k for k, ck in enumerate(c)) - atan_series(Z * i / 400)) * (Z * i / 400) for i in range(1, 401))
print('; atan series: %s; relative error 2^%.1f' % (', '.join('%s (%s)' % (literal(v), hexbits(v)) for v in c), float(worst.ln() / Decimal(2).ln())))
