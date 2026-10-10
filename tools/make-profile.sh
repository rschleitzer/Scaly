#!/bin/bash
# Make the profile the distributed compiler and tool are optimised with
# (tools/make-bindist.sh, $SCALY_PGO_PROFILE): the compiler built instrumented
# from its sources, two training runs, the counts merged.
#
#   tools/make-profile.sh [outfile]      (default dist/scalyc.profdata)
#
# The training is the compiler emitting its own package and the stdlib — the
# runs measured 2026-10-02: about -30 % compile time, on
# roots the profile never saw as on the trained one.
#
# ★The profile is made ONCE, on the fast machine, and used on every system we
# build programs for: it counts how often each branch of the compiler was
# taken, keyed by function name and a hash of its control flow, so it depends
# on the compiler's sources and the training input, not on the machine or its
# speed. A function whose hash does not match on another target is optimised
# without a profile; LLVM says so.
# ★It belongs to the SOURCES it was made from: make it again after the
# compiler changed (a stale one costs speed, never correctness).
#
# Needs the tool beside the compiler (scalyc/build/scaly — ./build.sh) and, for
# the instrumented link, the clang and the profile runtime of LLVM 21
# (Ubuntu: libclang-rt-21-dev).
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh > /dev/null
[ "$llvm_env_ok" = "1" ] || { echo "make-profile: FAIL — LLVM $LLVM_MAJOR not found"; exit 1; }

OUT="${1:-dist/scalyc.profdata}"
SCALY="$(tools/scaly-of.sh "${SCALYC:-scalyc/build/scalyc}")"
[ -x "$SCALY" ] || { echo "make-profile: FAIL — $SCALY not found (./build.sh)"; exit 1; }

PROFDATA=""
for c in "$LLVM_PREFIX/bin/llvm-profdata" "llvm-profdata-$LLVM_MAJOR"; do
  command -v "$c" >/dev/null 2>&1 && { PROFDATA="$c"; break; }
done
[ -n "$PROFDATA" ] || { echo "make-profile: FAIL — llvm-profdata of LLVM $LLVM_MAJOR not found"; exit 1; }

W="$(mktemp -d)"
trap 'rm -rf "$W"' EXIT
export SCALY_CACHE="$W/cache"
unset SCALY_HOME

# --export: the instrumented compiler is built as the distributed one will be,
# so the counts fit the functions of that build.
"$SCALY" build packages/scalyc/0.2.0/main.scaly --pgo-train --export \
  -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$W/scalyc_train" > "$W/build.log" 2>&1 \
  || { tail -10 "$W/build.log"; echo "make-profile: FAIL — the instrumented build"; exit 1; }

export LLVM_PROFILE_FILE="$W/train-%p.profraw"
"$W/scalyc_train" -S --no-tests -o "$W/scalyc.ll" packages/scalyc/0.2.0/scalyc.scaly > "$W/train1.log" 2>&1 \
  || { tail -5 "$W/train1.log"; echo "make-profile: FAIL — training run 1 (the compiler's package)"; exit 1; }
"$W/scalyc_train" -S --no-prelude --no-tests -o "$W/scaly.ll" packages/scaly/0.1.1/scaly.scaly > "$W/train2.log" 2>&1 \
  || { tail -5 "$W/train2.log"; echo "make-profile: FAIL — training run 2 (the stdlib)"; exit 1; }
unset LLVM_PROFILE_FILE

mkdir -p "$(dirname "$OUT")"
"$PROFDATA" merge -o "$OUT" "$W"/train-*.profraw
echo "make-profile: OK -> $OUT ($(du -h "$OUT" | cut -f1))"
