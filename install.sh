#!/bin/bash
# install.sh - Build and install the Scaly compiler
#
# Prerequisites:
#   - LLVM (development libraries)
#   - Clang (C++ compiler)
#   - CMake
#
# This script will:
#   1. Build the scalyc compiler from C++ source
#   2. Install scalyc to /usr/local/bin
#   3. Install the standard library to /usr/local/share/scaly/packages

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "Scaly Compiler Installation"
echo "==========================="
echo ""

# Check prerequisites
echo "Checking prerequisites..."

if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: cmake not found.${NC}"
    echo ""
    echo "On macOS:  brew install cmake"
    echo "On Ubuntu: sudo apt install cmake"
    exit 1
fi

if ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: clang++ not found.${NC}"
    echo ""
    echo "On macOS:  brew install llvm"
    echo "On Ubuntu: sudo apt install clang"
    exit 1
fi

# Detect LLVM
LLVM_CONFIG=""
for cfg in llvm-config llvm-config-18 llvm-config-17; do
    if command -v $cfg &> /dev/null; then
        LLVM_CONFIG=$cfg
        break
    fi
done

if [ -z "$LLVM_CONFIG" ]; then
    echo -e "${RED}Error: llvm-config not found. Please install LLVM development libraries.${NC}"
    echo ""
    echo "On macOS:  brew install llvm"
    echo "On Ubuntu: sudo apt install llvm-18-dev"
    exit 1
fi

LLVM_VERSION=$($LLVM_CONFIG --version | cut -d. -f1)
echo -e "${GREEN}Found LLVM version: $LLVM_VERSION${NC}"

# Build the compiler
echo ""
echo "Building scalyc..."
cd scalyc
mkdir -p build
cd build
cmake .. > /dev/null
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo -e "${GREEN}Build successful!${NC}"

# Test the build
echo ""
echo "Running tests..."
if ./scalyc --test 2>/dev/null; then
    echo -e "${GREEN}All tests passed!${NC}"
else
    echo -e "${YELLOW}Warning: Some tests failed. Installation will continue.${NC}"
fi

# Install
echo ""
echo "Installing..."

INSTALL_BIN="/usr/local/bin"
INSTALL_SHARE="/usr/local/share/scaly/packages"

# Check if we need sudo
NEED_SUDO=""
if [ ! -w "$INSTALL_BIN" ] || [ ! -w "$(dirname $INSTALL_SHARE)" ]; then
    NEED_SUDO="sudo"
    echo "Installation requires sudo access."
fi

# Install binary
echo "Installing scalyc to $INSTALL_BIN..."
$NEED_SUDO install -m 755 scalyc "$INSTALL_BIN/scalyc"

# Install standard library
echo "Installing standard library to $INSTALL_SHARE..."
$NEED_SUDO mkdir -p "$INSTALL_SHARE"
$NEED_SUDO cp -r ../../packages/scaly "$INSTALL_SHARE/"

# Go back to project root
cd ../..

echo ""
echo -e "${GREEN}Installation complete!${NC}"
echo ""
echo "You can now compile Scaly programs:"
echo "  scalyc hello.scaly -o hello"
echo "  ./hello"
echo ""
echo "Try the included hello world example:"
echo "  scalyc hello.scaly -o hello && ./hello"
