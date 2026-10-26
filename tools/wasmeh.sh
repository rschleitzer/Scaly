#!/bin/bash
# Compile the compiler's one C++ file, compiler/wasm_eh.cpp, into an object
# (default /tmp/wasm_eh.o): it sets the exception model of a WebAssembly target
# machine, which LLVM 21's C API cannot say (the file's header has the account).
# Every link of scalyc, scaly and scalyls lists it next to panic.o -- all three
# hold the emitter -- and nothing else does: it is not part of libscaly.a, a
# program never needs it.
#
# It needs LLVM's C++ headers (`$LLVM_PREFIX/include/llvm`), which a
# development install of LLVM 21 carries (Homebrew's llvm@21, Ubuntu's
# llvm-21-dev); it names no symbol of the C++ library, so it links wherever the
# C API does. On Windows the standalone installer carries no C++ headers: they
# come with the static libraries ($SCALY_STATIC_LLVM_DIR, the release workflow
# and tests/win32/static-llvm.sh unpack its include/ too).
#
# Usage: tools/wasmeh.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/wasm_eh.o}
# shellcheck disable=SC1091
source tools/llvm-env.sh > /dev/null
INC="$LLVM_PREFIX/include"
if [ ! -f "$INC/llvm/Target/TargetMachine.h" ] && [ -n "${SCALY_STATIC_LLVM_DIR:-}" ]; then
    INC="$SCALY_STATIC_LLVM_DIR/include"
fi
if [ ! -f "$INC/llvm/Target/TargetMachine.h" ]; then
    echo "$(basename "$0"): FAIL — LLVM's C++ headers are not under $INC (a development install of LLVM $LLVM_MAJOR)" >&2
    exit 1
fi
# as tools/panic.sh: SCALY_ARCH cross-builds for another arch on Darwin
ARCHFLAG=""
if [ -n "${SCALY_ARCH:-}" ]; then
    if [ "$(uname -s)" != "Darwin" ]; then
        echo "$(basename "$0"): FAIL — SCALY_ARCH is Darwin-only (universal SDK); host is $(uname -s)" >&2
        exit 1
    fi
    ARCHFLAG="-arch $SCALY_ARCH"
fi

${CLANGXX:-clang++} $ARCHFLAG -std=c++17 -O2 -fno-rtti -fno-exceptions -I"$INC" \
    -c packages/scalyc/0.2.1/scalyc/compiler/wasm_eh.cpp -o "$OUT"
