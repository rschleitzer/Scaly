#!/bin/bash
# Build the compiler from the C++ in this directory: trust/build.sh [out-dir]
#
# Needs a C++20 compiler (CXX, default c++), a C compiler for the runtime's
# C and assembly files (CC, default cc), and LLVM 21: libLLVM to link and its
# C++ headers for compiler/wasm_eh.cpp, found as tools/llvm-env.sh finds them
# (LLVM21=/path overrides). No Scaly compiler, no seed, no clang required:
# with CXX=g++ CC=gcc nothing of LLVM's own toolchain takes part.
#
# The flags are part of the translation: Scaly's integer arithmetic wraps
# (-fwrapv), a record may be read through another type of the same layout
# (-fno-strict-aliasing), and a floating-point expression is evaluated as
# written (-ffp-contract=off).
#
#   out-dir   default dist/trust; the compiler is <out-dir>/scalyc
#   CXX_OPT   the optimization level of the C++ (default -O1)
set -e
cd "$(dirname "$0")/.."
OUT=${1:-dist/trust}
CXX=${CXX:-c++}
export CLANG=${CC:-cc}          # tools/fcontext.sh & co. compile with $CLANG
source tools/llvm-env.sh > /dev/null
[ "$llvm_env_ok" = "1" ] || { echo "trust/build.sh: FAIL — LLVM $LLVM_MAJOR not found"; exit 1; }
mkdir -p "$OUT"
FLAGS=(-std=c++20 -fwrapv -fno-strict-aliasing -ffp-contract=off -w ${CXX_OPT:--O1})

pids=()
for f in libscaly scalyc main; do
  $CXX "${FLAGS[@]}" -c -o "$OUT/$f.o" "trust/$f.cpp" & pids+=($!)
done
rc=0; for p in "${pids[@]}"; do wait "$p" || rc=1; done
[ $rc = 0 ] || { echo "trust/build.sh: FAIL — the C++ did not compile"; exit 1; }

# the runtime's files in C and assembly, as every build of the compiler has them
for s in fcontext eio ctime panic; do
  tools/$s.sh "$OUT/$s.o" > /dev/null || { echo "trust/build.sh: FAIL — tools/$s.sh"; exit 1; }
done
# WebAssembly's exception model, which LLVM's C API cannot set (one C++ file)
LLVM_CXXFLAGS=$("$LLVM_PREFIX/bin/llvm-config" --cxxflags | sed 's/-std=[^ ]*//')
# shellcheck disable=SC2086
$CXX -std=c++17 -O1 -w $LLVM_CXXFLAGS -c -o "$OUT/wasm_eh.o" packages/scalyc/0.2.1/scalyc/compiler/wasm_eh.cpp

LIBS=(-L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm)
[ "$(uname -s)" = Linux ] && LIBS+=(-ldl -lpthread)
$CXX -o "$OUT/scalyc" "$OUT/main.o" "$OUT/scalyc.o" "$OUT/libscaly.o" \
  "$OUT/fcontext.o" "$OUT/eio.o" "$OUT/ctime.o" "$OUT/panic.o" "$OUT/wasm_eh.o" "${LIBS[@]}" 2>&1 \
  | grep -v '^ld: warning' || true
[ -x "$OUT/scalyc" ] || { echo "trust/build.sh: FAIL — no $OUT/scalyc"; exit 1; }
echo "trust/build.sh: OK -> $OUT/scalyc ($($CXX --version | head -1))"
