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

CMAKE_ARGS=""
if [ -n "$LLVM_DIR" ]; then
    CMAKE_ARGS="-DLLVM_DIR=$LLVM_DIR"
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
