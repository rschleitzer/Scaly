#!/bin/bash
# tests/pg/run.sh [compiler] — the pg package (ROADMAP-http.md, stage C):
# build the tls and pg archives the way tests/tls/run.sh does, link every
# test here against them, the runtime archive and libssl/libcrypto, run it
# under poison and compare stdout with its "; Expected:" line. A test that
# names DATABASE_URL in its header needs a server: it runs against
# PGTEST_URL (e.g. HttpArena's seed in postgres:18) and SKIPs by name
# without one.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive pg "$BIN" || exit 1
set -u

SSL=(); OPENSSL=openssl
if command -v brew > /dev/null 2>&1 && p=$(brew --prefix openssl@3 2> /dev/null) && [ -e "$p/lib/libssl.dylib" ]; then
  SSL=(-L"$p/lib" -lssl -lcrypto); OPENSSL="$p/bin/openssl"
else
  for d in /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu /usr/lib64 /usr/lib /usr/local/lib; do
    if ls "$d"/libssl.so* > /dev/null 2>&1; then SSL=(-L"$d" -lssl -lcrypto); break; fi
  done
fi
if [ ${#SSL[@]} = 0 ]; then
  echo "pg: SKIP (no OpenSSL 3: brew install openssl@3 / apt install libssl-dev)"
  exit 0
fi

# the tls package needs OpenSSL 3 (3.5 only since h3 moved out of https,
# 2026-09-30: h3's own suite asks for it)
ver=$("$OPENSSL" version 2>/dev/null | awk '{print $2}')
major=${ver%%.*}
if [ -z "$ver" ] || [ "${major:-0}" -lt 3 ]; then
  echo "pg: SKIP (OpenSSL ${ver:-?}; the tls package needs 3 or later)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

archive() {
  if ! "$BIN" -S --no-prelude -o "$TMP/$1.ll" packages/$1/0.1.0/$1.scaly > "$TMP/emit_$1.log" 2>&1; then
    echo "pg: FAIL (emit $1)"; tail -8 "$TMP/emit_$1.log"; exit 1
  fi
  sed 's/^define linkonce_odr /define weak_odr /' "$TMP/$1.ll" > "$TMP/$1_weak.ll"
  if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/$1_weak.ll" -o "$TMP/$1.o" > "$TMP/llc_$1.log" 2>&1; then
    echo "pg: FAIL (llc $1)"; tail -8 "$TMP/llc_$1.log"; exit 1
  fi
  ar rcs "$TMP/lib$1.a" "$TMP/$1.o"
}
archive tls
archive pg

pass=0; fail=0; skip=0; failures=()
for f in tests/pg/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  if grep -q DATABASE_URL "$f" && [ -z "${PGTEST_URL:-}" ]; then
    skip=$((skip+1)); echo "  SKIP $t (no PGTEST_URL)"; continue
  fi
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libpg.a" "$TMP/libtls.a" "${SSL[@]}" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(DATABASE_URL="${PGTEST_URL:-}" SCALY_POISON=1 perl -e 'alarm shift; exec @ARGV' 60 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "pg: $pass PASS, $fail FAIL, $skip SKIP"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
