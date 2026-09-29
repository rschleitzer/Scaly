#!/bin/bash
# tests/json/run.sh [compiler] — the json package (ROADMAP-http.md, stage B): build its
# archive the way tests/opensp/run.sh builds opensp's, link every test program
# here against it and the runtime archive, run it under poison, and compare
# stdout with its "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive json "$BIN" || exit 1
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if ! "$BIN" -S --no-prelude -o "$TMP/json.ll" packages/json/0.1.0/json.scaly > "$TMP/emit.log" 2>&1; then
  echo "json: FAIL (emit)"; tail -8 "$TMP/emit.log"; exit 1
fi
sed 's/^define linkonce_odr /define weak_odr /' "$TMP/json.ll" > "$TMP/json_weak.ll"
if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/json_weak.ll" -o "$TMP/json.o" > "$TMP/llc.log" 2>&1; then
  echo "json: FAIL (llc)"; tail -8 "$TMP/llc.log"; exit 1
fi
ar rcs "$TMP/libjson.a" "$TMP/json.o"

pass=0; fail=0; failures=()
for f in tests/json/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libjson.a" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(SCALY_POISON=1 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "json: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
