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
cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Optionally run tests
if [ "$1" = "test" ]; then
    ./scalyc --test
fi
