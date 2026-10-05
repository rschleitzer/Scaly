#!/usr/bin/env bash
# Parallel codegen for ONE whole-program module: split the
# OPTIMIZED module into n parts with llvm-split and run one llc per part at once.
# opt still runs over the whole module first, so inlining across the parts is
# untouched; what the split costs is code placement — measured on the tscaly
# bench binary 1-2 % slower at run time, 4 % larger. So the compiler's own build
# (tools/build-from-seed.sh) uses it — never a binary that is measured against
# another compiler (`scaly build --release` emits one object).
#
#   tools/llc-split.sh <n|auto> <input .bc/.ll> <out prefix> [llc args...]
#
# Prints the object files it wrote, one per line: <prefix>.o when n <= 1 or
# llvm-split is missing (one llc, exactly as before), else <prefix>.<k>.o.
# `auto` is the number of logical CPUs.
set -u
N=$1; IN=$2; PFX=$3; shift 3
HERE="$(cd "$(dirname "$0")" && pwd)"
[ -n "${LLC:-}" ] || source "$HERE/llvm-env.sh" >/dev/null 2>&1
if [ "$N" = auto ]; then
  N=$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 1)
fi
if [ "$N" -le 1 ] || [ -z "${LLVM_SPLIT:-}" ]; then
  "$LLC" "$@" "$IN" -o "$PFX.o" || exit 1
  echo "$PFX.o"
  exit 0
fi
"$LLVM_SPLIT" -j "$N" -o "$PFX.part" "$IN" || exit 1
pids=()
k=0
while [ "$k" -lt "$N" ]; do
  "$LLC" "$@" "$PFX.part$k" -o "$PFX.$k.o" & pids+=($!)
  k=$((k + 1))
done
rc=0
for p in "${pids[@]}"; do wait "$p" || rc=1; done
[ "$rc" = 0 ] || exit 1
rm -f "$PFX".part*
k=0
while [ "$k" -lt "$N" ]; do
  echo "$PFX.$k.o"
  k=$((k + 1))
done
