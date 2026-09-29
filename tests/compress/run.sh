#!/bin/bash
# tests/compress/run.sh [compiler] — the compress package (ROADMAP-http.md,
# stage B; gzip over libdeflate): build its archive the way tests/opensp/run.sh
# builds opensp's, link every test program here against it, the runtime
# archive and libdeflate, run it under poison, and compare stdout with its
# "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive compress "$BIN" || exit 1
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

if ! "$BIN" -S --no-prelude -o "$TMP/compress.ll" packages/compress/0.1.0/compress.scaly > "$TMP/emit.log" 2>&1; then
  echo "compress: FAIL (emit)"; tail -8 "$TMP/emit.log"; exit 1
fi
sed 's/^define linkonce_odr /define weak_odr /' "$TMP/compress.ll" > "$TMP/compress_weak.ll"
if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/compress_weak.ll" -o "$TMP/compress.o" > "$TMP/llc.log" 2>&1; then
  echo "compress: FAIL (llc)"; tail -8 "$TMP/llc.log"; exit 1
fi
ar rcs "$TMP/libcompress.a" "$TMP/compress.o"

pass=0; fail=0; failures=()
for f in tests/compress/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libcompress.a" "${DEFLATE[@]}" > "$TMP/$t.log" 2>&1; then
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
