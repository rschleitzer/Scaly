#!/bin/bash
# Install a freshly-minted seed into the committed seed/ tree.
#
# Run AFTER tools/seed.sh has produced + fixed-point-verified the seed in
# dist/seed/ (gitignored). This copies the three .ll into seed/ with a
# SHA256SUMS manifest, ready to commit.
#
# A single seed serves every 64-bit little-endian LP64 target (the IR is
# triple-less and bakes layout from fixed LP64 constants), so this overwrites
# the one seed/ rather than keeping a per-triple copy. Mint on any LP64-LE host
# — the emitted .ll is host-independent — but VERIFY on each target (run hello +
# the AOT corpus + a fixed-point re-emit) before trusting it there.
#
# Usage: tools/install-seed.sh [src-dir]
#   src-dir   default dist/seed (where tools/seed.sh writes)
set -e
cd "$(dirname "$0")/.."

SRC=${1:-dist/seed}
for f in main scalyc scaly; do
    [ -f "$SRC/$f.ll" ] || { echo "install-seed: FAIL — $SRC/$f.ll not found; run tools/seed.sh first"; exit 1; }
done

cp "$SRC/main.ll" "$SRC/scalyc.ll" "$SRC/scaly.ll" seed/
SUM_FILES="main.ll scalyc.ll scaly.ll"

# scalyls language server seed (optional — present when tools/seed.sh emitted
# it, self-hosted. A separate program with TWO roots (scalyls_main.ll +
# scalyls.ll) that links against the compiler seed objects; not part of the
# compiler fixed point).
if [ -f "$SRC/scalyls.ll" ]; then
    cp "$SRC/scalyls.ll" seed/
    SUM_FILES="$SUM_FILES scalyls.ll"
fi
if [ -f "$SRC/scalyls_main.ll" ]; then
    cp "$SRC/scalyls_main.ll" seed/
    SUM_FILES="$SUM_FILES scalyls_main.ll"
fi

# shellcheck disable=SC2086
( cd seed && shasum -a 256 $SUM_FILES > SHA256SUMS )

echo "install-seed: OK — seed/ updated ($SUM_FILES)"
echo "  git add seed && git commit -m \"Refresh seed\""
