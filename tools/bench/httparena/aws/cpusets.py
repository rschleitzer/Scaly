#!/usr/bin/env python3
"""cpusets.py <httparena-checkout> -- runs ON the host (setup.sh calls it).

The compose stacks (gateway-64, gateway-h3, production-stack) pin their
services in the board's numbering, physical core c as CPUs c and c+64. Each
entry's own split is mapped onto this machine's server cores -- the same
cores setup.sh gives the single-container profiles: board core c goes to
server core c * n / 32, with both its siblings -- and written back as the
default of its variable (`${PROXY_CPUSET:-...}`), so an override still wins;
a literal cpuset stays a literal. On a machine with fewer than 32 server
cores two small services may share one."""
import glob
import re
import sys

cores = {}
for d in glob.glob('/sys/devices/system/cpu/cpu[0-9]*'):
    n = int(re.search(r'(\d+)$', d).group(1))
    try:
        key = (int(open(d + '/topology/physical_package_id').read()),
               int(open(d + '/topology/core_id').read()))
    except OSError:
        continue
    cores.setdefault(key, []).append(n)
pkgs = sorted({k[0] for k in cores})
first = sorted(k for k in cores if k[0] == pkgs[0])
# setup.sh's split: 32 cores of socket 0, or half of the only socket
server = first[:32] if len(pkgs) > 1 else first[:min(32, len(first) // 2)]


def expand(text):
    out = set()
    for part in text.split(','):
        a, _, b = part.partition('-')
        out.update(range(int(a), int(b or a) + 1))
    return out


def mapped(text):
    cpus = set()
    for c in {cpu % 64 for cpu in expand(text)}:
        cpus.update(cores[server[c * len(server) // 32]])
    ns = sorted(cpus)
    runs, start = [], ns[0]
    for a, b in zip(ns, ns[1:] + [None]):
        if b != a + 1:
            runs.append('%d-%d' % (start, a) if a != start else '%d' % a)
            start = b
    return ','.join(runs)


PAT = re.compile(r'(cpuset: ")(\$\{\w+:-)?([0-9,\-]+)(\}?")')
changed = 0
for f in sorted(glob.glob(sys.argv[1] + '/frameworks/*/compose*.yml')):
    text = open(f).read()
    new = PAT.sub(lambda m: m.group(1) + (m.group(2) or '') + mapped(m.group(3)) + m.group(4), text)
    if new != text:
        open(f, 'w').write(new)
        changed += 1
print('cpusets.py: %d compose files mapped onto %d server cores' % (changed, len(server)))
