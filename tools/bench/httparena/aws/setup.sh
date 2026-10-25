#!/bin/bash
# setup.sh <httparena-commit> -- runs ON the host (prepare.sh copies it there
# with the entry): Docker, an HttpArena checkout at a pinned commit with the
# Scaly entry in frameworks/scaly, and the board's CPU split rebuilt for
# this machine's topology.
#
# The board pins the server to 0-31,64-95 -- 32 cores and their SMT siblings
# on a Threadripper whose siblings are n+64 -- and the load generators to the
# other half. Here the same shape is derived from /sys: the server on 32
# cores of socket 0 with their siblings, the load generators on 32 cores of
# socket 1 with theirs, latency-500k-8cpu on 4 cores of socket 0 (unlike the
# board's single socket, server and load generator talk across sockets). On a
# one-socket machine its cores are halved instead, at most 32 for each side.
set -eu
COMMIT="$1"
HA="$HOME/HttpArena"

if ! command -v docker > /dev/null; then
  curl -fsSL https://get.docker.com | sudo sh
  sudo usermod -aG docker "$USER"
fi
# benchmark.sh restarts the daemon before every profile, and one a framework
# does not subscribe to takes seconds: systemd's start limit then refuses the
# daemon and every later run fails at once (2026-10-25, eleven entries lost)
sudo mkdir -p /etc/systemd/system/docker.service.d
printf '[Unit]\nStartLimitIntervalSec=0\n' | \
  sudo tee /etc/systemd/system/docker.service.d/no-start-limit.conf > /dev/null
sudo systemctl daemon-reload
sudo apt-get update -q
sudo apt-get install -y -q git jq python3 curl linux-tools-common "linux-tools-$(uname -r)" || \
  sudo apt-get install -y -q git jq python3 curl

[ -d "$HA" ] || git clone -q https://github.com/MDA2AV/HttpArena.git "$HA"
git -C "$HA" fetch -q origin
git -C "$HA" checkout -q -- scripts/lib/profiles.sh 2> /dev/null || true
git -C "$HA" checkout -q "$COMMIT"
# undo an earlier run's cpuset mapping (cpusets.py) before mapping again
git -C "$HA" checkout -q -- frameworks
rm -rf "$HA/frameworks/scaly"
cp -R "$HOME/entry" "$HA/frameworks/scaly"
# the gateway entries beside it (prepare.sh copies them along)
for g in scaly_nginx scaly_caddy; do
  if [ -d "$HOME/$g" ]; then
    rm -rf "$HA/frameworks/$g"
    cp -R "$HOME/$g" "$HA/frameworks/$g"
  fi
done

# the split
read -r SERVER LOADGEN SMALL LOADGEN_N < <(python3 - << 'EOF'
import glob, re
cpus = {}
for d in glob.glob('/sys/devices/system/cpu/cpu[0-9]*'):
    n = int(re.search(r'(\d+)$', d).group(1))
    try:
        pkg = int(open(d + '/topology/physical_package_id').read())
        core = int(open(d + '/topology/core_id').read())
    except OSError:
        continue
    cpus.setdefault((pkg, core), []).append(n)
def cpulist(cores):
    # as ranges, `0-15,32-47`: HttpArena takes a limit without a `-` for a
    # NUMBER of CPUs (docker --cpus), not for a set
    ns = sorted(c for k in cores for c in cpus[k])
    runs, start = [], ns[0]
    for a, b in zip(ns, ns[1:] + [None]):
        if b != a + 1:
            runs.append('%d-%d' % (start, a))
            start = b
    return ','.join(runs)
pkgs = sorted({k[0] for k in cpus})
first = sorted(k for k in cpus if k[0] == pkgs[0])
if len(pkgs) > 1:
    # two sockets: the server on one, the load generators on the other
    other = sorted(k for k in cpus if k[0] == pkgs[-1])
    server, loadgen = first[:32], other[:32]
else:
    # one socket: its cores halved, as the board halves its Threadripper
    half = min(32, len(first) // 2)
    server, loadgen = first[:half], first[half:2 * half]
print(cpulist(server), cpulist(loadgen), cpulist(first[:4]), sum(len(cpus[k]) for k in loadgen))
EOF
)
sed -i "s/|0-31,64-95|/|$SERVER|/; s/|0-3,64-67|/|$SMALL|/" "$HA/scripts/lib/profiles.sh"
grep -q "|0-31,64-95|" "$HA/scripts/lib/profiles.sh" && { echo "setup.sh: profiles.sh still names the board's cpuset"; exit 1; }
# the compose stacks' own splits, onto the same server cores
python3 "$HOME/cpusets.py" "$HA"
# the board runs its load generators with 64 threads on its 64 CPUs: as many
# threads as the load generators have CPUs here
cat > "$HOME/bench.env" << EOF
export GCANNON_CPUS=$LOADGEN
export SERVER_CPUS=$SERVER
export LOADGEN_DOCKER=true
export THREADS=$LOADGEN_N
export H2THREADS=$LOADGEN_N
export H3THREADS=$LOADGEN_N
EOF
echo "setup.sh: HttpArena at $(git -C "$HA" rev-parse --short HEAD); server $SERVER; load generators $LOADGEN; 8cpu $SMALL"
lscpu | grep -E 'Model name|^CPU\(s\)|Socket|NUMA node\(s\)'
