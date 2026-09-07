#!/bin/bash
# Stage-6 milestone 6.0: the Metal GPU matmul spike. Compiles the
# mgpu.m shim (tools/mgpu.sh), the bench program at -O2, links the
# Metal frameworks and runs the measurement (CPU tiled vs MPS vs MSL
# 4x4 shader, per-dispatch vs batched, incl. transfer). Metal exists
# only on macOS: on any other platform this prints SKIP and exits 0
# (CI-safe); a Mac without a Metal device SKIPs inside the program.
#
# Usage: tests/tensor/bench/run_gpu.sh [compiler]  (default scalyc/build/scalyc)
set -e
cd "$(dirname "$0")/../../.."

if [ "$(uname -s)" != "Darwin" ]; then
  echo "SKIP: Metal requires macOS"
  exit 0
fi

SCALYC=${1:-scalyc/build/scalyc}
if [ ! -x "$SCALYC" ]; then
  echo "error: $SCALYC not found - run ./build.sh first" >&2
  exit 1
fi

# -O2 runtime archive (the CLAUDE.md recipe; NEVER -c -O2 a library).
if [ ! -f /tmp/libscaly.a ] || find packages/scaly -name '*.scaly' -newer /tmp/libscaly.a | grep -q .; then
  echo "building -O2 runtime archive..."
  source tools/llvm-env.sh
  "$SCALYC" -S --no-prelude --no-tests -o /tmp/libscaly.ll packages/scaly/0.1.0/scaly.scaly
  sed 's/^define linkonce_odr /define weak_odr /' /tmp/libscaly.ll > /tmp/libscaly_weak.ll
  opt -O2 /tmp/libscaly_weak.ll -o /tmp/libscaly_opt.bc
  llc -relocation-model=pic -O2 -filetype=obj /tmp/libscaly_opt.bc -o /tmp/libscaly.o
  tools/fcontext.sh /tmp/fcontext.o
  tools/eio.sh /tmp/eio.o
  tools/ctime.sh /tmp/ctime.o
  tools/panic.sh /tmp/panic.o
  ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o /tmp/ctime.o /tmp/panic.o
fi

tools/mgpu.sh /tmp/mgpu.o
"$SCALYC" -O2 -c -o /tmp/gpu_matmul.o tests/tensor/bench/gpu_matmul.scaly
${CLANG:-clang} /tmp/gpu_matmul.o /tmp/mgpu.o /tmp/libscaly.a \
  -framework Metal -framework MetalPerformanceShaders -framework Foundation \
  -o /tmp/gpu_matmul
/tmp/gpu_matmul
