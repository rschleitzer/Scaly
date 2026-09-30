#!/bin/bash
# tests/tls/run.sh [compiler] — the tls and https packages (ROADMAP-http.md,
# stage D; TLS 1.3 through OpenSSL): build their archives (and http's), make
# a self-signed RSA-2048 certificate for the run, link every test program here against the archives, the runtime archive and
# libssl/libcrypto, run it under poison with the certificate and key as its
# arguments, and compare stdout with its "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive tls "$BIN" || exit 1
set -u

# OpenSSL 3: Homebrew's prefix, else the system's library directories. A
# host without it SKIPs by name -- never a vacuous pass.
SSL=(); OPENSSL=openssl
if command -v brew > /dev/null 2>&1 && p=$(brew --prefix openssl@3 2> /dev/null) && [ -e "$p/lib/libssl.dylib" ]; then
  SSL=(-L"$p/lib" -lssl -lcrypto); OPENSSL="$p/bin/openssl"
else
  for d in /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu /usr/lib64 /usr/lib /usr/local/lib; do
    if ls "$d"/libssl.so* > /dev/null 2>&1; then SSL=(-L"$d" -lssl -lcrypto); break; fi
  done
fi
if [ ${#SSL[@]} = 0 ] || ! command -v "$OPENSSL" > /dev/null 2>&1; then
  echo "tls: SKIP (no OpenSSL 3: brew install openssl@3 / apt install libssl-dev openssl)"
  exit 0
fi

# OpenSSL 3. (Until 2026-09-30 https served HTTP/3 through OpenSSL's QUIC
# server and needed 3.5; that is the h3 package now, and its suite asks.)
ver=$("$OPENSSL" version 2>/dev/null | awk '{print $2}')
major=${ver%%.*}
if [ -z "$ver" ] || [ "${major:-0}" -lt 3 ]; then
  echo "tls: SKIP (OpenSSL ${ver:-?}; the tls package needs 3 or later)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if ! "$OPENSSL" req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=localhost \
     -keyout "$TMP/key.pem" -out "$TMP/cert.pem" > "$TMP/cert.log" 2>&1; then
  echo "tls: FAIL (certificate)"; tail -3 "$TMP/cert.log"; exit 1
fi

# The archives of tls, http and https (the last instantiates http's
# connection loop over a TlsStream), each the way tests/compress/run.sh builds
# compress's.
archive() {
  if ! "$BIN" -S --no-prelude -o "$TMP/$1.ll" packages/$1/0.1.0/$1.scaly > "$TMP/emit_$1.log" 2>&1; then
    echo "tls: FAIL (emit $1)"; tail -8 "$TMP/emit_$1.log"; exit 1
  fi
  sed 's/^define linkonce_odr /define weak_odr /' "$TMP/$1.ll" > "$TMP/$1_weak.ll"
  if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/$1_weak.ll" -o "$TMP/$1.o" > "$TMP/llc_$1.log" 2>&1; then
    echo "tls: FAIL (llc $1)"; tail -8 "$TMP/llc_$1.log"; exit 1
  fi
  ar rcs "$TMP/lib$1.a" "$TMP/$1.o"
}
archive tls
archive http
archive https

pass=0; fail=0; failures=()
for f in tests/tls/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libhttps.a" "$TMP/libhttp.a" "$TMP/libtls.a" "${SSL[@]}" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  # a lost record stalls both sides of a handshake instead of failing it:
  # 30 s and the test is killed (perl's alarm -- no `timeout` on macOS)
  out=$(SCALY_POISON=1 perl -e 'alarm shift; exec @ARGV' 30 "$bin" "$TMP/cert.pem" "$TMP/key.pem" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "tls: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
