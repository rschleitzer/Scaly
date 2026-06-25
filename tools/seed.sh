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

# ---------------------------------------------------------------------------
# scalyls language server seed (separate program; NOT part of the compiler
# fixed point above).
#
# scalyls depends on the scalyc compiler PACKAGE. It is now emitted by the
# SELF-HOSTED compiler ($CC, the same stage-2 that minted the compiler seed),
# PER-PACKAGE-ROOT like the compiler seed: a tiny scalyls main.ll (extern
# server.run) + scalyls.ll (the LSP package bodies), linked against the
# already-emitted scalyc.ll + scaly.ll (the compiler package + stdlib). The
# four roots link with just -lLLVM-18 and need no libscaly.a. There is NO
# fixed-point requirement (scalyls does not self-compile): mint it, link it
# clean (zero undefined), and smoke-test an LSP initialize round-trip.
echo "seed: emitting scalyls (4-root, self-hosted) with $CC --no-tests"
( ulimit -s 65520
  "$CC" -S --no-tests -o "$OUT/lsmain.ll"  packages/scalyls/0.1.0/main.scaly    || exit 1
  "$CC" -S --no-tests -o "$OUT/scalyls.ll" packages/scalyls/0.1.0/scalyls.scaly || exit 1
) || fail "scalyls emission"
# scalyc.o + scaly.o were produced by the compiler-seed llc loop above; only
# the two scalyls-specific roots need lowering here.
for f in lsmain scalyls; do
  "$LLC" -relocation-model=pic -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" || fail "llc $f.ll"
done
if ! "$CLANG" "$OUT/lsmain.o" "$OUT/scalyls.o" "$OUT/scalyc.o" "$OUT/scaly.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT/scalyls" 2> "$OUT/scalyls_link.log"; then
  grep -v 'reexported library' "$OUT/scalyls_link.log" || true
  fail "scalyls link (undefined symbols)"
fi
echo "seed: scalyls linked clean -> $OUT/scalyls"
echo "seed: scalyls LSP smoke (initialize -> capabilities)"
python3 - "$OUT/scalyls" <<'PY' || fail "scalyls smoke"
import sys, json, subprocess
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
inp  = frame({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}})
inp += frame({"jsonrpc": "2.0", "id": 2, "method": "shutdown"})
inp += frame({"jsonrpc": "2.0", "method": "exit"})
out = subprocess.run([sys.argv[1]], input=inp, stdout=subprocess.PIPE, timeout=30).stdout
i = out.find(b"\r\n\r\n")
if i < 0:
    sys.exit("no response frame")
n = int(out[:i].decode().split(":")[1].strip())
f0 = json.loads(out[i + 4 : i + 4 + n])
sys.exit(0 if (f0.get("id") == 1 and "capabilities" in f0.get("result", {})) else
         "bad initialize response")
PY

echo "SEED: OK — compiler links clean, runs hello + AOT, reproduces itself"
echo "          byte-identical; scalyls language server links + serves"
