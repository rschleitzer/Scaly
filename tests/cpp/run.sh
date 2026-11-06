#!/bin/bash
# The C++ emitter (scalyc --emit-cpp, compiler/CppEmitter.scaly) against the
# LLVM one: every program is built both ways and must print the same and
# exit the same.
#
#   tests/cpp/run.sh [compiler] [filter]     (default /tmp/scalyc_stage2)
#
# The programs: tests/cpp/*.scaly (one whose first lines say `; no-prelude`
# is compiled without the standard library), then the AOT corpus
# tests/aot/*.scaly with the standard library -- which goes through C++ too:
# its root emitted as C++ once, as the runtime archive is (--no-prelude
# --no-tests), and linked with the C shims (tools/fcontext.sh, eio.sh,
# ctime.sh, panic.sh). The LLVM side links /tmp/libscaly.a as always. The
# C++ is built with the flags the emitter's header names; CXX picks the C++
# compiler (default c++).
set -u
cd "$(dirname "$0")/../.."
SCALYC="${1:-/tmp/scalyc_stage2}"
FILTER="${2:-}"
CXX="${CXX:-c++}"
CXXFLAGS=(-std=c++20 -fwrapv -fno-strict-aliasing -ffp-contract=off -w)
T="$(mktemp -d)"
trap 'rm -rf "$T"' EXIT
pass=0; fail=0; skip=0

# compare <name> <source> <C++ objects to link...>: both ways, then the outputs
compare() {
  local name=$1 src=$2; shift 2
  local flags=()
  head -5 "$src" | grep -q '^; no-prelude' && flags+=(--no-prelude)
  # a program the LLVM way cannot build alone (one that wants a JIT host or
  # LLVM itself linked) is no comparison
  if ! "$SCALYC" ${flags[@]+"${flags[@]}"} -o "$T/$name.llvm" "$src" > "$T/$name.llvm.log" 2>&1; then
    skip=$((skip+1)); return
  fi
  if ! "$SCALYC" ${flags[@]+"${flags[@]}"} --emit-cpp -o "$T/$name.cpp" "$src" > "$T/$name.cpp.log" 2>&1; then
    echo "FAIL $name: --emit-cpp"; tail -3 "$T/$name.cpp.log"; fail=$((fail+1)); return
  fi
  if ! $CXX "${CXXFLAGS[@]}" -o "$T/$name.cxx" "$T/$name.cpp" "$@" -lm > "$T/$name.cxx.log" 2>&1; then
    echo "FAIL $name: $CXX"; grep -m5 'error' "$T/$name.cxx.log"; fail=$((fail+1)); return
  fi
  "$T/$name.llvm" > "$T/$name.llvm.out" 2>&1; local lrc=$?
  "$T/$name.cxx" > "$T/$name.cxx.out" 2>&1; local crc=$?
  if [ $lrc != $crc ] || ! cmp -s "$T/$name.llvm.out" "$T/$name.cxx.out"; then
    echo "FAIL $name: LLVM rc $lrc, C++ rc $crc"; diff "$T/$name.llvm.out" "$T/$name.cxx.out" | head -6
    fail=$((fail+1)); return
  fi
  pass=$((pass+1))
}

for src in tests/cpp/*.scaly; do
  name="$(basename "$src" .scaly)"
  [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
  compare "$name" "$src"
done

# the standard library as C++, and the shims
STDLIB="$(ls -d packages/scaly/*/scaly.scaly | tail -1)"
if ! "$SCALYC" --no-prelude --no-tests --emit-cpp -o "$T/libscaly.cpp" "$STDLIB" > "$T/libscaly.log" 2>&1 \
   || ! $CXX "${CXXFLAGS[@]}" -c -o "$T/libscaly.o" "$T/libscaly.cpp" > "$T/libscaly.cxx.log" 2>&1; then
  echo "FAIL the standard library as C++"; grep -m5 'error' "$T/libscaly.cxx.log" "$T/libscaly.log"
  fail=$((fail+1))
else
  for s in fcontext eio ctime panic; do tools/$s.sh "$T/$s.o" > /dev/null 2>&1 || echo "tools/$s.sh failed"; done
  for src in tests/aot/*.scaly; do
    name="aot_$(basename "$src" .scaly)"
    [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
    compare "$name" "$src" "$T/libscaly.o" "$T/fcontext.o" "$T/eio.o" "$T/ctime.o" "$T/panic.o"
  done
fi
echo "cpp: $pass passed, $fail failed, $skip skipped (no LLVM build)"
[ $fail = 0 ]
