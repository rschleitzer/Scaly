#!/bin/bash
# Stage-5 op-kernel suite (5.1): f32 matmul / softmax / layernorm /
# GELU / embedding-lookup, each checked against reference values.
# Compiled at -O2 — the level the demo uses; the kernels' classifier
# verdicts (parallel) are asserted so a TaskPlanner regression that
# silently serializes an op kernel fails the suite.
#
# The autodiff_* tests (5.2) exercise the scaly/tensor tape: their op
# kernels live in libscaly.a and classify at the ARCHIVE build, not in
# the test compile, so the parallel assertion is skipped for them.
#
# Usage: tests/tensor/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
pass=0; fail=0; failures=()
for f in tests/tensor/*.scaly; do
  t=$(basename "$f" .scaly)
  bin=/tmp/tt_$t; rm -f "$bin"
  plan=$("$STAGE" --task-plan -O2 -o "$bin" "$f" 2>&1)
  case "$t" in autodiff_*) ;; *)
  if ! printf '%s' "$plan" | grep -q ": parallel$"; then
    fail=$((fail+1)); failures+=("$t: not classified parallel"); continue
  fi;; esac
  out=$("$bin" 2>/dev/null)
  if [ "$out" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: '$out'")
  fi
done
echo "tensor: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
