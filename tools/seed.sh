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
#   self-hosted-compiler  default /tmp/scalyc_stage2 (canonical promoted stage);
#                         if it does not exist, tools/bootstrap.sh builds it.
#   out-dir               default dist/seed (gitignored; per-target artifact)
#
# TURNKEY per platform: on a fresh checkout with the deps below installed, run
# `tools/seed.sh` (no args) -> bootstraps stage-2 -> emits + verifies the seed
# for THIS host's target triple (the compiler is host-only; one seed per box).
#   The "fabulous four" LP64 targets and how to install deps:
#     arm64-apple-darwin   brew install llvm@18 cmake openjade
#     x86_64-apple-darwin  (Intel Mac)  same brew formulae
#     x86_64-linux-gnu     apt install llvm-18-dev clang-18 cmake openjade \
#                                      zlib1g-dev libzstd-dev
#     aarch64-linux-gnu    same apt packages (arm64 Ubuntu)
# Override LLVM detection with LLVM18=/path; see tools/llvm-env.sh.
#
# .ll artifacts are NOT checked into the repo: ~12MB, target DataLayout + LLVM
# version baked in (union payload sizes computed at emission). One .ll set per
# target triple, regenerated and fixed-point-verified per release.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "SEED: FAIL — LLVM 18 not found"; exit 1; }

CC=${1:-/tmp/scalyc_stage2}
OUT=${2:-dist/seed}
CLANG=${CLANG:-clang}

mkdir -p "$OUT"
fail() { echo "SEED: FAIL — $1"; exit 1; }

if [ ! -x "$CC" ]; then
  echo "seed: $CC not found — bootstrapping"
  tools/bootstrap.sh || fail "bootstrap"
  CC=/tmp/scalyc_stage2          # bootstrap.sh's output
  [ -x "$CC" ] || fail "bootstrap produced no $CC"
fi

echo "seed: emitting .ll with $CC --no-tests"
( ulimit -s 65520
  for f in main scalyc; do
    "$CC" -S --no-tests -o "$OUT/$f.ll" packages/scalyc/0.1.0/$f.scaly || exit 1
  done
  "$CC" -S --no-tests -o "$OUT/scaly.ll" packages/scaly/0.1.0/scaly.scaly || exit 1
) || fail "emission"

echo "seed: llc -> obj (LLVM 18) + link ($CLANG, no dynamic_lookup)"
for f in main scalyc scaly; do
  # -relocation-model=pic: x86-64 Linux PIE rejects llc's default R_X86_64_32
  # abs relocations; PIC is the Mach-O default, so this is a no-op on macOS.
  "$LLC" -relocation-model=pic -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" || fail "llc $f.ll"
done
# NO -Wl,-undefined,dynamic_lookup — a clean link proves zero undefined.
# (LLVM-18 llc is required: some system clangs reject the seed's
# mul/ptrtoint-getelementptr constexprs; llc-18 accepts them. The final link
# is plain object linking, so any clang/cc works.)
if ! "$CLANG" "$OUT/main.o" "$OUT/scalyc.o" "$OUT/scaly.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT/scalyc_seed" 2> "$OUT/link.log"; then
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
