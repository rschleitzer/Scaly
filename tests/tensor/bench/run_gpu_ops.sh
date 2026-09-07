#!/bin/bash
# Stage-6 milestone 6.1: GPU op parity suite. Compiles the mgpu.m shim
# and gpu_ops.scaly, runs the mini-transformer whole-step parity check
# (hybrid MPS+MSL and MSL-only variants) plus Adam. Metal exists only
# on macOS: prints SKIP and exits 0 elsewhere (CI-safe); a Mac without
# a Metal device SKIPs inside the program.
#
# Usage: tests/tensor/bench/run_gpu_ops.sh [compiler]  (default scalyc/build/scalyc)
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

# scalygpu package object (same recipe: non-generic package bodies come
# from a library object, and NEVER `-c -O2` a library).
if [ ! -f /tmp/libscalygpu.o ] || find packages/scalygpu -name '*.scaly' -newer /tmp/libscalygpu.o | grep -q . \
   || [ /tmp/libscaly.a -nt /tmp/libscalygpu.o ]; then
  echo "building -O2 scalygpu package object..."
  source tools/llvm-env.sh
  "$SCALYC" -S --no-tests -o /tmp/libscalygpu.ll packages/scalygpu/0.1.0/scalygpu.scaly
  sed 's/^define linkonce_odr /define weak_odr /' /tmp/libscalygpu.ll > /tmp/libscalygpu_weak.ll
  opt -O2 /tmp/libscalygpu_weak.ll -o /tmp/libscalygpu_opt.bc
  llc -relocation-model=pic -O2 -filetype=obj /tmp/libscalygpu_opt.bc -o /tmp/libscalygpu.o
fi

tools/mgpu.sh /tmp/mgpu.o
"$SCALYC" -O2 -c -o /tmp/gpu_ops.o tests/tensor/bench/gpu_ops.scaly
${CLANG:-clang} /tmp/gpu_ops.o /tmp/libscalygpu.o /tmp/mgpu.o /tmp/libscaly.a \
  -framework Metal -framework MetalPerformanceShaders -framework Foundation \
  -o /tmp/gpu_ops
/tmp/gpu_ops
