#!/bin/bash
# Write the compiler as C++ into this directory: trust/emit.sh [compiler]
#
# The three roots tools/seed.sh links into the compiler -- the standard
# library, the compiler package and its entry -- each through `scalyc
# --emit-cpp` (the C++ emitter, packages/scalyc/<version>/scalyc/compiler/
# CppEmitter.scaly). The standard library's run also writes scaly_types.h,
# the records the other two files include. Every file here is generated: it
# changes with the sources, and tools/bar.sh writes it again whenever it
# refreshes the seed, so the C++ and the seed always stand for the same tree.
#
#   compiler   default /tmp/scalyc_stage2 (a compiler built from this tree)
#   dir        default trust
set -e
cd "$(dirname "$0")/.."
CC=${1:-/tmp/scalyc_stage2}
DIR=${2:-trust}
mkdir -p "$DIR"
C=packages/scalyc/0.2.1
"$CC" --no-prelude --no-tests --emit-cpp -o "$DIR/libscaly.cpp" packages/scaly/0.2.1/scaly.scaly
"$CC" --no-tests --emit-cpp -o "$DIR/scalyc.cpp" $C/scalyc.scaly
"$CC" --no-tests --emit-cpp -o "$DIR/main.cpp" $C/main.scaly
