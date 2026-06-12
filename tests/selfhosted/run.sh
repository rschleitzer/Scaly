#!/bin/bash
# Self-hosted literate-test harness.
# Compiles each generated test program in tests/selfhosted/ with the given
# stage binary and runs it. Value tests must print PASS (rc 0); tests with
# "; mode: compile" in the header only have to plan (--plan --no-prelude).
#
# Usage: tests/selfhosted/run.sh [stage-binary] [name-filter]
#   tests/selfhosted/run.sh                          # stage-0 reference
#   tests/selfhosted/run.sh /tmp/scalyc_stage2       # burn-down under stage-2
#   tests/selfhosted/run.sh /tmp/scalyc_stage2 choose
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-./scalyc/build/scalyc}
FILTER=$2
TIMEOUT_SECS=${TIMEOUT_SECS:-20}

pass=0; fail=0
failures=()

for f in tests/selfhosted/*.scaly; do
  t=$(basename "$f" .scaly)
  if [ -n "$FILTER" ]; then case $t in *$FILTER*) ;; *) continue;; esac; fi
  mode=$(sed -n 's/^; mode: //p' "$f")
  bin=/tmp/sht_$t
  rm -f "$bin" "$bin.o"

  if [ "$mode" = "compile" ]; then
    # plan-only with no prelude = exact parity with the C++ JIT harness's
    # compileToPlan (string/char/json tests were never executed there either)
    perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$STAGE" --plan --no-prelude "$f" >/dev/null 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1)); failures+=("$t PLAN rc=$rc")
    fi
    continue
  fi

  perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$STAGE" -o "$bin" "$f" >/dev/null 2>&1
  rc=$?
  if [ $rc -ne 0 ] || [ ! -x "$bin" ]; then
    fail=$((fail+1)); failures+=("$t COMPILE rc=$rc")
    continue
  fi
  out=$(perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$bin" 2>/dev/null)
  rc=$?
  if [ $rc -eq 0 ] && [ "$out" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t RUN rc=$rc out='$out'")
  fi
done

echo "selfhosted: $pass PASS, $fail FAIL ($STAGE)"
for line in "${failures[@]}"; do echo "  FAIL: $line"; done
[ $fail -eq 0 ]
