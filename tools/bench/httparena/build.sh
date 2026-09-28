#!/bin/bash
# tools/bench/httparena/build.sh [image] — build the Scaly entry for HttpArena
# (default image name httparena-scaly, the name HttpArena's scripts expect for
# a framework directory called scaly).
#
# The Docker build context is staged from the WORKING TREE, not the whole
# repository (the test corpora alone are gigabytes): the seed, the tools the
# seed build calls, the scaly/scalyc/http packages and the arena program. To
# run it under HttpArena's own scripts, put a frameworks/scaly/ directory in an
# HttpArena checkout holding meta.json and a build.sh that calls this one:
#
#   frameworks/scaly/build.sh:   exec /path/to/Scaly/tools/bench/httparena/build.sh
#   scripts/validate.sh scaly
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../../.." && pwd)"
IMAGE=${1:-httparena-scaly}
CTX="$(mktemp -d)"
trap 'rm -rf "$CTX"' EXIT
cd "$ROOT"
mkdir -p "$CTX/packages" "$CTX/tools/bench/http"
cp -R seed "$CTX/"
# the tools directory without the benchmark programs and their build output
( cd tools && find . -maxdepth 1 -type f -exec cp {} "$CTX/tools/" \; )
cp -R tools/lldb "$CTX/tools/" 2>/dev/null || true
cp -R packages/scaly packages/scalyc packages/http "$CTX/packages/"
cp tools/bench/http/arena.scaly "$CTX/tools/bench/http/"
rm -f "$CTX"/seed/r_*.ll
docker build -t "$IMAGE" -f "$HERE/Dockerfile" "$CTX"
