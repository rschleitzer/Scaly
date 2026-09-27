#!/bin/bash
# tools/bench/fetch.sh — the C side of the benchmarks-game comparison.
#
# Downloads the benchmarks game's source archive into the work directory
# (BENCH_WORK, default /tmp/scaly-bench) and unpacks it. On arm64 it also
# fetches sse2neon.h and writes the <xmmintrin.h>-family wrappers around it:
# the fastest C programs are written with x86 SSE intrinsics, and sse2neon
# translates them to NEON. On x86-64 nothing of that is needed.
#
# Nothing third-party is kept in the tree; this script is the recipe.
set -e
cd "$(dirname "$0")/../.."
. tests/platform.sh
W=${BENCH_WORK:-/tmp/scaly-bench}
mkdir -p "$W"
if [ ! -f "$W/src.zip" ]; then
  curl -fL -o "$W/src.zip" https://benchmarksgame-team.pages.debian.net/benchmarksgame/download/benchmarksgame-sourcecode.zip
fi
rm -rf "$W/src"
mkdir -p "$W/src"
if command -v unzip >/dev/null 2>&1; then
  unzip -q "$W/src.zip" -d "$W/src"
else
  # Git Bash may come without unzip; its python (tools/win-env.sh) unpacks the
  # archive, handed the Windows spelling of the paths
  zp=$W/src.zip; dp=$W/src
  if [ "$SCALY_COFF" = 1 ]; then zp=$(cygpath -w "$zp"); dp=$(cygpath -w "$dp"); fi
  python3 -c 'import sys, zipfile; zipfile.ZipFile(sys.argv[1]).extractall(sys.argv[2])' "$zp" "$dp"
fi
echo "fetch: sources in $W/src"

if [ "$(uname -m)" = arm64 ] || [ "$(uname -m)" = aarch64 ]; then
  mkdir -p "$W/shim-arm64"
  if [ ! -f "$W/shim-arm64/sse2neon.h" ]; then
    curl -fL -o "$W/shim-arm64/sse2neon.h" https://raw.githubusercontent.com/DLTcollab/sse2neon/master/sse2neon.h
  fi
  for h in emmintrin immintrin pmmintrin smmintrin tmmintrin x86intrin xmmintrin; do
    echo '#include "sse2neon.h"' > "$W/shim-arm64/$h.h"
  done
  echo "fetch: sse2neon shim in $W/shim-arm64"
fi
