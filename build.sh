#!/bin/bash
set -e

# Regenerate from spec
if [ -f ./mkp ]; then
    ./mkp
fi

# Build compiler
cd scalyc
mkdir -p build
cd build

# Find LLVM (macOS homebrew or Linux system)
if [ -z "$LLVM_DIR" ]; then
    if [ -d "/opt/homebrew/opt/llvm@18/lib/cmake/llvm" ]; then
        LLVM_DIR="/opt/homebrew/opt/llvm@18/lib/cmake/llvm"
    elif [ -d "/usr/lib/llvm-18/lib/cmake/llvm" ]; then
        LLVM_DIR="/usr/lib/llvm-18/lib/cmake/llvm"
    fi
fi

# Optimize the C++ stage-0 by default (RelWithDebInfo = -O2 -g -DNDEBUG).
# An unoptimized (-O0) stage-0 compiles the scalyc package ~3x slower — the
# bootstrap's stage-0 -> stage1 step takes ~6.9s vs ~2.5s — and self-hosting
# work rarely touches the C++ stage-0, so the one-time optimize cost pays
# back on every bootstrap. Override with CMAKE_BUILD_TYPE=Debug to step
# through the C++ compiler itself.
BUILD_TYPE="${CMAKE_BUILD_TYPE:-RelWithDebInfo}"
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
if [ -n "$LLVM_DIR" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DLLVM_DIR=$LLVM_DIR"
fi

# Use Ninja if available (faster), otherwise Make
if command -v ninja &> /dev/null; then
    cmake -G Ninja $CMAKE_ARGS ..
    ninja
else
    cmake $CMAKE_ARGS ..
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

# Optionally run tests
if [ "$1" = "test" ]; then
    ./scalyc --test
fi
