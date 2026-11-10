#!/bin/bash
# Compile the graphics-card host for THIS host into an object file (default
# /tmp/gpu.o) and print the object's path -- on macOS, where the host is
# Metal's (packages/scaly/0.2.1/scaly/gpu/gpu_darwin.c). Everywhere else there
# is no host yet: nothing is written and nothing printed, so a caller
# archives what this prints,
#
#     GPU_O=$(tools/gpu.sh /tmp/gpu.o)
#     ar rcs /tmp/libscaly.a ... $GPU_O
#
# and gets the same archive as before on a system without one.
#
# Only a program compiled with --gpu calls into it; the compiler itself does
# not, so no link of scalyc, scaly or scalyls lists the object. It names no
# framework at link time either (Metal is opened at the first kernel), so an
# archive that holds it links exactly as one that does not.
#
# Usage: tools/gpu.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/gpu.o}
rm -f "$OUT"
[ "$(uname -s)" = "Darwin" ] || exit 0

# SCALY_ARCH as in tools/panic.sh: another arch of the universal SDK.
ARCHFLAG=""
if [ -n "${SCALY_ARCH:-}" ]; then
    ARCHFLAG="-arch $SCALY_ARCH"
fi

${CLANG:-clang} $ARCHFLAG -O2 -c packages/scaly/0.2.1/scaly/gpu/gpu_darwin.c -o "$OUT"
echo "$OUT"
