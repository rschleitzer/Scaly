#!/bin/bash
# tools/bench/net/race.sh [threads] [seconds] — every server of build.sh under
# the same load: 128 keep-alive connections, 16 requests pipelined per round
# trip (TechEmpower's plaintext shape: it cuts the CLIENT's cost per request
# so that the servers become the limit). Each server runs on 1 thread, then
# on `threads` (default: half the CPUs — the load generator takes the rest,
# on the same machine), through SCALY_WORKERS / GOMAXPROCS /
# TOKIO_WORKER_THREADS; the client gets the remaining CPUs. Printed per run:
# requests per second, the mean round trip, and the CPU seconds the SERVER
# used — requests per CPU second is the efficiency when the client limits.
# NODELAY=1 sets TCP_NODELAY in the Scaly, go-raw and tokio servers (go-http
# always has it: Go's default).
cd "$(dirname "$0")/../../.." || exit 1
. tests/platform.sh || exit 1
X=$SCALY_EXE
W=${BENCH_WORK:-/tmp/scaly-bench}/net
NCPU=$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)
T=${1:-$((NCPU / 2))}
D=${2:-8}
CT=$((NCPU - T)); [ "$CT" -lt 1 ] && CT=1
PORT=18090
ND=
[ "${NODELAY:-0}" = 1 ] && ND=nodelay
cpus() { ps -o cputime= -p "$1" 2>/dev/null | awk -F: '{ if (NF==3) print $1*3600+$2*60+$3; else print $1*60+$2 }'; }
run() {
  local label=$1 threads=$2 bin=$W/$3$X
  [ -x "$bin" ] || { echo "$label: not built"; return; }
  PORT=$((PORT + 1))
  SCALY_WORKERS=$threads GOMAXPROCS=$threads TOKIO_WORKER_THREADS=$threads "$bin" $PORT $ND > /dev/null 2>&1 &
  local p=$!
  sleep 1
  local c0; c0=$(cpus $p)
  local out; out=$(GOMAXPROCS=$CT "$W/load$X" -a 127.0.0.1:$PORT -c 128 -d "$D" -p 16)
  local c1; c1=$(cpus $p)
  kill $p; wait $p 2>/dev/null
  printf '%-12s %2s threads  %s  server cpu %s s\n' "$label" "$threads" "$out" "$(echo "$c1 - $c0" | bc)"
}
echo "$NCPU CPUs: servers on 1 and $T threads, the load generator on $CT, ${D}s per run${ND:+, TCP_NODELAY}"
for t in 1 "$T"; do
  run scaly "$t" hello
  run rust-tokio "$t" rust-tokio
  run go-raw "$t" go-raw
  run go-http "$t" go-http
done
