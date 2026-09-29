#!/bin/bash
# tests/redis/run.sh [compiler] — the redis package (ROADMAP-http.md, stage
# C): build its archive, link every test here against it and the runtime
# archive, run it under poison and compare stdout with its "; Expected:"
# line. A test that names REDIS_URL in its header needs a server: it runs
# against REDISTEST_URL (e.g. redis://127.0.0.1:6379 of a redis:7-alpine
# container) and SKIPs by name without one.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive redis "$BIN" || exit 1
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if ! "$BIN" -S --no-prelude -o "$TMP/redis.ll" packages/redis/0.1.0/redis.scaly > "$TMP/emit.log" 2>&1; then
  echo "redis: FAIL (emit)"; tail -8 "$TMP/emit.log"; exit 1
fi
sed 's/^define linkonce_odr /define weak_odr /' "$TMP/redis.ll" > "$TMP/redis_weak.ll"
if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/redis_weak.ll" -o "$TMP/redis.o" > "$TMP/llc.log" 2>&1; then
  echo "redis: FAIL (llc)"; tail -8 "$TMP/llc.log"; exit 1
fi
ar rcs "$TMP/libredis.a" "$TMP/redis.o"

pass=0; fail=0; skip=0; failures=()
for f in tests/redis/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  if grep -q REDIS_URL "$f" && [ -z "${REDISTEST_URL:-}" ]; then
    skip=$((skip+1)); echo "  SKIP $t (no REDISTEST_URL)"; continue
  fi
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libredis.a" > "$TMP/$t.log" 2>&1; then
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
