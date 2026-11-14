#!/bin/bash
# tests/math/run.sh [compiler] -- scaly/math.scaly against the exact values
# and across builds.
#
#   1. Each function, built for this machine (the compiler under
#      SCALY_POISON), on random numbers of its
#      domain and its special values: the largest error at most 0.51 units
#      in the last place (check.py: Python's decimal to 60 digits).
#   2. Where this is a Mac that runs x86_64 (Rosetta 2): the same program
#      built for x86_64 with -mcpu=haswell (FMA) prints the same bits as the
#      one built here -- the condition for a cluster of both -- and built
#      for the generic x86_64 (no FMA: relaxed -- as fast, not as exact)
#      stays within a unit in the last place.
#      SKIPPED by name elsewhere.
set -u
cd "$(dirname "$0")/../.."
BIN="${1:-/tmp/scalyc_stage2}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
export MATH_INPUTS="$WORK/inputs.txt"
pass=0; fail=0; failures=()
verdict() { if [ "$1" = 0 ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$2"); fi; }

# under SCALY_POISON: the compiler reading a released page faults here
# (the type of `to_bits` once lived on one)
if ! SCALY_POISON=1 "$BIN" -O2 -o "$WORK/bits" tests/math/bits.scaly > "$WORK/build.log" 2>&1; then
  echo "math: FAIL — tests/math/bits.scaly does not build ($(grep -m1 error "$WORK/build.log" | cut -c1-120))"; exit 1
fi
for fn in exp exp2 expm1 log log2 log10 log1p; do
  python3 tests/math/check.py $fn 20000 1 "$WORK/bits"; verdict $? "$fn"
done

# 2. x86_64 under Rosetta: Haswell the same bits, generic within the limit
cross() {
  local T=x86_64-apple-darwin X="$WORK/x86"
  [ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] || { echo "math: x86_64 SKIPPED (not an arm64 Mac)"; return 0; }
  echo 'int main(void){return 42;}' > "$WORK/probe.c"
  clang -arch x86_64 "$WORK/probe.c" -o "$WORK/probe" 2>/dev/null && "$WORK/probe"; [ $? = 42 ] || { echo "math: x86_64 SKIPPED (no Rosetta 2)"; return 0; }
  mkdir -p "$X"
  "$BIN" --target $T -c --no-prelude --no-tests -o "$X/libscaly.o" packages/scaly/$(tools/version.sh scaly)/scaly.scaly > "$X/rt.log" 2>&1 || { verdict 1 "x86_64 runtime"; return 0; }
  for s in fcontext eio ctime panic; do SCALY_ARCH=x86_64 "tools/$s.sh" "$X/$s.o" > /dev/null 2>&1 || { verdict 1 "x86_64 $s.o"; return 0; }; done
  ar rcs "$X/libscaly.a" "$X/libscaly.o" "$X/fcontext.o" "$X/eio.o" "$X/ctime.o" "$X/panic.o"
  for cpu in haswell generic; do
    "$BIN" --target $T -mcpu=$cpu -O2 -c -o "$X/bits_$cpu.o" tests/math/bits.scaly > /dev/null 2>&1 &&
      clang -arch x86_64 "$X/bits_$cpu.o" "$X/libscaly.a" -lm -o "$X/bits_$cpu" 2>/dev/null || { verdict 1 "x86_64 $cpu build"; return 0; }
  done
  for fn in exp exp2 expm1 log log2 log10 log1p; do
    python3 tests/math/check.py $fn 20000 2 "$WORK/bits" > /dev/null
    "$WORK/bits" $fn "$MATH_INPUTS" > "$WORK/$fn.here"
    "$X/bits_haswell" $fn "$MATH_INPUTS" > "$WORK/$fn.haswell"
    if cmp -s "$WORK/$fn.here" "$WORK/$fn.haswell"; then
      echo "$fn: x86_64 haswell the same bits as here"; verdict 0 ""
    else
      echo "$fn: x86_64 haswell differs from here in $(diff "$WORK/$fn.here" "$WORK/$fn.haswell" | grep -c '^<') results"; verdict 1 "$fn haswell bits"
    fi
    printf '%s (x86_64 generic, relaxed) ' "$fn"
    python3 tests/math/check.py $fn 20000 3 "$X/bits_generic" 1.0; verdict $? "$fn generic"
  done
}
cross

echo "math: $pass PASS, $fail FAIL ${failures[*]:-}"
[ $fail -eq 0 ]
