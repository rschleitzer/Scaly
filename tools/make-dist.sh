#!/bin/bash
# Assemble the public Scaly distribution tarball that scaly.io/install.sh fetches.
#
# Payload (everything the seed installer needs, NONE of the private scalyc
# sources): the seed (.ll = the compiler as IR), the scaly stdlib + prelude
# SOURCES (the front-end parses these to type-check user programs), LICENSE,
# and a VERSION stamp. Output: dist/scaly-<version>.tar.gz (gitignored scratch).
# tools/publish-install.sh uploads it to https://scaly.io/downloads/ (kept out
# of the website --delete sync so a routine docs deploy can't remove it).
#
# Usage: tools/make-dist.sh [version]   (default 0.1.0)
#
# Run tools/seed.sh + tools/install-seed.sh first so seed/ is current; this
# script packages whatever is committed under seed/.
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-0.1.0}"
OUT="dist"
TARBALL="$OUT/scaly-$VERSION.tar.gz"

for f in main scaly_main scalyc scaly; do
  [ -f "seed/$f.ll" ] || { echo "make-dist: FAIL — seed/$f.ll missing (run tools/seed.sh + install-seed.sh)"; exit 1; }
done
[ -f LICENSE ] || { echo "make-dist: FAIL — LICENSE missing"; exit 1; }
[ -d "packages/scaly/$VERSION" ] || { echo "make-dist: FAIL — packages/scaly/$VERSION missing"; exit 1; }

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

# seed/ (the compiler IR)
mkdir -p "$STAGE/seed"
cp seed/main.ll seed/scaly_main.ll seed/scalyc.ll seed/scaly.ll "$STAGE/seed/"

# scalyls language server seed (optional — a separate program with two roots;
# install.sh links them with the compiler seed objects into
# <prefix>/bin/scalyls when both are present).
[ -f seed/scalyls.ll ] && cp seed/scalyls.ll "$STAGE/seed/"
[ -f seed/scalyls_main.ll ] && cp seed/scalyls_main.ll "$STAGE/seed/"
[ -f seed/json.ll ] && cp seed/json.ll "$STAGE/seed/"

# scaly stdlib + prelude SOURCES (parsed by the front-end; no scalyc sources)
mkdir -p "$STAGE/packages/scaly"
cp -R "packages/scaly/$VERSION" "$STAGE/packages/scaly/$VERSION"

cp LICENSE "$STAGE/LICENSE"
printf '%s\n' "$VERSION" > "$STAGE/VERSION"

# Drop editor/OS cruft so the tarball is reproducible-ish.
find "$STAGE" -name '.DS_Store' -delete 2>/dev/null || true

mkdir -p "$OUT"
# Deterministic-ish: sort entries, no owner/timestamp noise.
tar --no-xattrs -czf "$TARBALL" -C "$STAGE" . 2>/dev/null \
  || tar -czf "$TARBALL" -C "$STAGE" .

SIZE=$(du -h "$TARBALL" | cut -f1)
echo "make-dist: OK -> $TARBALL ($SIZE)"
echo "  contents:"
tar -tzf "$TARBALL" | sed 's/^/    /' | head -12
echo "    ..."
echo "  publish with: tools/publish-install.sh $VERSION"
