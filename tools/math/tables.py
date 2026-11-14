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
