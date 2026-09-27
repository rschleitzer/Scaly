#!/bin/bash
# tools/bench/build.sh [scalyc] — build both sides of the comparison into
# BENCH_WORK/bin (default /tmp/scaly-bench/bin); run tools/bench/fetch.sh first.
#
# Scaly: every program of tests/selfscale/ that has a C counterpart, at -O2,
# twice — for the generic CPU (<name>) and for this machine (<name>_native,
# -mcpu=native). On x86-64 that pair is exactly the AVX question: generic is
# SSE2, an f64x4 two xmm operations; native with AVX2 one ymm operation.
#
# C: the fastest programs of the morning's comparison (memory
# benchmarks-game-c-race), clang -O3 with the host CPU (-march=native on
# x86-64, -mcpu=native on arm64 plus the sse2neon shim), OpenMP through
# libomp. A program that fails to build is reported and skipped by race.py.
#
# The Windows box (Git Bash, clang with the MSVC runtime; tests/platform.sh
# tells it apart): binaries carry .exe, a Scaly program links against
# /tmp/libscaly.lib (tools/win-archive.sh builds it), the C programs take
# shim-win/ for the three POSIX headers they include (unistd.h, malloc.h,
# sched.h) and libomp from the LLVM install, there is no -lm, and the two
# programs written on pthreads (fannkuch #6, binary-trees #5) are not built.
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
. tools/llvm-env.sh >/dev/null 2>&1 || true
X=$SCALY_EXE
SCALYC=${1:-scalyc/build/scalyc$X}
W=${BENCH_WORK:-/tmp/scaly-bench}
B=$W/bin
mkdir -p "$B"
[ -d "$W/src" ] || { echo "build: no sources in $W/src - run tools/bench/fetch.sh first"; exit 1; }

# ---- Scaly
if scaly_need_archive bench "$SCALYC"; then
  for p in mandelbrot mandelbrot_simd spectralnorm spectralnorm_simd nbody fannkuch binarytrees; do
    if "$SCALYC" -O2 -o "$B/$p$X" "tests/selfscale/$p.scaly" > "$B/$p.log" 2>&1 \
       && "$SCALYC" -O2 -mcpu=native -o "$B/${p}_native$X" "tests/selfscale/$p.scaly" >> "$B/$p.log" 2>&1; then
      echo "ok   scaly $p"
    else
      echo "FAIL scaly $p: $(grep -m2 -i error "$B/$p.log")"
    fi
  done
fi

# ---- C
CC=${CC:-}
if [ -z "$CC" ]; then
  for cand in "${LLVM_PREFIX:-/nonexistent}/bin/clang" clang-20 clang; do
    if command -v "$cand" >/dev/null 2>&1; then CC=$cand; break; fi
  done
fi
ARCH=$(uname -m)
if [ "$ARCH" = arm64 ] || [ "$ARCH" = aarch64 ]; then
  CPU=(-mcpu=native -I"$W/shim-arm64")
else
  CPU=(-march=native)
fi
OS=$(uname -s)
SHIM=()
COMPAT=()
LIBM=(-lm)
OMP=(-fopenmp)
if [ "$SCALY_COFF" = 1 ]; then
  SHIM=(-Itools/bench/shim-win)
  LIBM=()
  # libomp.lib ships beside clang in the LLVM install; LIB does not name it
  OMP=(-fopenmp -L"$(dirname "$(command -v "$CC")")/../lib")
elif [ "$OS" = Darwin ]; then
  SHIM=(-Itools/bench/shim)
  COMPAT=(-include tools/bench/shim/linuxcompat.h)
  OMPP=$(brew --prefix libomp 2>/dev/null)
  if [ -n "$OMPP" ] && [ -d "$OMPP/lib" ]; then
    OMP=(-fopenmp -I"$OMPP/include" -L"$OMPP/lib" -lomp)
  else
    echo "build: no libomp (brew install libomp) - the OpenMP programs will fail"
  fi
fi
echo "build: C with $CC ${CPU[*]}"
build() {
  local name=$1 src=$2; shift 2
  if "$CC" -O3 "${CPU[@]}" "${SHIM[@]}" "${OMP[@]}" "$@" -o "$B/$name$X" -x c "$W/src/$src" -x none "${LIBM[@]}" > "$B/$name.log" 2>&1; then
    echo "ok   C $name"
  else
    echo "FAIL C $name: $(grep -m2 error "$B/$name.log")"
  fi
}
posix_threads() {
  if [ "$SCALY_COFF" = 1 ]; then
    rm -f "$B/$1$X"
    echo "skip C $1: written on pthreads, which the MSVC runtime does not have"
    return 1
  fi
  return 0
}
build mandel6    mandelbrot/mandelbrot.gcc-6.gcc -fgnu89-inline
build mandel5    mandelbrot/mandelbrot.gcc-5.gcc
build nbody4     nbody/nbody.gcc-4.gcc
build nbody6     nbody/nbody.gcc-6.gcc
build spectral5  spectralnorm/spectralnorm.gcc-5.gcc
build spectral4  spectralnorm/spectralnorm.gcc-4.gcc "${COMPAT[@]}"
posix_threads fannkuch6 && build fannkuch6 fannkuchredux/fannkuchredux.gcc-6.gcc -lpthread
build fannkuch5  fannkuchredux/fannkuchredux.gcc-5.gcc
posix_threads btree5 && build btree5 binarytrees/binarytrees.gcc-5.gcc -lpthread
