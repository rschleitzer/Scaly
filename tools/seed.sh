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
# scalyls depends on the scalyc compiler PACKAGE. The self-hosted compiler can
# now EMIT + LINK the whole scalyls package per-root (the multi-package
# emission gap is closed), and the resulting server initializes — BUT its LSP
# feature functions are still miscompiled (e.g. parse_program# returns Success
# with program.file.declarations null, so document_symbols#/hover/inlay return
# empty). Until that self-hosted-codegen class is fixed module-by-module,
# scalyls.ll is emitted by the C++ stage-0 (scalyc/build/scalyc) — the only
# compiler that builds a FUNCTIONALLY correct scalyls. It is a SELF-CONTAINED
# .ll (the scaly stdlib + scalyc compiler are baked in), so it links standalone
# with just -lLLVM-18 and needs no libscaly.a. There is NO fixed-point
# requirement (scalyls does not self-compile): mint it, link it clean (zero
# undefined), and smoke-test an LSP initialize round-trip.
SCALYLS_CC=${SCALYLS_CC:-scalyc/build/scalyc}
if [ ! -x "$SCALYLS_CC" ]; then
  echo "seed: building C++ stage-0 for scalyls ($SCALYLS_CC missing)"
  ./build.sh >/dev/null 2>&1 || fail "build.sh (C++ stage-0 for scalyls)"
fi
# NOTE: no --no-tests — that flag is self-hosted-only; the C++ stage-0 rejects
# it. scalyls emitted by stage-0 links clean WITH test functions present (they
# are unreferenced from server.run and the linker drops them), verified below.
echo "seed: emitting scalyls.ll with $SCALYLS_CC"
( ulimit -s 65520
  "$SCALYLS_CC" -S -o "$OUT/scalyls.ll" packages/scalyls/0.1.0/main.scaly
) || fail "scalyls emission"
"$LLC" -relocation-model=pic -filetype=obj "$OUT/scalyls.ll" -o "$OUT/scalyls.o" || fail "llc scalyls.ll"
if ! "$CLANG" "$OUT/scalyls.o" -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" \
     -o "$OUT/scalyls" 2> "$OUT/scalyls_link.log"; then
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
