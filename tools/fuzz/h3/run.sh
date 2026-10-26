#!/bin/bash
# tools/fuzz/h3/run.sh [seconds 600] [seed 1] -- h3_fuzz.scaly, built with
# this tree's packages, under SCALY_POISON=1, once without the QPACK dynamic
# table (as the HttpArena entry serves) and once with it, side by side on two
# UDP ports. OUT (default ./fuzz-h3-out) receives per run its stdout, the
# lines of its stderr that are not case markers -- each with the case before
# it, which made it -- and the exit code. A run that ends with anything but
# 0, or whose stderr holds such a line, is a finding: replay its case with
#   h3_fuzz <cert> <key> <port> 0 <seed> [table] --case <k>
# Needs ngtcp2 with crypto_ossl and OpenSSL 3.5 (as tests/h3/run.sh).
# H3_FUZZ_BIN names a fuzzer built already (linux.sh's image) and OPENSSL
# the openssl to make the certificate with; without them it is built here.
set -eu
SECONDS_RUN="${1:-600}"
SEED="${2:-1}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../../.." && pwd)"
OUT="$(mkdir -p "${OUT:-fuzz-h3-out}" && cd "${OUT:-fuzz-h3-out}" && pwd)"
if [ -z "${OPENSSL:-}" ]; then
  OPENSSL=openssl
  if command -v brew > /dev/null 2>&1 && p=$(brew --prefix openssl@3 2> /dev/null); then OPENSSL="$p/bin/openssl"; fi
fi
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
if [ -n "${H3_FUZZ_BIN:-}" ]; then
  cp "$H3_FUZZ_BIN" "$TMP/h3_fuzz"
else
  # from the tree: its packages are found under ./packages
  SCALY=$("$TREE/tools/scaly-of.sh" "$TREE/scalyc/build/scalyc")
  (cd "$TREE" && "$SCALY" build tools/fuzz/h3/h3_fuzz.scaly -o "$TMP/h3_fuzz")
fi
# a certificate already in OUT is taken (linux.sh makes one on the host: the
# image's static openssl has no configuration for req)
if [ -f "$OUT/cert.pem" ] && [ -f "$OUT/key.pem" ]; then
  cp "$OUT/cert.pem" "$OUT/key.pem" "$TMP/"
else
  "$OPENSSL" req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=localhost \
    -keyout "$TMP/key.pem" -out "$TMP/cert.pem" > /dev/null 2>&1
  cp "$TMP/cert.pem" "$TMP/key.pem" "$OUT/"
fi
base=$((20000 + RANDOM % 20000))
run() {
  local name="$1" port="$2"; shift 2
  local rc=0
  # an alarm well past the run's own end: a hang the kept connection does
  # not see still ends
  SCALY_POISON=1 perl -e 'alarm shift; exec @ARGV' $((SECONDS_RUN + 120)) \
    "$TMP/h3_fuzz" "$TMP/cert.pem" "$TMP/key.pem" "$port" "$SECONDS_RUN" "$SEED" "$@" \
    > "$OUT/$name.out" 2> "$TMP/$name.err" || rc=$?
  echo "$rc" > "$OUT/$name.rc"
  # every line that is not a case marker, after the marker of its case
  awk '/^case [0-9]+$/ { last = $0; next } { print last ": " $0 }' "$TMP/$name.err" > "$OUT/$name.findings"
  tail -1 "$TMP/$name.err" > "$OUT/$name.last"
}
run plain "$base" &
run table $((base + 1)) table &
wait
for name in plain table; do
  echo "$name: rc $(cat "$OUT/$name.rc"), $(wc -l < "$OUT/$name.findings" | tr -d ' ') stderr lines beyond the cases, last $(cat "$OUT/$name.last")"
  cat "$OUT/$name.out"
done
