#!/bin/bash
# Build the .ll seed (Step D distribution) and verify the fixed point.
#
# The seed is the self-hosted compiler shipped as its own emitted LLVM IR:
# main.ll + scalyc.ll + scaly.ll, emitted with --no-tests (test/test_*
# functions are dead from cli.main and reference uninstantiated generics, so
# omitting them lets the seed link with zero undefined symbols). The seed is
# the FIRST execution of stage-emitted stdlib code — the bootstrap fixed point
# never RUNS it (stage binaries link C++-built libscaly.a), so this script
# both BUILDS and VERIFIES the seed end-to-end.
#
# Verifies:
#   1. seed links with NO undefined symbols (no -Wl,-undefined,dynamic_lookup)
#   2. seed compiles+runs hello.scaly  -> "Hello, World!"
#   3. seed compiles+runs the AOT corpus (tests/aot)
#   4. FIXED POINT: seed re-emits the 3 .ll byte-identical to the reference
#
# Usage: tools/seed.sh [self-hosted-compiler] [out-dir]
#   self-hosted-compiler  default /tmp/scalyc_stage2 (canonical promoted stage)
#   out-dir               default dist/seed (gitignored; per-target artifact)
#
# .ll artifacts are NOT checked into the repo: ~12MB, arm64 DataLayout + LLVM
# version baked in (union payload sizes computed at emission). One .ll set per
# target triple, regenerated and fixed-point-verified per release.
set -e
cd "$(dirname "$0")/.."

CC=${1:-/tmp/scalyc_stage2}
OUT=${2:-dist/seed}
L=/opt/homebrew/opt/llvm@18
LLC=$L/bin/llc

mkdir -p "$OUT"
fail() { echo "SEED: FAIL — $1"; exit 1; }

echo "seed: emitting .ll with $CC --no-tests"
( ulimit -s 65520
  for f in main scalyc; do
    "$CC" -S --no-tests -o "$OUT/$f.ll" packages/scalyc/0.1.0/$f.scaly || exit 1
  done
  "$CC" -S --no-tests -o "$OUT/scaly.ll" packages/scaly/0.1.0/scaly.scaly || exit 1
) || fail "emission"

echo "seed: llc -> obj (llvm@18) + link (system clang, no dynamic_lookup)"
for f in main scalyc scaly; do
  "$LLC" -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" || fail "llc $f.ll"
done
# System clang: Apple ld. NO -Wl,-undefined,dynamic_lookup — a clean link
# proves zero undefined. (llvm@18 llc is required: Apple clang rejects the
# seed's mul/ptrtoint-getelementptr constexprs.)
if ! clang "$OUT/main.o" "$OUT/scalyc.o" "$OUT/scaly.o" \
     -L$L/lib -lLLVM-18 -o "$OUT/scalyc_seed" 2> "$OUT/link.log"; then
  grep -v 'reexported library' "$OUT/link.log" || true
  fail "link (undefined symbols)"
fi
echo "seed: linked clean -> $OUT/scalyc_seed"

echo "seed: hello.scaly"
"$OUT/scalyc_seed" -o "$OUT/hello" tests/aot/hello.scaly || fail "compile hello"
out=$("$OUT/hello")
[ "$out" = "Hello, World!" ] || fail "hello output: '$out'"

echo "seed: AOT corpus"
tools/aot_corpus.sh "$OUT/scalyc_seed" seed | tail -1

echo "seed: fixed-point self-reproduction"
( ulimit -s 65520
  for f in main scalyc; do
    "$OUT/scalyc_seed" -S --no-tests -o "$OUT/r_$f.ll" packages/scalyc/0.1.0/$f.scaly || exit 1
  done
  "$OUT/scalyc_seed" -S --no-tests -o "$OUT/r_scaly.ll" packages/scaly/0.1.0/scaly.scaly || exit 1
) || fail "re-emission"
for f in main scalyc scaly; do
  cmp -s "$OUT/r_$f.ll" "$OUT/$f.ll" || fail "fixed point: $f.ll differs"
done
echo "SEED: OK — links clean, runs hello + AOT, reproduces itself byte-identical"
