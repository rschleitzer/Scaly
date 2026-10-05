#!/bin/bash
# Stage-5 demo: train a char transformer on Thomas
# Mann's "Der Tod in Venedig" and print generated text — pure Scaly,
# zero Python. Training and inference each finish well under a
# minute on an Apple-silicon Mac.
#
# Pins the cwd to the repo root: the corpus (tests/tensor/mann.txt)
# loads cwd-relative, and the compiler resolves packages/ + the
# runtime archive relative to the cwd as well.
#
# Usage: demo/run.sh [compiler]   (default scalyc/build/scalyc)
set -e
cd "$(dirname "$0")/.."

SCALYC=${1:-scalyc/build/scalyc}
if [ ! -x "$SCALYC" ]; then
  echo "error: $SCALYC not found - run ./build.sh first" >&2
  exit 1
fi

# The tape kernels run at the ARCHIVE's opt level, not the program's:
# build /tmp/libscaly.a at -O2 (emit IR, promote linkonce_odr to
# weak_odr, then opt/llc — never -c -O2 a library: GlobalDCE would
# delete every body).
if [ ! -f /tmp/libscaly.a ] || find packages/scaly -newer /tmp/libscaly.a | grep -q .; then
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

BIN=/tmp/scaly_mann_demo
rm -f "$BIN"
"$SCALYC" -O2 -o "$BIN" demo/mann.scaly
exec "$BIN"
