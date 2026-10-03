#!/bin/bash
# Build the three programs for THIS system as the installer hands them out:
# scalyc, scaly and scalyls with LLVM linked in statically, and the runtime
# archive `scalyc -o` links against.
#
# Output: <outdir>/scaly-<version>-<system>-<machine>.tar.gz, the two names as
# `uname -s` (lower case) and `uname -m` give them — darwin-arm64,
# linux-aarch64, linux-x86_64 — which is how docs/website/install.sh asks for
# it. Payload: libexec/scalyc, libexec/scaly, libexec/scalyls, lib/libscaly.a.
# The packages and the seed are NOT in it: they are the same for every system
# and travel in tools/make-dist.sh's tarball, which the installer unpacks
# first. A system without such an archive is built from the seed there.
#
# The build is tools/build-from-seed.sh's, with SCALY_STATIC_LLVM=1 — the
# recipe every development build uses, so what is handed out is what the bar
# tests. Needs the static LLVM libraries beside the tools:
#   macOS    brew install llvm@21 zstd
#   Ubuntu   apt install llvm-21 llvm-21-dev lld-21 clang zlib1g-dev libzstd-dev libxml2-dev
# ★The archive runs on the system it was built on and newer ones: Homebrew's
# LLVM is built for the macOS it runs on (26, the minimum we promise), and a
# Linux build carries the glibc version of its host as its floor — build it on
# the OLDEST Ubuntu we support (26.04).
#
# Usage: tools/make-bindist.sh [version] [outdir]   (default 0.1.0 dist)
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-0.1.0}"
OUT="${2:-dist}"
SYSTEM="$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)"
TARBALL="$OUT/scaly-$VERSION-$SYSTEM.tar.gz"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/libexec" "$STAGE/lib"

SCALY_STATIC_LLVM=1 tools/build-from-seed.sh "$STAGE/libexec/scalyc" > "$STAGE/build.log" 2>&1 \
  || { tail -20 "$STAGE/build.log"; echo "make-bindist: FAIL — build-from-seed"; exit 1; }
for p in scalyc scaly scalyls; do
  [ -x "$STAGE/libexec/$p" ] || { echo "make-bindist: FAIL — $p was not built"; exit 1; }
done
cp /tmp/libscaly.a "$STAGE/lib/libscaly.a"
rm -f "$STAGE/build.log"

# Nothing of LLVM, and nothing of a package manager, may be left to load.
if [ "$(uname -s)" = "Darwin" ]; then
  deps=$(otool -L "$STAGE"/libexec/* | grep -v ':$' | grep -v '^[[:space:]]*/usr/lib/' || true)
else
  deps=$(ldd "$STAGE"/libexec/* | grep -iE 'llvm|zstd|xml2|libz\.|stdc\+\+' || true)
fi
[ -z "$deps" ] || { echo "make-bindist: FAIL — the programs still load:"; echo "$deps"; exit 1; }

mkdir -p "$OUT"
COPYFILE_DISABLE=1 tar --no-xattrs -czf "$TARBALL" -C "$STAGE" libexec lib 2>/dev/null \
  || COPYFILE_DISABLE=1 tar -czf "$TARBALL" -C "$STAGE" libexec lib

echo "make-bindist: OK -> $TARBALL ($(du -h "$TARBALL" | cut -f1))"
