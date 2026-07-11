#!/bin/bash
# Stage-7 finale (ROADMAP 7.4): the DISTRIBUTED Mann trainer. Trains the
# same 2-layer char transformer as demo/run.sh, but by synchronous
# data-parallel SGD across N OS processes that all-reduce their gradient
# regions over TCP (stage-7 remote channels) — pure Scaly, no MPI/NCCL.
# The reducer (rank 0) prints progress + throughput and, when training
# finishes, generates German text from the trained model. Every process
# runs the SAME binary (memo rule 10: one cluster, one build).
#
# Pins the cwd to the repo root: the corpus (tests/tensor/mann.txt) and
# the compiler's packages/ + runtime archive all resolve cwd-relative.
#
# Usage:
#   demo/run_mann_dist.sh [N] [compiler]        distributed, N nodes (default 4)
#   demo/run_mann_dist.sh solo [N] [compiler]   single-process baseline, batch N
set -e
cd "$(dirname "$0")/.."

MODE=dist
if [ "$1" = "solo" ]; then MODE=solo; shift; fi
N=${1:-4}
SCALYC=${2:-scalyc/build/scalyc}
PORT=${MANN_PORT:-47800}

if [ ! -x "$SCALYC" ]; then
  echo "error: $SCALYC not found - run ./build.sh first" >&2
  exit 1
fi

# The tape kernels run at the ARCHIVE's opt level: build /tmp/libscaly.a
# at -O2 (never -c -O2 a library — GlobalDCE would delete every body).
if [ ! -f /tmp/libscaly.a ] || find packages/scaly -newer /tmp/libscaly.a | grep -q .; then
  echo "building -O2 runtime archive..."
  source tools/llvm-env.sh
  "$SCALYC" -S --no-prelude --no-tests -o /tmp/libscaly.ll packages/scaly/0.1.0/scaly.scaly
  sed 's/^define linkonce_odr /define weak_odr /' /tmp/libscaly.ll > /tmp/libscaly_weak.ll
  opt -O2 /tmp/libscaly_weak.ll -o /tmp/libscaly_opt.bc
  llc -relocation-model=pic -O2 -filetype=obj /tmp/libscaly_opt.bc -o /tmp/libscaly.o
  tools/fcontext.sh /tmp/fcontext.o
  tools/eio.sh /tmp/eio.o
  ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o
fi

BIN=/tmp/scaly_mann_dist
rm -f "$BIN"
"$SCALYC" -O2 -o "$BIN" demo/mann_dist.scaly

if [ "$MODE" = "solo" ]; then
  exec env MANN_N="$N" "$BIN"
fi

# Distributed: reducer (rank 0) in the foreground terminal, workers quiet
# in the background. Workers retry-connect, so ordering is race-free.
echo "launching $N nodes (1 reducer + $((N-1)) workers) on port $PORT ..."
pids=()
r=1
while [ "$r" -lt "$N" ]; do
  MANN_N="$N" MANN_RANK="$r" MANN_PORT="$PORT" "$BIN" >/dev/null 2>&1 &
  pids+=($!)
  r=$((r + 1))
done

# Clean up stray workers if the reducer dies early.
cleanup() { for p in "${pids[@]}"; do kill "$p" 2>/dev/null; done; }
trap cleanup EXIT

MANN_N="$N" MANN_RANK=0 MANN_PORT="$PORT" "$BIN"
RC=$?
for p in "${pids[@]}"; do wait "$p" 2>/dev/null; done
trap - EXIT
exit $RC
