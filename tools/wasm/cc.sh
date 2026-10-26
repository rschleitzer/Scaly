#!/bin/sh
# tools/wasm/cc.sh -- the wasi-sdk's clang as the C compiler of
# `scaly build --target wasm32-wasip1` for the compiler and its tool
# (tools/wasm/repl.sh): a final link (no -c) gets what the compiler links
# besides its own code -- libLLVM and lld's wasm linker (tools/wasm/
# build-llvm.sh, in SCALY_WASM_LLVM, default ~/scaly-wasm), the compiler's
# C++ bridge built for wasm (wasm_eh.o there), libc++ without exceptions, the
# mman emulation LLVM's Support uses, and the stack the planner wants (64 MB,
# as natively).
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
SYSROOT="$W/share/wasi-sysroot"
link=1
for a in "$@"; do
  case "$a" in -c|-S|-E) link=0 ;; esac
done
if [ "$link" = 0 ]; then
  exec "$W/bin/clang" --sysroot="$SYSROOT" "$@"
fi
LIB="$L/wasm-build/lib"
exec "$W/bin/clang" --sysroot="$SYSROOT" "$@" \
  "$L/wasm_eh.o" "$LIB/liblldWasm.a" "$LIB/liblldCommon.a" "$LIB"/libLLVM*.a \
  -L"$SYSROOT/lib/wasm32-wasip1/noeh" -lc++ -lc++abi -lwasi-emulated-mman \
  -Wl,-z,stack-size=67108864 -Wl,--max-memory=4294967296
