#!/bin/bash
# Compile the Metal GPU matmul shim (stage 6.0 spike) into an object
# file (default /tmp/mgpu.o). Metal exists only on macOS — callers must
# skip on other platforms (tests/tensor/bench/run_gpu.sh does). The
# object is NOT part of libscaly.a; programs using it link it plus
# -framework Metal -framework MetalPerformanceShaders -framework
# Foundation explicitly.
#
# Usage: tools/mgpu.sh [output.o]
set -e
cd "$(dirname "$0")/.."

if [ "$(uname -s)" != "Darwin" ]; then
  echo "error: Metal shim requires macOS" >&2
  exit 1
fi

OUT=${1:-/tmp/mgpu.o}
${CLANG:-clang} -fobjc-arc -O2 -c packages/scaly/0.1.1/scaly/tensor/mgpu.m -o "$OUT"
