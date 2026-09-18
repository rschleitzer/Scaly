#!/usr/bin/env python3
# xctrace-bottleneck.py <trace>: the duration-weighted mean of the four CPU
# Bottlenecks values Instruments computes per 10 ms slice for the recorded process.
#
#   xcrun xctrace record --template "CPU Counters" --output t.trace --launch -- <cmd>
#   tools/xctrace-bottleneck.py t.trace
#
# The export gives four numbers per slice in 1/10000 units and does not say which is
# which. The order below was CALIBRATED on an Apple M4 with tools/xctrace-calib.c
# (2026-09-18): a dependent pointer chase over 256 MB reads 98.5 % in a1, a tight
# ALU loop puts its non-a1 share in a0, and unpredictable branches land in a2 and
# a3. So a0 Useful, a1 Processing (back end: load latency AND dependency chains —
# this mode does not separate the two), a2 Delivery (front end, refetch after a
# redirect), a3 Discarded (squashed speculation). Re-run the calibration after an
# Xcode update before trusting the order.
import sys, subprocess, re
trace = sys.argv[1]
xp = '/trace-toc/run[@number="1"]/data/table[@schema="CounterMetricAggregatedForProcess"]'
xml = subprocess.run(['xcrun', 'xctrace', 'export', '--input', trace, '--xpath', xp],
                     capture_output=True, text=True).stdout
ids = {}
tot = [0.0] * 4
wsum = 0.0
for row in re.findall(r'<row>(.*?)</row>', xml, re.S):
    # durations and arrays may be references to earlier ids
    for m in re.finditer(r'<(duration|uint64-array|boolean) id="(\d+)"[^>]*>([^<]*)<', row):
        ids[m.group(2)] = m.group(3)
    def val(tag):
        m = re.search(r'<%s id="(\d+)"[^>]*>([^<]*)<' % tag, row)
        if m:
            return m.group(2)
        m = re.search(r'<%s ref="(\d+)"' % tag, row)
        return ids.get(m.group(1)) if m else None
    precise = val('boolean')
    if precise != '0':
        continue
    d = val('duration'); arr = val('uint64-array')
    if d is None or arr is None:
        continue
    d = float(d); a = [float(x) for x in arr.split()]
    if len(a) != 4:
        continue
    for i in range(4):
        tot[i] += a[i] * d
    wsum += d
if wsum == 0:
    print('no rows'); sys.exit(1)
m = [t / wsum / 100 for t in tot]
print('useful %5.1f%%  processing %5.1f%%  delivery %5.1f%%  discarded %5.1f%%   (sum %5.1f%%)' % (m[0], m[1], m[2], m[3], sum(m)))
