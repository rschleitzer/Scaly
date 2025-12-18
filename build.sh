#!/bin/bash
set -e

# Regenerate from spec (skip for now - codegen needs updating)
# if [ -f ./mkp ]; then
#     ./mkp
# fi

# Build compiler
cd scalyc
mkdir -p build
cd build
cmake ..
make

# Optionally run tests
if [ "$1" = "test" ]; then
    ./scalyc --test
fi
