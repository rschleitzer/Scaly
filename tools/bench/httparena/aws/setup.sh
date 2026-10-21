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
# socket 1 with theirs, latency-500k-8cpu on 4 cores of socket 0. Unlike the
# board's single socket, server and load generator talk across sockets.
set -eu
COMMIT="$1"
HA="$HOME/HttpArena"

if ! command -v docker > /dev/null; then
  curl -fsSL https://get.docker.com | sudo sh
  sudo usermod -aG docker "$USER"
fi
sudo apt-get update -q
sudo apt-get install -y -q git jq python3 curl linux-tools-common "linux-tools-$(uname -r)" || \
  sudo apt-get install -y -q git jq python3 curl

[ -d "$HA" ] || git clone -q https://github.com/MDA2AV/HttpArena.git "$HA"
git -C "$HA" fetch -q origin
git -C "$HA" checkout -q "$COMMIT"
rm -rf "$HA/frameworks/scaly"
cp -R "$HOME/entry" "$HA/frameworks/scaly"

# the split
read -r SERVER LOADGEN SMALL < <(python3 - << 'EOF'
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
def pick(pkg, count):
    cores = sorted(k for k in cpus if k[0] == pkg)[:count]
    return ','.join(str(c) for k in cores for c in sorted(cpus[k]))
pkgs = sorted({k[0] for k in cpus})
print(pick(pkgs[0], 32), pick(pkgs[-1], 32), pick(pkgs[0], 4))
EOF
)
sed -i "s/|0-31,64-95|/|$SERVER|/; s/|0-3,64-67|/|$SMALL|/" "$HA/scripts/lib/profiles.sh"
grep -q "|0-31,64-95|" "$HA/scripts/lib/profiles.sh" && { echo "setup.sh: profiles.sh still names the board's cpuset"; exit 1; }
cat > "$HOME/bench.env" << EOF
export GCANNON_CPUS=$LOADGEN
export LOADGEN_DOCKER=true
EOF
echo "setup.sh: HttpArena at $(git -C "$HA" rev-parse --short HEAD); server $SERVER; load generators $LOADGEN; 8cpu $SMALL"
lscpu | grep -E 'Model name|^CPU\(s\)|Socket|NUMA node\(s\)'
