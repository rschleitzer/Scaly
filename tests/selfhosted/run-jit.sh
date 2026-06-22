#!/bin/bash
# Self-hosted literate-test harness — JIT edition (Phase 2).
# Runs each generated semantic test in tests/selfhosted/ THROUGH THE
# SELF-HOSTED IN-PROCESS JIT (scalyc --jit) instead of AOT compile+link+run.
# Value tests ("; mode: run") must print PASS via the JIT'd @main; tests with
# "; mode: compile" only have to plan (--plan --no-prelude), same as run.sh —
# they are compile-only (string/char/json/float) and never executed.
#
# This is the JIT counterpart of run.sh: same ground truth (PASS / plan-ok),
# different execution path. It exercises the whole emit→ORC-LLJIT→run pipeline
# (Phase 1's --run/--jit) across the full semantic corpus.
#
# Usage: tests/selfhosted/run-jit.sh [stage-binary] [name-filter]
#   tests/selfhosted/run-jit.sh /tmp/scalyc_stage2
#   tests/selfhosted/run-jit.sh /tmp/scalyc_stage2 choose
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
FILTER=$2
TIMEOUT_SECS=${TIMEOUT_SECS:-30}

pass=0; fail=0
failures=()

run_with_stack() {
  # The JIT self-compiles the prelude into one module; keep the bootstrap
  # stack limit so the planner's deep recursion doesn't overflow.
  ( ulimit -s 65520 2>/dev/null; perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$@" )
}

for f in tests/selfhosted/*.scaly; do
  t=$(basename "$f" .scaly)
  if [ -n "$FILTER" ]; then case $t in *$FILTER*) ;; *) continue;; esac; fi
  mode=$(sed -n 's/^; mode: //p' "$f")

  if [ "$mode" = "compile" ]; then
    # Compile-only tests (string/char/json/float) are not executable — plan
    # them with no prelude, exactly as run.sh does.
    run_with_stack "$STAGE" --plan --no-prelude "$f" >/dev/null 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$t PLAN rc=$rc"); fi
    continue
  fi

  out=$(run_with_stack "$STAGE" --jit "$f" 2>/dev/null)
  rc=$?
  if [ $rc -eq 0 ] && [ "$out" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t JIT rc=$rc out='$out'")
  fi
done

echo "selfhosted-jit: $pass PASS, $fail FAIL ($STAGE)"
for line in "${failures[@]}"; do echo "  FAIL: $line"; done
[ $fail -eq 0 ]
