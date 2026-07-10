#!/bin/bash
# Stage-6 milestone 6.1: the BPE Thomas-Mann trainer on the GPU —
# the whole training step (forward + backward + Adam) encoded as ONE
# Metal command buffer per step, tape buffers living in unified-memory
# MTLBuffers. Metal exists only on macOS: prints SKIP and exits 0
# elsewhere (CI-safe); a Mac without a Metal device SKIPs inside the
# program. Reuses the CPU demo's corpus + BPE artifacts (runs
# demo/run_bpe.sh's steps 1+2 if they are missing).
#
# Env switches (see demo/mann_bpe_gpu.scaly):
#   MGPU_SYNC=1   per-op-sync matmuls (the launch-cost measurement)
#   MGPU_MSL=1    own MSL matmul kernels instead of MPS
#   MGPU_BIG=1    bigger model (D=512, T=256, F=2048, 4 layers)
#   MGPU_STEPS=n  step cap (smoke tests)
#
# Usage: demo/run_bpe_gpu.sh [compiler]   (default scalyc/build/scalyc)
set -e
cd "$(dirname "$0")/.."

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
  ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o
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

# corpus + BPE vocab/token stream (cached; the CPU demo's pipeline)
if [ ! -s demo/build/mann_full.txt ]; then
  demo/fetch_corpus.sh
fi
if [ ! -s demo/build/bpe_tokens.txt ] || [ demo/bpe.scaly -nt demo/build/bpe_tokens.txt ]; then
  echo "training BPE tokenizer..."
  BPE=/tmp/scaly_bpe; rm -f "$BPE"
  "$SCALYC" -O2 -o "$BPE" demo/bpe.scaly
  "$BPE"
fi

# train + generate on the GPU (binary cached against its sources so a
# checkpoint-driven showcase rerun starts instantly)
BIN=/tmp/scaly_mann_bpe_gpu
if [ ! -x "$BIN" ] \
   || [ demo/mann_bpe_gpu.scaly -nt "$BIN" ] \
   || [ packages/scaly/0.1.0/scaly/tensor/mgpu.m -nt "$BIN" ] \
   || [ /tmp/libscalygpu.o -nt "$BIN" ] \
   || [ /tmp/libscaly.a -nt "$BIN" ]; then
  tools/mgpu.sh /tmp/mgpu.o
  "$SCALYC" -O2 -c -o /tmp/mann_bpe_gpu.o demo/mann_bpe_gpu.scaly
  ${CLANG:-clang} /tmp/mann_bpe_gpu.o /tmp/libscalygpu.o /tmp/mgpu.o /tmp/libscaly.a \
    -framework Metal -framework MetalPerformanceShaders -framework Foundation \
    -o "$BIN"
fi
exec "$BIN"
