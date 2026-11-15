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
#      one built here, on the numbers of 1 -- the condition for a cluster of
#      both -- and built for the generic x86_64 (no FMA: relaxed -- as fast,
#      not as exact) stays within a unit in the last place (tan, atan2,
#      hypot: two). SKIPPED by name elsewhere.
#
# The exact values are the cost (decimal, one core each), so the checks run
# side by side, each function in a directory of its own: MATH_JOBS at once
# (default every core), pow's first.
set -u
cd "$(dirname "$0")/../.."
BIN="${1:-/tmp/scalyc_stage2}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
FNS="exp exp2 expm1 log log2 log10 log1p sin cos tan atan asin acos atan2 pow hypot cbrt sinh cosh tanh"
JOBS=${MATH_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
pass=0; fail=0; failures=()
verdict() { if [ "$1" = 0 ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$2"); fi; }
# under SCALY_POISON: the compiler reading a released page faults here
# (the type of `to_bits` once lived on one)
if ! SCALY_POISON=1 "$BIN" -O2 -o "$WORK/bits" tests/math/bits.scaly > "$WORK/build.log" 2>&1; then
  echo "math: FAIL — tests/math/bits.scaly does not build ($(grep -m1 error "$WORK/build.log" | cut -c1-120))"; exit 1
fi

# 2's builds: x86_64 under Rosetta, Haswell and generic
X="$WORK/x86"
cross_builds() {
  local T=x86_64-apple-darwin
  [ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] || { echo "math: x86_64 SKIPPED (not an arm64 Mac)"; return 1; }
  echo 'int main(void){return 42;}' > "$WORK/probe.c"
  clang -arch x86_64 "$WORK/probe.c" -o "$WORK/probe" 2>/dev/null && "$WORK/probe"; [ $? = 42 ] || { echo "math: x86_64 SKIPPED (no Rosetta 2)"; return 1; }
  mkdir -p "$X"
  "$BIN" --target $T -c --no-prelude --no-tests -o "$X/libscaly.o" packages/scaly/$(tools/version.sh scaly)/scaly.scaly > "$X/rt.log" 2>&1 || { verdict 1 "x86_64 runtime"; return 1; }
  for s in fcontext eio ctime panic; do SCALY_ARCH=x86_64 "tools/$s.sh" "$X/$s.o" > /dev/null 2>&1 || { verdict 1 "x86_64 $s.o"; return 1; }; done
  ar rcs "$X/libscaly.a" "$X/libscaly.o" "$X/fcontext.o" "$X/eio.o" "$X/ctime.o" "$X/panic.o"
  for cpu in haswell generic; do
    "$BIN" --target $T -mcpu=$cpu -O2 -c -o "$X/bits_$cpu.o" tests/math/bits.scaly > /dev/null 2>&1 &&
      clang -arch x86_64 "$X/bits_$cpu.o" "$X/libscaly.a" -lm -o "$X/bits_$cpu" 2>/dev/null || { verdict 1 "x86_64 $cpu build"; return 1; }
  done
}
CROSS=0
cross_builds && CROSS=1

# one job, in $WORK/<fn>: `native` checks the function built here and, where
# 2 runs, compares Haswell's bits on the same numbers; `generic` checks the
# generic x86_64 build. Each writes its report to out_<job> and one line per
# verdict ("<rc> <name>") to verdicts_<job>.
one() {
  local fn=$1 job=$2 D="$WORK/$1" lim=1.0
  mkdir -p "$D"
  local out="$D/out_$job" ver="$D/verdicts_$job"
  if [ "$job" = native ]; then
    MATH_INPUTS="$D/inputs" python3 tests/math/check.py "$fn" 20000 1 "$WORK/bits" > "$out"
    echo "$? $fn" > "$ver"
    [ "$CROSS" = 1 ] || return 0
    "$WORK/bits" "$fn" "$D/inputs" > "$D/here"
    "$X/bits_haswell" "$fn" "$D/inputs" > "$D/haswell"
    if cmp -s "$D/here" "$D/haswell"; then
      echo "$fn: x86_64 haswell the same bits as here" >> "$out"; echo "0 $fn haswell bits" >> "$ver"
    else
      echo "$fn: x86_64 haswell differs from here in $(diff "$D/here" "$D/haswell" | grep -c '^<') results" >> "$out"
      echo "1 $fn haswell bits" >> "$ver"
    fi
  else
    # tan, atan2: the rest of a division is not exact without FMA, hypot:
    # the squares' errors (math.scaly)
    case "$fn" in tan|atan2|hypot) lim=2.0 ;; esac
    printf '%s (x86_64 generic, relaxed) ' "$fn" > "$out"
    MATH_INPUTS="$D/inputs3" python3 tests/math/check.py "$fn" 20000 3 "$X/bits_generic" $lim >> "$out"
    echo "$? $fn generic" > "$ver"
  fi
}
export -f one
export WORK X CROSS
# the jobs, pow's first: its exact values cost five times any other's
JOBLIST="$WORK/jobs"
for fn in pow $FNS; do
  [ "$fn" = pow ] && [ -s "$JOBLIST" ] && continue
  echo "$fn native" >> "$JOBLIST"
  [ "$CROSS" = 1 ] && echo "$fn generic" >> "$JOBLIST"
done
xargs -P "$JOBS" -L1 bash -c 'one "$1" "$2"' _ < "$JOBLIST"

for fn in $FNS; do
  for job in native generic; do
    [ "$job" = generic ] && [ "$CROSS" = 0 ] && continue
    cat "$WORK/$fn/out_$job" 2>/dev/null
    if [ -s "$WORK/$fn/verdicts_$job" ]; then
      while read -r rc name; do
        if [ "$rc" = 0 ]; then verdict 0 ""; else verdict 1 "$name"; fi
      done < "$WORK/$fn/verdicts_$job"
    else
      verdict 1 "$fn $job (no verdict)"
    fi
  done
done
echo "math: $pass PASS, $fail FAIL ${failures[*]:-}"
[ $fail -eq 0 ]
