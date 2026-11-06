#!/bin/bash
# The C++ emitter (scalyc --emit-cpp, compiler/CppEmitter.scaly) against the
# LLVM one: every program is built both ways and must print the same and
# exit the same.
#
#   tests/cpp/run.sh [compiler] [filter]     (default /tmp/scalyc_stage2)
#
# The programs: tests/cpp/*.scaly (one whose first lines say `; no-prelude`
# is compiled without the standard library), the AOT corpus tests/aot and
# the regression corpus tests/regress (what compiles and runs on its own: not
# the diagnostics, the IR checks, nor what wants an environment, arguments
# or a home of its own). The standard library goes through C++ too: its root
# emitted as C++ once, as the runtime archive is (--no-prelude --no-tests),
# and linked with the C shims (tools/fcontext.sh, eio.sh, ctime.sh,
# panic.sh). The LLVM side links /tmp/libscaly.a as always. A program gets
# 30 s each way. The C++ is built with the flags the emitter's header names;
# CXX picks the C++ compiler (default c++), JOBS how many run side by side.
set -u
cd "$(dirname "$0")/../.."
SCALYC="${1:-/tmp/scalyc_stage2}"
FILTER="${2:-}"
CXX="${CXX:-c++}"
T="$(mktemp -d)"
trap 'rm -rf "$T"' EXIT
JOBS="${JOBS:-$(sysctl -n hw.ncpu 2>/dev/null || nproc)}"
export SCALYC CXX T

# one <name> <source> [lib]: both ways, then the outputs; prints PASS, SKIP
# or FAIL with the reason
one() {
  local name=$1 src=$2 lib=${3:-}
  local CXXFLAGS=(-std=c++20 -fwrapv -fno-strict-aliasing -ffp-contract=off -w)
  local objs=()
  [ -n "$lib" ] && objs=("$T/libscaly.o" "$T/fcontext.o" "$T/eio.o" "$T/ctime.o" "$T/panic.o")
  local flags=()
  head -5 "$src" | grep -q '^; no-prelude' && flags+=(--no-prelude)
  # a program the LLVM way cannot build alone (one that wants a JIT host or
  # LLVM itself linked) is no comparison
  if ! "$SCALYC" ${flags[@]+"${flags[@]}"} -o "$T/$name.llvm" "$src" > "$T/$name.llvm.log" 2>&1; then
    echo "SKIP $name"; return
  fi
  if ! "$SCALYC" ${flags[@]+"${flags[@]}"} --emit-cpp -o "$T/$name.cpp" "$src" > "$T/$name.cpp.log" 2>&1; then
    echo "FAIL $name: --emit-cpp: $(tail -1 "$T/$name.cpp.log")"; return
  fi
  if ! $CXX "${CXXFLAGS[@]}" -o "$T/$name.cxx" "$T/$name.cpp" ${objs[@]+"${objs[@]}"} -lm > "$T/$name.cxx.log" 2>&1; then
    echo "FAIL $name: c++: $(grep -m1 -e "error" -e "^  \"" "$T/$name.cxx.log" | sed "s/^[^ ]*: error: //")"; return
  fi
  perl -e 'alarm 30; exec @ARGV' "$T/$name.llvm" > "$T/$name.llvm.out" 2>&1; local lrc=$?
  perl -e 'alarm 30; exec @ARGV' "$T/$name.cxx" > "$T/$name.cxx.out" 2>&1; local crc=$?
  if [ $lrc != $crc ] || ! cmp -s "$T/$name.llvm.out" "$T/$name.cxx.out"; then
    echo "FAIL $name: LLVM rc $lrc, C++ rc $crc"; return
  fi
  echo "PASS $name"
}
export -f one

LIST="$T/list"
: > "$LIST"
for src in tests/cpp/*.scaly; do
  name="$(basename "$src" .scaly)"
  [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
  if head -5 "$src" | grep -q '^; no-prelude'; then
    echo "$name $src" >> "$LIST"
  else
    echo "$name $src lib" >> "$LIST"
  fi
done

# the standard library as C++, and the shims
STDLIB="$(ls -d packages/scaly/*/scaly.scaly | tail -1)"
libok=1
if ! "$SCALYC" --no-prelude --no-tests --emit-cpp -o "$T/libscaly.cpp" "$STDLIB" > "$T/libscaly.log" 2>&1 \
   || ! $CXX -std=c++20 -fwrapv -fno-strict-aliasing -ffp-contract=off -w -c -o "$T/libscaly.o" "$T/libscaly.cpp" > "$T/libscaly.cxx.log" 2>&1; then
  echo "FAIL the standard library as C++"; grep -m5 'error' "$T/libscaly.cxx.log" "$T/libscaly.log"
  libok=0
else
  for s in fcontext eio ctime panic; do tools/$s.sh "$T/$s.o" > /dev/null 2>&1 || echo "tools/$s.sh failed"; done
  for src in tests/aot/*.scaly; do
    name="aot_$(basename "$src" .scaly)"
    [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
    echo "$name $src lib" >> "$LIST"
  done
  for src in tests/regress/*.scaly; do
    base="$(basename "$src" .scaly)"
    case "$base" in xfail_*|neg*) continue ;; esac
    grep -q '^; expect-ir:\|^; env:\|^; args:\|^; home:' "$src" && continue
    name="regress_$base"
    [ -n "$FILTER" ] && [[ "$name" != *"$FILTER"* ]] && continue
    echo "$name $src lib" >> "$LIST"
  done
fi

xargs -P "$JOBS" -L 1 bash -c 'one "$@"' _ < "$LIST" > "$T/results"
grep '^FAIL' "$T/results" | sort
pass=$(grep -c '^PASS' "$T/results")
fail=$(grep -c '^FAIL' "$T/results")
skip=$(grep -c '^SKIP' "$T/results")
[ $libok = 0 ] && fail=$((fail+1))
echo "cpp: $pass passed, $fail failed, $skip skipped (no LLVM build)"
[ $fail = 0 ]
