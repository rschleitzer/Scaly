#!/bin/bash
# The C++ emitter (scalyc --emit-cpp, compiler/CppEmitter.scaly) against the
# LLVM one: every program here is built both ways and must print the same.
#
#   tests/cpp/run.sh [compiler] [filter]     (default /tmp/scalyc_stage2)
#
# A program whose first lines say `; no-prelude` is compiled without the
# standard library. The C++ is built with the flags the emitter's header
# names; CXX picks the C++ compiler (default c++).
set -u
cd "$(dirname "$0")/../.."
SCALYC="${1:-/tmp/scalyc_stage2}"
FILTER="${2:-}"
CXX="${CXX:-c++}"
CXXFLAGS="-std=c++20 -fwrapv -fno-strict-aliasing -ffp-contract=off -w"
T="$(mktemp -d)"
trap 'rm -rf "$T"' EXIT
pass=0; fail=0
for src in tests/cpp/*.scaly; do
  name="$(basename "$src" .scaly)"
  [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
  flags=()
  head -5 "$src" | grep -q '^; no-prelude' && flags+=(--no-prelude)
  if ! "$SCALYC" "${flags[@]}" -o "$T/$name.llvm" "$src" > "$T/$name.llvm.log" 2>&1; then
    echo "FAIL $name: the LLVM build"; tail -3 "$T/$name.llvm.log"; fail=$((fail+1)); continue
  fi
  if ! "$SCALYC" "${flags[@]}" --emit-cpp -o "$T/$name.cpp" "$src" > "$T/$name.cpp.log" 2>&1; then
    echo "FAIL $name: --emit-cpp"; tail -3 "$T/$name.cpp.log"; fail=$((fail+1)); continue
  fi
  if ! $CXX $CXXFLAGS -o "$T/$name.cxx" "$T/$name.cpp" > "$T/$name.cxx.log" 2>&1; then
    echo "FAIL $name: $CXX"; head -5 "$T/$name.cxx.log"; fail=$((fail+1)); continue
  fi
  "$T/$name.llvm" > "$T/$name.llvm.out" 2>&1; lrc=$?
  "$T/$name.cxx" > "$T/$name.cxx.out" 2>&1; crc=$?
  if [ $lrc != $crc ] || ! cmp -s "$T/$name.llvm.out" "$T/$name.cxx.out"; then
    echo "FAIL $name: LLVM rc $lrc, C++ rc $crc"; diff "$T/$name.llvm.out" "$T/$name.cxx.out" | head -6
    fail=$((fail+1)); continue
  fi
  pass=$((pass+1))
done
echo "cpp: $pass passed, $fail failed"
[ $fail = 0 ]
