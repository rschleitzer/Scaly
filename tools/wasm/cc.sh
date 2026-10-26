#!/bin/sh
# tools/wasm/cc.sh -- the wasi-sdk's clang as the C compiler of
# `scaly build --target wasm32-wasip1` for the compiler and its tool
# (tools/wasm/repl.sh): a final link (no -c) gets what the compiler links
# besides its own code -- libLLVM and lld's wasm linker (tools/wasm/
# build-llvm.sh, in SCALY_WASM_LLVM, default ~/scaly-wasm), the compiler's
# C++ bridge built for wasm (wasm_eh.o there), libc++ without exceptions, the
# mman emulation LLVM's Support uses, and the stack the planner wants (64 MB,
# as natively). And what an entry of the REPL needs, which the host loads as a
# side module into this program (Emitter.wasm_run): the table and the stack
# pointer exported, the table growable, every Scaly definition exported
# (--export-dynamic; LLVM's C++ is hidden, wasi-sdk's default), and the C
# symbols the standard library calls, which the C library and the shims define
# hidden: exported by name -- the undefined symbols of its objects
# (scaly-<version>-*.o in the arguments), and every extern its sources declare,
# pulled in (-u) where this program does not call it: a generic routine or an
# inline one (print and puts) is made in the entry, not here.
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
TREE="$(cd "$(dirname "$0")/../.." && pwd)"
SYSROOT="$W/share/wasi-sysroot"
link=1
for a in "$@"; do
  case "$a" in -c|-S|-E) link=0 ;; esac
done
if [ "$link" = 0 ]; then
  exec "$W/bin/clang" --sysroot="$SYSROOT" "$@"
fi
LIB="$L/wasm-build/lib"
RSP="$(mktemp -t scaly-exports)"
trap 'rm -f "$RSP"' EXIT
for a in "$@"; do
  case "$a" in
    scaly-[0-9]*.o|*/scaly-[0-9]*.o) "$W/bin/llvm-nm" -u "$a" ;;
  esac
done | awk '{print $NF}' | sort -u | sed 's/^/-Wl,--export-if-defined=/' > "$RSP"
STD="$TREE/packages/scaly/$("$TREE/tools/version.sh" scaly)"
{ grep -rhE '^[[:space:]]*(function|procedure)[[:space:]]+[A-Za-z][A-Za-z0-9_]*\(.*[[:space:]]extern[[:space:]]*$' "$STD" --include='*.scaly' \
    | sed -E 's/^[[:space:]]*(function|procedure)[[:space:]]+([A-Za-z][A-Za-z0-9_]*).*/\2/'
  echo malloc free aligned_alloc memcpy memmove memset __c_longjmp __wasm_setjmp __wasm_setjmp_test __wasm_longjmp | tr ' ' '\n'
} | sort -u | sed 's/.*/-Wl,-u,& -Wl,--export-if-defined=&/' >> "$RSP"
"$W/bin/clang" --sysroot="$SYSROOT" "$@" @"$RSP" \
  -Wl,--export-dynamic -Wl,--export-table -Wl,--growable-table -Wl,--export=__stack_pointer \
  "$L/wasm_eh.o" "$LIB/liblldWasm.a" "$LIB/liblldCommon.a" "$LIB"/libLLVM*.a \
  -L"$SYSROOT/lib/wasm32-wasip1/noeh" -lc++ -lc++abi -lwasi-emulated-mman \
  -Wl,-z,stack-size=67108864 -Wl,--max-memory=4294967296
