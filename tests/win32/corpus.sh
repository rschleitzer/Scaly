#!/bin/bash
# The AOT + fiber corpora on Windows (stage 7, breadth after the substrate).
#
# ONE script, TWO modes, because the selection logic must not exist twice:
#
#   corpus.sh emit <compiler> <outdir>   on the LINUX leg — cross-emits one
#                                        Windows object per eligible test
#   corpus.sh run  <outdir> <archive>    on the WINDOWS runner — links each
#                                        object and checks what it printed
#
# The expectations are read from the SOURCES in both modes (the Windows job
# checks the repo out too), so there is no manifest to drift out of step.
#
# ELIGIBILITY: a test is eligible if it carries its own ground truth in ANY of
# the three forms below. A test with none of them is cross-checked on POSIX
# against a reference binary built by another compiler, which cannot exist on
# the Windows runner where nothing but the cross-emitted object arrives. Those
# are SKIPPED and the count is printed: a corpus that quietly covered less than
# it looked like would be worse than a smaller one that says so.
#
# ★The first version required `; Expected:` and thereby dropped the three most
# interesting fiber cases — `guard_overflow` (exit 108 plus a stderr
# substring), which is the only thing that exercises the Windows vectored
# exception handler at all, and the two deadlock detectors (exit 106). Their
# truth is an exit code, not stdout. Checking for the WIDEST form of ground
# truth rather than the most common one is what keeps a corpus honest.
#
# Conventions honoured, same as the POSIX drivers:
#   ; Expected:      exact stdout (required for eligibility)
#   ; ExpectedExit:  exit code, default 0
#   ; ExpectedErr:   substring that must appear on stderr
#
# Every run is under a TIMEOUT. A hang is the one outcome worse than a failure
# here — a fiber test that waits on something Windows reports differently would
# otherwise stall the job until the six-hour job limit, and tell us nothing.
set -u
cd "$(dirname "$0")/../.." || exit 1

MODE=${1:-}
SUITES="aot fiber"
TIMEOUT_S=60
TRIPLE=x86_64-pc-windows-msvc

# Objects are named <suite>__<test>.o so the run mode can find the source again
# without a manifest.
src_of() {
  local base=$1
  echo "tests/${base%%__*}/${base#*__}.scaly"
}

case "$MODE" in
emit)
  SC=${2:?usage: corpus.sh emit <compiler> <outdir>}
  OUT=${3:?usage: corpus.sh emit <compiler> <outdir>}
  mkdir -p "$OUT"
  n=0; skipped=0; failed=0; names=""
  for s in $SUITES; do
    for f in tests/$s/*.scaly; do
      [ -f "$f" ] || continue
      t=$(basename "$f" .scaly)
      if ! grep -q '^; Expected\(Exit\|Err\)\?: ' "$f"; then
        skipped=$((skipped+1)); continue
      fi
      if "$SC" -c --target "$TRIPLE" -o "$OUT/${s}__${t}.o" "$f" > /dev/null 2>&1; then
        n=$((n+1))
      else
        failed=$((failed+1)); names="$names ${s}/${t}"
      fi
    done
  done
  echo "corpus emit: $n objects, $skipped skipped (no self-contained ground truth), $failed cross-emit failures$names"
  [ "$failed" -eq 0 ]
  ;;

run)
  OUT=${2:?usage: corpus.sh run <outdir> <archive>}
  LIB=${3:?usage: corpus.sh run <outdir> <archive>}
  # Git Bash usually ships coreutils' timeout; without it the guard degrades to
  # "no guard", which is said out loud rather than assumed.
  TO=""
  if command -v timeout > /dev/null 2>&1; then
    TO="timeout $TIMEOUT_S"
  else
    echo "corpus run: WARNING — no timeout(1); a hanging test will stall the job"
  fi
  pass=0; fail=0; failures=""
  for o in "$OUT"/*.o; do
    [ -f "$o" ] || continue
    base=$(basename "$o" .o)
    src=$(src_of "$base")
    if [ ! -f "$src" ]; then
      fail=$((fail+1)); failures="$failures $base(no-source)"; continue
    fi
    want=$(sed -n 's/^; Expected: //p' "$src")
    want_rc=$(sed -n 's/^; ExpectedExit: //p' "$src"); want_rc=${want_rc:-0}
    want_err=$(sed -n 's/^; ExpectedErr: //p' "$src")

    exe="$OUT/$base.exe"
    if ! clang --target="$TRIPLE" "$o" "$LIB" -lws2_32 -o "$exe" > "$OUT/$base.link" 2>&1; then
      fail=$((fail+1)); failures="$failures $base(link)"; continue
    fi
    # shellcheck disable=SC2086
    got=$($TO "$exe" 2> "$OUT/$base.err" | tr -d '\r'); rc=$?
    ok=1
    [ "$rc" = "$want_rc" ] || ok=0
    [ "$got" = "$want" ] || ok=0
    if [ -n "$want_err" ] && ! grep -q "$want_err" "$OUT/$base.err"; then ok=0; fi
    if [ "$ok" = 1 ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1))
      if [ "$rc" = 124 ]; then
        failures="$failures $base(TIMEOUT)"
      else
        failures="$failures $base(rc=$rc)"
      fi
    fi
  done
  echo "corpus run: $pass PASS, $fail FAIL$failures"
  [ "$fail" -eq 0 ]
  ;;

*)
  echo "usage: tests/win32/corpus.sh emit <compiler> <outdir>"
  echo "       tests/win32/corpus.sh run  <outdir> <archive>"
  exit 2
  ;;
esac
