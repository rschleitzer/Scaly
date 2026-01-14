#!/bin/bash
# publish.sh - Build and test the Scaly compiler for release
#
# This script is for maintainers to verify the build before tagging a release.
# Users should use install.sh instead.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "Scaly Compiler Release Build"
echo "============================="
echo ""

echo "Building compiler..."
cd scalyc
mkdir -p build
cd build
cmake .. > /dev/null
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Running full test suite..."
./scalyc --test --slow-tests

echo ""
echo "Testing AOT compilation..."
cd ../..
./scalyc/build/scalyc hello.scaly -o /tmp/hello_test
/tmp/hello_test
rm /tmp/hello_test

echo ""
echo "All checks passed!"
echo ""
echo "To publish a release:"
echo "  1. Update version in documentation if needed"
echo "  2. git tag v0.1.0"
echo "  3. git push origin v0.1.0"
echo "  4. Create release on GitHub with install instructions"
