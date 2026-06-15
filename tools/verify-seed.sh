#!/bin/bash
# Verify a seed-built compiler on THIS host's target — entirely C++-free.
#
# Three gates (the same ones tools/seed.sh applies to a fresh mint):
#   1. hello.scaly compiles + runs -> "Hello, World!"
#   2. AOT corpus: each tests/aot/*.scaly compiles, runs, and its stdout matches
#      the test's own `; Expected:` comment (no stage-0 reference needed).
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

echo "verify: fixed point vs committed seed/"
for f in main scalyc; do
  "$SC" -S --no-tests -o "$WORK/$f.ll" packages/scalyc/0.1.0/$f.scaly >/dev/null 2>&1 || fail "re-emit $f.ll"
done
"$SC" -S --no-tests -o "$WORK/scaly.ll" packages/scaly/0.1.0/scaly.scaly >/dev/null 2>&1 || fail "re-emit scaly.ll"
for f in main scalyc scaly; do
  cmp -s "$WORK/$f.ll" "seed/$f.ll" || fail "fixed point: $f.ll differs from committed seed"
done

echo "VERIFY: OK — hello + AOT $pass/$total + fixed point byte-identical to seed/"
