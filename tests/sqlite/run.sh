#!/bin/bash
# tests/sqlite/run.sh [compiler] — the sqlite package
# (SQLite's own library): build every test program here with `scaly
# build` (libsqlite3 through the package's `extern`), run it under poison, and compare stdout with its
# "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

# SQLite's library: Homebrew's prefix, else the system's library directories.
# A host without it SKIPs by name -- never a vacuous pass.
FOUND=
if command -v brew > /dev/null 2>&1 && p=$(brew --prefix sqlite 2> /dev/null) && [ -e "$p/lib/libsqlite3.dylib" ]; then
  FOUND=1
else
  for d in /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu /usr/lib64 /usr/lib /usr/local/lib; do
    if ls "$d"/libsqlite3.* > /dev/null 2>&1; then FOUND=1; break; fi
  done
fi
if [ -z "$FOUND" ]; then
  echo "sqlite: SKIP (no libsqlite3: brew install sqlite / apt install libsqlite3-dev)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0; failures=()
for f in tests/sqlite/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$SCALY" build "$f" -o "$bin" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(SCALY_POISON=1 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "sqlite: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
