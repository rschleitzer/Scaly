#!/bin/bash
# tests/compress/run.sh [compiler] — the compress package (ROADMAP-http.md,
# stage B; gzip over libdeflate): build every test program here with `scaly
# build` and libdeflate, run it under poison, and compare stdout with its
# "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

# libdeflate: Homebrew's prefix, else the system's library directories. A host
# without it SKIPs by name -- never a vacuous pass.
DEFLATE=()
if command -v brew > /dev/null 2>&1 && p=$(brew --prefix libdeflate 2> /dev/null) && [ -e "$p/lib/libdeflate.dylib" ]; then
  DEFLATE=(-L"$p/lib" -ldeflate)
else
  for d in /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu /usr/lib64 /usr/lib /usr/local/lib; do
    if ls "$d"/libdeflate.* > /dev/null 2>&1; then DEFLATE=(-L"$d" -ldeflate); break; fi
  done
fi
if [ ${#DEFLATE[@]} = 0 ]; then
  echo "compress: SKIP (no libdeflate: brew install libdeflate / apt install libdeflate-dev)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0; failures=()
for f in tests/compress/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" build "$f" -o "$bin" "${DEFLATE[@]}" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(SCALY_POISON=1 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "compress: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
