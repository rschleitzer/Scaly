#!/bin/bash
# tools/wasm/build-llvm.sh [dir] -- libLLVM (WebAssembly backend only) and
# lld's wasm linker as static libraries for wasm32-wasip1: what the compiler
# built for WebAssembly links (the REPL in the page). Into DIR (default
# ~/scaly-wasm), once; the result is DIR/wasm-build/lib/*.a and the headers
# under DIR/llvm-project-<v>.src and DIR/wasm-build/include.
#
#   1. llvm-project at the version the compiler binds (its C API), llvm/ and
#      lld/ only
#   2. YoWASP's patch for building LLVM for WASI (the `+wasm` branch of
#      github.com/YoWASP/llvm-project for the 21.x line: `__wasi__`
#      conditionals in Support, cmake, lld/Common) -- upstream it is an open
#      RFC -- and one fix of ours: CrashRecoveryContext.cpp calls abort()
#      without <cstdlib>, which a newer wasi-sdk's libc++ no longer brings
#   3. a native llvm-tblgen
#   4. the libraries, MinSizeRel, no threads, static (~20 min on 8 cores)
#
# Needs a wasi-sdk ($WASI_SDK, else ~/opt/wasi-sdk-*), cmake, curl, git.
set -eu
V=21.1.8
PATCH_BASE=llvmorg-21.1.4
DIR="${1:-$HOME/scaly-wasm}"
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
[ -n "$W" ] && [ -x "$W/bin/clang" ] || { echo "build-llvm: no wasi-sdk (set WASI_SDK)"; exit 1; }
mkdir -p "$DIR"
cd "$DIR"
SRC="llvm-project-$V.src"
if [ ! -d "$SRC/llvm" ]; then
  curl -fsSL -o "$SRC.tar.xz" "https://github.com/llvm/llvm-project/releases/download/llvmorg-$V/$SRC.tar.xz"
  tar -xJf "$SRC.tar.xz" "$SRC/llvm" "$SRC/lld" "$SRC/cmake" "$SRC/third-party" "$SRC/libunwind/include"
  curl -fsSL -o yowasp.diff "https://github.com/YoWASP/llvm-project/compare/$PATCH_BASE...$PATCH_BASE+wasm.diff"
  (cd "$SRC" && git apply --include='llvm/*' --include='lld/*' ../yowasp.diff)
  f="$SRC/llvm/lib/Support/CrashRecoveryContext.cpp"
  grep -q '#include <cstdlib>' "$f" || sed -i.bak 's|^#include <cassert>$|#include <cassert>\n#include <cstdlib>|' "$f"
fi
if [ ! -x tblgen-build/bin/llvm-tblgen ]; then
  cmake -S "$SRC/llvm" -B tblgen-build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_TARGETS_TO_BUILD=WebAssembly -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF \
    -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -DLLVM_ENABLE_ZSTD=OFF -DLLVM_ENABLE_ZLIB=OFF \
    -DLLVM_ENABLE_LIBXML2=OFF > tblgen-config.log 2>&1
  cmake --build tblgen-build --target llvm-tblgen llvm-min-tblgen -j 8 > tblgen-build.log 2>&1
fi
cmake -S "$SRC/llvm" -B wasm-build -G "Unix Makefiles" \
  -DCMAKE_TOOLCHAIN_FILE="$W/share/cmake/wasi-sdk-p1.cmake" -DWASI_SDK_PREFIX="$W" \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DLLVM_NATIVE_TOOL_DIR="$PWD/tblgen-build/bin" -DLLVM_TABLEGEN="$PWD/tblgen-build/bin/llvm-tblgen" \
  -DLLVM_ENABLE_ASSERTIONS=OFF -DBUILD_SHARED_LIBS=OFF -DLLVM_ENABLE_PIC=OFF -DLLVM_BUILD_STATIC=ON \
  -DLLVM_ENABLE_THREADS=OFF -DLLVM_TARGETS_TO_BUILD=WebAssembly \
  -DLLVM_HOST_TRIPLE=wasm32-wasip1 -DLLVM_DEFAULT_TARGET_TRIPLE=wasm32-wasip1 \
  -DLLVM_ENABLE_PROJECTS=lld \
  -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF \
  -DLLVM_INCLUDE_DOCS=OFF -DLLVM_INCLUDE_UTILS=OFF -DLLVM_BUILD_TOOLS=OFF \
  -DLLVM_ENABLE_ZSTD=OFF -DLLVM_ENABLE_ZLIB=OFF -DLLVM_ENABLE_LIBXML2=OFF \
  -DLLVM_ENABLE_LIBEDIT=OFF -DLLVM_ENABLE_LIBPFM=OFF \
  -DCMAKE_C_FLAGS="-D_WASI_EMULATED_MMAN" -DCMAKE_CXX_FLAGS="-D_WASI_EMULATED_MMAN" \
  -DCMAKE_EXE_LINKER_FLAGS="-lwasi-emulated-mman" > wasm-config.log 2>&1
cmake --build wasm-build --target llvm-libraries lldWasm -j 8 > wasm-build.log 2>&1
ls wasm-build/lib/*.a | wc -l | awk '{print "build-llvm: " $1 " archives in '"$DIR"'/wasm-build/lib"}'
