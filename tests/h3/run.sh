#!/bin/bash
# tests/h3/run.sh [compiler] — the h3 package (ROADMAP-http.md, stage G):
# build the tls, http and h3 archives and h3/ngtcp2_glue.c, link every test
# here against them, ngtcp2 and OpenSSL, run it under poison with a fresh
# self-signed certificate and a UDP port of its own, and compare stdout with
# its "; Expected:" line. A host without ngtcp2 (with its crypto_ossl) or
# without OpenSSL 3.5 SKIPs by name.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

source tools/llvm-env.sh >/dev/null 2>&1
. tests/platform.sh || exit 1
scaly_need_archive h3 "$BIN" || exit 1
set -u

# OpenSSL 3.5+ and ngtcp2: Homebrew's prefixes, else /opt (the arena image's)
# and the system's.
SSLP=""; NGP=""; OPENSSL=openssl
if command -v brew > /dev/null 2>&1; then
  p=$(brew --prefix openssl@3 2> /dev/null) && [ -e "$p/include/openssl/ssl.h" ] && SSLP="$p" && OPENSSL="$p/bin/openssl"
  p=$(brew --prefix libngtcp2 2> /dev/null) && [ -e "$p/include/ngtcp2/ngtcp2_crypto_ossl.h" ] && NGP="$p"
fi
[ -z "$SSLP" ] && [ -e /opt/openssl/include/openssl/ssl.h ] && SSLP=/opt/openssl
[ -z "$NGP" ] && [ -e /opt/ngtcp2/include/ngtcp2/ngtcp2_crypto_ossl.h ] && NGP=/opt/ngtcp2
[ -z "$NGP" ] && [ -e /usr/local/include/ngtcp2/ngtcp2_crypto_ossl.h ] && NGP=/usr/local
if [ -z "$NGP" ]; then
  echo "h3: SKIP (no ngtcp2 with crypto_ossl: brew install libngtcp2)"
  exit 0
fi
SSLINC=(); SSLLIB=(-lssl -lcrypto)
if [ -n "$SSLP" ]; then SSLINC=(-I"$SSLP/include"); SSLLIB=(-L"$SSLP/lib" -lssl -lcrypto); fi
ver=$("$OPENSSL" version 2>/dev/null | awk '{print $2}')
major=${ver%%.*}; rest=${ver#*.}; minor=${rest%%.*}
if [ -z "$ver" ] || [ "${major:-0}" -lt 3 ] || { [ "$major" = 3 ] && [ "${minor:-0}" -lt 5 ]; }; then
  echo "h3: SKIP (OpenSSL ${ver:-?}; ngtcp2's crypto_ossl needs 3.5 or later)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

if ! "$OPENSSL" req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=localhost \
     -keyout "$TMP/key.pem" -out "$TMP/cert.pem" > "$TMP/cert.log" 2>&1; then
  echo "h3: FAIL (certificate)"; tail -3 "$TMP/cert.log"; exit 1
fi

archive() {
  if ! "$BIN" -S --no-prelude -o "$TMP/$1.ll" packages/$1/0.1.0/$1.scaly > "$TMP/emit_$1.log" 2>&1; then
    echo "h3: FAIL (emit $1)"; tail -8 "$TMP/emit_$1.log"; exit 1
  fi
  sed 's/^define linkonce_odr /define weak_odr /' "$TMP/$1.ll" > "$TMP/$1_weak.ll"
  if ! "$LLC" -relocation-model=pic -O2 -filetype=obj "$TMP/$1_weak.ll" -o "$TMP/$1.o" > "$TMP/llc_$1.log" 2>&1; then
    echo "h3: FAIL (llc $1)"; tail -8 "$TMP/llc_$1.log"; exit 1
  fi
  ar rcs "$TMP/lib$1.a" "$TMP/$1.o"
}
archive tls
archive http
archive h3
if ! ${CLANG:-clang} -O2 -c packages/h3/0.1.0/h3/ngtcp2_glue.c -I"$NGP/include" "${SSLINC[@]+"${SSLINC[@]}"}" \
     -o "$TMP/ngtcp2_glue.o" > "$TMP/glue.log" 2>&1; then
  echo "h3: FAIL (glue)"; tail -5 "$TMP/glue.log"; exit 1
fi
ar rcs "$TMP/libh3glue.a" "$TMP/ngtcp2_glue.o"

pass=0; fail=0; failures=()
for f in tests/h3/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$BIN" -o "$bin" "$f" "$TMP/libh3.a" "$TMP/libh3glue.a" "$TMP/libhttp.a" "$TMP/libtls.a" \
       -L"$NGP/lib" -lngtcp2_crypto_ossl -lngtcp2 "${SSLLIB[@]}" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  port=$((20000 + RANDOM % 30000))
  # a lost datagram stalls a handshake instead of failing it: 30 s and the
  # test is killed (perl's alarm -- no `timeout` on macOS)
  out=$(SCALY_POISON=1 perl -e 'alarm shift; exec @ARGV' 30 "$bin" "$TMP/cert.pem" "$TMP/key.pem" "$port" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "h3: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
