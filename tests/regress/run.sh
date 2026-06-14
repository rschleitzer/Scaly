#!/bin/bash
# Self-hosted regression suite — fixes NOT present in the frozen C++ stage-0.
#
# Step-C/D-era corrections (s192+) sometimes make the self-hosted compiler
# strictly MORE correct than stage-0 (e.g. use-aliased free-function call
# mangling: stage-0 silently drops the call). Such cases can't live in the
# stage-0-referenced AOT corpus or the stage-0-baselined selfhosted suite, so
# they run only against a self-hosted stage binary. Each test compiles+runs
# and must print PASS.
#
# Usage: tests/regress/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
pass=0; fail=0; failures=()
for f in tests/regress/*.scaly; do
  t=$(basename "$f" .scaly)
  bin=/tmp/rt_$t; rm -f "$bin"
  "$STAGE" -o "$bin" "$f" >/dev/null 2>&1
  out=$("$bin" 2>/dev/null)
  if [ "$out" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: '$out'")
  fi
done
echo "regress: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
