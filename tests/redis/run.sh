#!/bin/bash
# tests/redis/run.sh [compiler] — the redis package:
# build every test here with `scaly build` (the package objects come out
# of the build cache), run it under poison and compare stdout with its "; Expected:"
# line. A test that names REDIS_URL in its header needs a server: it runs
# against REDISTEST_URL (e.g. redis://127.0.0.1:6379 of a redis:7-alpine
# container) and SKIPs by name without one.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0; skip=0; failures=()
for f in tests/redis/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  if grep -q REDIS_URL "$f" && [ -z "${REDISTEST_URL:-}" ]; then
    skip=$((skip+1)); echo "  SKIP $t (no REDISTEST_URL)"; continue
  fi
  bin="$TMP/$t$SCALY_EXE"
  if ! "$SCALY" build "$f" -o "$bin" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(REDIS_URL="${REDISTEST_URL:-}" SCALY_POISON=1 perl -e 'alarm shift; exec @ARGV' 60 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "redis: $pass PASS, $fail FAIL, $skip SKIP"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
