#!/bin/bash
# Verify a seed-built compiler on THIS host's target — entirely C++-free.
#
# Three gates (the same ones tools/seed.sh applies to a fresh mint):
#   1. hello.scaly compiles + runs -> "Hello, World!"
#   2. AOT corpus: each tests/aot/*.scaly compiles, runs, and its stdout matches
#      the test's own `; Expected:` comment (no stage-0 reference needed).
#   2b. Regression suite (tests/regress): self-hosted-only fixes that the frozen
#      C++ stage-0 lacks; each must print PASS.
#   3. FIXED POINT: the compiler re-emits main/scalyc/scaly .ll byte-identical to
#      the committed seed/. This is the real proof that the single committed seed
#      is valid on this target.
#
# Usage: tools/verify-seed.sh [compiler]   (default scalyc/build/scalyc)
set -e
cd "$(dirname "$0")/.."

SC=${1:-scalyc/build/scalyc}
fail() { echo "VERIFY: FAIL — $1"; exit 1; }
[ -x "$SC" ] || fail "compiler $SC not found (run tools/build-from-seed.sh first)"

# Deep planner frames; harmless if the platform caps lower (s178 made it fit 8MB).
ulimit -s 65520 2>/dev/null || true

WORK=$(mktemp -d); trap 'rm -rf "$WORK"' EXIT

echo "verify: hello.scaly"
"$SC" -o "$WORK/hello" tests/aot/hello.scaly >/dev/null 2>&1 || fail "compile hello"
out=$("$WORK/hello"); [ "$out" = "Hello, World!" ] || fail "hello output: '$out'"

echo "verify: AOT corpus (self-check vs ; Expected:)"
# scalyc_test_exit pulls in the whole scalyc package (its own _main), so it is
# not a standalone AOT program — aot_corpus.sh skips it for the same reason.
SKIP="scalyc_test_exit"
pass=0; total=0; skipped=0; bad=""
for f in tests/aot/*.scaly; do
  t=$(basename "$f" .scaly)
  case " $SKIP " in *" $t "*) skipped=$((skipped+1)); continue;; esac
  total=$((total+1))
  if ! "$SC" -o "$WORK/$t" "$f" >/dev/null 2>&1; then bad="$bad $t(compile)"; continue; fi
  exp=$(grep -m1 '; Expected:' "$f" | sed -e 's/.*; Expected:[[:space:]]*//' -e 's/[[:space:]]*$//')
  if [ -n "$exp" ]; then
    got=$("$WORK/$t" 2>/dev/null || true)
    if [ "$got" = "$exp" ]; then pass=$((pass+1)); else bad="$bad $t(got:'$got'!='$exp')"; fi
  else
    pass=$((pass+1))   # no Expected line: a clean compile is the bar
  fi
done
[ -z "$bad" ] || fail "AOT:$bad"
echo "verify: AOT $pass/$total OK ($skipped skipped)"

# Self-hosted regression suite — fixes where the self-hosted compiler is strictly
# more correct than the frozen C++ stage-0 (so they can't live in the stage-0-
# referenced AOT corpus). Each test must print PASS.
echo "verify: regression suite"
tests/regress/run.sh "$SC" || fail "regression suite"

# scalyls language server (when the seed shipped it): on THIS target, link it
# from the committed seed IR — scalyls' own main + root plus the scalyc and
# scaly packages it depends on, and the fcontext/eio runtime objects — the
# same objects tools/seed.sh links a fresh mint from. Smoke-test an LSP
# initialize round-trip. Best-effort: skipped if LLVM 20 is not resolvable.
if [ -f seed/scalyls.ll ] && [ -f seed/scalyls_main.ll ]; then
  echo "verify: scalyls language server"
  # shellcheck disable=SC1091
  . tools/llvm-env.sh 2>/dev/null || true
  if [ "$llvm_env_ok" = "1" ]; then
    LD_ARG=""
    if [ "$(uname -s)" = "Linux" ]; then
      for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld ld.lld-20; do
        command -v "$c" >/dev/null 2>&1 && { LD_ARG="-fuse-ld=$c"; break; }
      done
    fi
    for f in scalyls_main scalyls scalyc scaly; do
      "$LLC" -relocation-model=pic -filetype=obj "seed/$f.ll" -o "$WORK/$f.o" \
        >/dev/null 2>&1 || fail "llc $f.ll"
    done
    CLANG="${CLANG:-clang}" tools/fcontext.sh "$WORK/fcontext.o" >/dev/null 2>&1 || fail "fcontext assembly"
    CLANG="${CLANG:-clang}" tools/eio.sh "$WORK/eio.o" >/dev/null 2>&1 || fail "eio shim compile"
    CLANG="${CLANG:-clang}" tools/ctime.sh "$WORK/ctime.o" >/dev/null 2>&1 || fail "ctime shim compile"
    CLANG="${CLANG:-clang}" tools/panic.sh "$WORK/panic.o" >/dev/null 2>&1 || fail "panic shim compile"
    # Keep the linker's stderr and PRINT it on failure (seed.sh's idiom): this
    # link used to discard it, so a missing -lm on Linux reported a bare
    # "VERIFY: FAIL — link scalyls" with no undefined symbol named.
    # shellcheck disable=SC2086
    if ! "${CLANG:-clang}" $LD_ARG "$WORK/scalyls_main.o" "$WORK/scalyls.o" "$WORK/scalyc.o" \
      "$WORK/scaly.o" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" "$WORK/panic.o" -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" \
      -lm -o "$WORK/scalyls" 2> "$WORK/scalyls_link.log"; then
      grep -v 'reexported library' "$WORK/scalyls_link.log" || true
      fail "link scalyls"
    fi
    python3 - "$WORK/scalyls" <<'PY' || fail "scalyls smoke (initialize)"
import sys, json, subprocess
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
inp  = frame({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}})
inp += frame({"jsonrpc": "2.0", "id": 2, "method": "shutdown"})
inp += frame({"jsonrpc": "2.0", "method": "exit"})
out = subprocess.run([sys.argv[1]], input=inp, stdout=subprocess.PIPE, timeout=30).stdout
i = out.find(b"\r\n\r\n")
if i < 0: sys.exit("no response frame")
n = int(out[:i].decode().split(":")[1].strip())
f0 = json.loads(out[i + 4 : i + 4 + n])
sys.exit(0 if (f0.get("id") == 1 and "capabilities" in f0.get("result", {})) else "bad response")
PY
    echo "verify: scalyls OK (links + serves on this target)"
  else
    echo "verify: scalyls SKIP (LLVM 20 not resolved)"
  fi
fi

# Byte-identical fixed-point re-emit proves the compiler reproduces the exact
# committed seed on this target. OFF by default: re-emitting scalyc.ll peaks at
# ~15 GB (main ~1 GB, scaly ~5.6 GB), over free CI runners. Run it at release
# time on a high-RAM machine:  VERIFY_FIXEDPOINT=1 tools/verify-seed.sh
if [ -n "$VERIFY_FIXEDPOINT" ]; then
  echo "verify: fixed point vs committed seed/ (re-emit; high memory)"
  for f in main scalyc; do
    "$SC" -S --no-tests -o "$WORK/$f.ll" packages/scalyc/0.1.0/$f.scaly >/dev/null 2>&1 || fail "re-emit $f.ll"
  done
  "$SC" -S --no-tests -o "$WORK/scaly.ll" packages/scaly/0.1.0/scaly.scaly >/dev/null 2>&1 || fail "re-emit scaly.ll"
  for f in main scalyc scaly; do
    cmp -s "$WORK/$f.ll" "seed/$f.ll" || fail "fixed point: $f.ll differs from committed seed"
  done
  echo "VERIFY: OK — hello + AOT $pass/$total + regress + fixed point byte-identical to seed/"
else
  echo "VERIFY: OK — hello + AOT $pass/$total + regress (fixed point skipped; set VERIFY_FIXEDPOINT=1 to include it)"
fi
