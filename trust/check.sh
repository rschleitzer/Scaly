#!/bin/bash
# The check: the compiler built from this C++ emits the committed seed, byte
# for byte.  trust/check.sh [out-dir]
#
# Builds the compiler with trust/build.sh, writes the packages' interfaces
# into a cache of its own (nothing another compiler wrote is read), emits the
# seven files of seed/ exactly as tools/seed.sh does, and compares each with
# the committed one. The environment of build.sh applies (CXX, CC, LLVM21).
#
#   out-dir   default dist/trust
set -e
cd "$(dirname "$0")/.."
OUT=${1:-dist/trust}
trust/build.sh "$OUT"
S="$OUT/scalyc"

# the committed seed is the one its manifest names
SUM="shasum -a 256"; command -v sha256sum > /dev/null && SUM=sha256sum
( cd seed && $SUM -c SHA256SUMS > /dev/null ) || { echo "trust/check.sh: FAIL — seed/ does not match seed/SHA256SUMS"; exit 1; }

export SCALY_CACHE="$OUT/cache"
mkdir -p "$SCALY_CACHE" "$OUT/seed"
tools/interfaces.sh "$S" > "$OUT/interfaces.log" 2>&1 || { tail -5 "$OUT/interfaces.log"; echo "trust/check.sh: FAIL — the packages' interfaces"; exit 1; }

SEED_TARGET=${SEED_TARGET:-arm64-apple-darwin}
C=packages/scalyc/0.2.1 L=packages/scalyls/0.2.1
emit() { ( ulimit -s 65520; "$S" -S --no-tests --portable-simd --target "$SEED_TARGET" -o "$OUT/seed/$1.ll" "$2" ); }
pids=()
for a in main=$C/main.scaly scaly_main=$C/scaly_main.scaly scalyc=$C/scalyc.scaly scaly=packages/scaly/0.2.1/scaly.scaly; do
  emit "${a%%=*}" "${a#*=}" & pids+=($!)
done
for a in scalyls_main=$L/main.scaly scalyls=$L/scalyls.scaly json=packages/json/0.2.0/json.scaly; do
  SCALY_REQUIRE_INTERFACES=1 emit "${a%%=*}" "${a#*=}" & pids+=($!)
done
rc=0; for p in "${pids[@]}"; do wait "$p" || rc=1; done
[ $rc = 0 ] || { echo "trust/check.sh: FAIL — an emission failed"; exit 1; }

differ=""
for f in main scaly_main scalyc scaly scalyls_main scalyls json; do
  cmp -s "$OUT/seed/$f.ll" "seed/$f.ll" || differ="$differ $f.ll"
done
if [ -n "$differ" ]; then
  echo "trust/check.sh: FAIL — differs from seed/:$differ"
  exit 1
fi
echo "trust/check.sh: OK — the compiler built from trust/*.cpp emits all seven files of seed/ byte for byte"
