#!/bin/bash
# tools/wasm/repl.sh [build] -- the REPL of the compiler built for
# WebAssembly, under node (ROADMAP-wasm.md W2): `build` builds it first --
# the compiler's C++ bridge for wasm32, the tool with `scaly build --target
# wasm32-wasip1` and tools/wasm/cc.sh, and the standard library's runtime
# archive for the target --, then it runs under tools/wasm/repl-node.mjs,
# which starts every entry's module. Needs tools/wasm/build-llvm.sh's
# libraries (SCALY_WASM_LLVM, default ~/scaly-wasm), a wasi-sdk and node.
# The REPL's standard input is this script's.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../.." && pwd)"
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
SYSROOT="$W/share/wasi-sysroot"
cd "$TREE"
SCALYC_PKG="packages/scalyc/$(tools/version.sh scalyc)"
SCALY_PKG="packages/scaly/$(tools/version.sh scaly)"
if [ "${1:-}" = build ]; then
  SRC="$(ls -d "$L"/llvm-project-*.src | tail -1)"
  "$W/bin/clang++" --target=wasm32-wasip1 --sysroot="$SYSROOT" -std=c++17 -O2 -fno-rtti -fno-exceptions \
    -D_WASI_EMULATED_MMAN -I"$SRC/llvm/include" -I"$L/wasm-build/include" -I"$SRC/lld/include" \
    -c "$SCALYC_PKG/scalyc/compiler/wasm_eh.cpp" -o "$L/wasm_eh.o"
  (ulimit -s 65520; SCALY_CC="$HERE/cc.sh" SCALY_CACHE="$L/cache" \
    scalyc/build/scaly build --target wasm32-wasip1 "$SCALYC_PKG/scaly_main.scaly" -o "$L/scaly.wasm")
  # the standard library for the target, as that build left it in the cache:
  # its object and its C files'
  v="$(tools/version.sh scaly)"
  objs=$(ls "$L"/cache/scaly-"$v"-*.o | grep -v '\.p[0-9]*\.o$')
  rm -f "$L/libscaly-wasm32.a"
  # shellcheck disable=SC2086
  "$W/bin/llvm-ar" rcs "$L/libscaly-wasm32.a" $objs
  shift
fi
clang_lib="$(ls -d "$W"/lib/clang/*/lib/wasm32-unknown-wasip1 | tail -1)"
SCALY_HOME="$TREE" SCALY_CACHE="$L/repl-cache" \
  SCALY_WASI_SYSROOT="$SYSROOT/lib/wasm32-wasip1" \
  SCALY_WASI_BUILTINS="$clang_lib/libclang_rt.builtins.a" \
  SCALY_WASM_RUNTIME="$L/libscaly-wasm32.a" \
  exec node --no-warnings --stack-size=65500 "$HERE/repl-node.mjs" "$L/scaly.wasm" "$@"
