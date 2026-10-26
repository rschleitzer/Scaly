#!/bin/bash
# tools/fuzz/h3/linux.sh [seconds 600] [seed 1] -- run.sh on Linux, in
# Docker: the HttpArena entry's build stage (scaly-fuzz-base: OpenSSL and
# ngtcp2 built as the entry builds them, the released toolchain) with the
# packages the fuzzer uses replaced by this tree's, h3_fuzz built there, and
# run.sh with both modes in a container (no port published; seccomp
# unconfined as HttpArena runs its entries). OUT (default ./fuzz-h3-linux)
# receives what run.sh keeps.
set -eu
SECONDS_RUN="${1:-600}"
SEED="${2:-1}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../../.." && pwd)"
OUT="$(mkdir -p "${OUT:-fuzz-h3-linux}" && cd "${OUT:-fuzz-h3-linux}" && pwd)"

docker build -q --target build -t scaly-fuzz-base "$TREE/tools/bench/httparena/scaly" > /dev/null
CTX="$(mktemp -d)"
trap 'rm -rf "$CTX"' EXIT
mkdir -p "$CTX/packages" "$CTX/fuzz/h3"
for p in scaly http json tls h3; do
  cp -R "$TREE/packages/$p/0.2.0" "$CTX/packages/$p"
done
cp "$HERE/h3_fuzz.scaly" "$HERE/run.sh" "$CTX/fuzz/h3/"
cat > "$CTX/Dockerfile" << 'EOF'
FROM scaly-fuzz-base
COPY packages/ /tmp/packages/
RUN P=/opt/scaly/toolchain/packages; \
    for p in /tmp/packages/*; do n=$(basename "$p"); rm -rf "$P/$n/0.2.0"; cp -R "$p" "$P/$n/0.2.0"; done; \
    rm -rf /tmp/packages
COPY fuzz/ /fuzz/
WORKDIR /fuzz/h3
RUN scaly build h3_fuzz.scaly -o /h3_fuzz
EOF
docker build -q -t scaly-fuzz-h3 "$CTX" > /dev/null
echo "linux.sh: image scaly-fuzz-h3 built from $(git -C "$TREE" rev-parse --short HEAD)$(git -C "$TREE" diff --quiet || echo '+changes')"
OPENSSL=openssl
if command -v brew > /dev/null 2>&1 && p=$(brew --prefix openssl@3 2> /dev/null); then OPENSSL="$p/bin/openssl"; fi
"$OPENSSL" req -x509 -newkey rsa:2048 -nodes -days 1 -subj /CN=localhost \
  -keyout "$OUT/key.pem" -out "$OUT/cert.pem" > /dev/null 2>&1
docker run --rm --security-opt seccomp=unconfined -v "$OUT:/out" \
  -e OUT=/out -e H3_FUZZ_BIN=/h3_fuzz \
  scaly-fuzz-h3 /fuzz/h3/run.sh "$SECONDS_RUN" "$SEED"
