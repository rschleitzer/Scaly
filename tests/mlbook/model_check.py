#!/usr/bin/env python3
"""tests/mlbook/model_check.py <node> <scaly> -- the numbers of the model page.

Chapter 2 tells what the model page (web/model) does when the reader presses
Learn: after how many steps all points are right, when learning stops by
itself, and why. This runs web/model/model.scaly's own routines on the
page's own start -- the points, the starting values and the learning rates
of web/model/index.html (node evaluates the point sets, so the disc's
coordinates are the very numbers the page computes) -- and prints, for each
case, a line: how many points are right at the start, at which step all of
them are right first, and at which step and why learning ended (1 the goal,
2 no headway). The driver steps the program's `Run` with its `advance`,
exactly as the program's loop does while the page lets it learn.
"""
import json
import os
import re
import struct
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
PAGE = os.path.join(ROOT, 'web', 'model', 'index.html')
PROGRAM = os.path.join(ROOT, 'web', 'model', 'model.scaly')


def point_sets(node, html):
    # the statements from `const SIX` up to `const START`, evaluated by node
    m = re.search(r'(const SIX = .*?)\nconst START', html, re.S)
    if not m:
        sys.exit('model_check: no point sets in index.html')
    script = m.group(1) + '\nconsole.log(JSON.stringify({ SIX, XOR, DISC }));'
    out = subprocess.run([node, '-e', script], capture_output=True, text=True, check=True).stdout
    return json.loads(out)


def model_start(html, name):
    # the three starting values of a model's sliders, in their order
    m = re.search(name + r': \{(.*?)\n  \},', html, re.S)
    if not m:
        sys.exit('model_check: no model ' + name + ' in index.html')
    values = [float(v) for v in re.findall(r'value: (-?[0-9.]+) \}', m.group(1))]
    rate = re.search(r'rate: ([0-9.]+)', m.group(1).split('params')[0])
    return values, (float(rate.group(1)) if rate else None)


def default_rate(html):
    m = re.search(r'<option selected>([0-9.]+)</option>', html)
    return float(m.group(1))


def bits(x):
    return '0x%016x' % struct.unpack('<Q', struct.pack('<d', float(x)))[0]


def points_code(name, pts):
    lines = ['procedure points_' + name + '(mutable points: Vector[Point]) returns int', '{']
    for k, (x, y, kind) in enumerate(pts):
        lines.append('    put_point(points, %d, %s, %s, %s)' % (k, bits(x), bits(y), '1.0' if kind > 0 else '0.0 - 1.0'))
    lines += ['    ' + str(len(pts)), '}', '']
    return '\n'.join(lines)


DRIVER = '''
; --- the check (tests/mlbook/model_check.py): the page's cases ----------

procedure put_point(mutable points: Vector[Point], k: int, x: u64, y: u64, kind: f64)
{
    points.put(k, Point(x.float_from_bits(), y.float_from_bits(), kind))
}

function right(points: Vector[Point], n: int, which: int, w1: f64, w2: f64, b: f64, net: Vector[f64]) returns int
{
    var r 0
    var k 0
    while k < n
    {
        if guess_at(points[k], which, w1, w2, b, net) = points[k].kind
            r := r + 1
        k := k + 1
    }
    r
}

procedure check(name: String, which: int, w1: f64, w2: f64, b: f64, rate: f64, points: Vector[Point], n: int)
{
    var net Vector[f64](NET)
    var grad Vector[f64](NET)
    begin_network(net)
    var run Run(which, w1, w2, b, 0, 0, 0.0, 0)
    begin_anew(run, points, n, net)
    let first right(points, n, which, run.a, run.b, run.c, net)
    var all 0 - 1
    if first = n
        all := 0
    while run.ended = 0 and run.steps < 20000
    {
        advance(run, net, grad, points, n, rate, true)
        if all < 0 and right(points, n, which, run.a, run.b, run.c, net) = n
            all := run.steps
    }
    if all < 0
        print "`name`: `first` of `n` right at the start, never all, ended `run.ended` at step `run.steps`"
    else
        print "`name`: `first` of `n` right at the start, all from step `all`, ended `run.ended` at step `run.steps`"
}
'''


def main():
    node, scaly = sys.argv[1], sys.argv[2]
    html = open(PAGE).read()
    sets = point_sets(node, html)
    (d1, d2, d0), _ = model_start(html, 'descent')
    (s1, s2, s0), _ = model_start(html, 'squares')
    _, hidden_rate = model_start(html, 'hidden')
    rate = default_rate(html)
    program = open(PROGRAM).read()
    cut = program.index('; --- the run ----')
    code = [program[:cut], DRIVER]
    for name in ('SIX', 'XOR', 'DISC'):
        code.append(points_code(name.lower(), sets[name]))
    code.append('var points Vector[Point](MAXP)')

    def num(x):
        # a number as Scaly writes it: a negative one as a difference
        return repr(float(x)) if x >= 0 else '(0.0 - %r)' % float(-x)

    def case(label, which, w, r, pts):
        code.append('var n_%s points_%s(points)' % (label, pts))
        code.append('check("%s", %d, %s, %s, %s, %s, points, n_%s)'
                    % (label, which, num(w[0]), num(w[1]), num(w[2]), num(r), label))

    case('descent', 2, (d1, d2, d0), rate, 'six')
    case('descent_slow', 2, (d1, d2, d0), 0.01, 'six')
    case('saturated', 2, (-3.0, -3.0, d0), rate, 'six')
    case('disc_plain', 2, (d1, d2, d0), rate, 'disc')
    case('squares', 3, (s1, s2, s0), rate, 'disc')
    case('xor', 4, (0.0, 0.0, 0.0), hidden_rate, 'xor')
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, 'model_check.scaly')
        open(path, 'w').write('\n'.join(code) + '\n')
        res = subprocess.run([scaly, 'run', path], capture_output=True, text=True)
        sys.stdout.write(res.stdout)
        if res.returncode != 0:
            sys.stderr.write(res.stderr[-2000:])
            sys.exit(res.returncode)


if __name__ == '__main__':
    main()
