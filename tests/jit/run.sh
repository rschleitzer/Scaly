#!/bin/bash
# JIT-mode regression suite — runs raw .scaly programs through the in-process
# ORC JIT (scalyc --jit) and checks their self-reported output.
#
# Distinct from tests/selfhosted (generated from .sgm, stack-only value tests):
# these hand-written programs exercise page/region allocation IN THE JIT, which
# the selfhosted value tests never do (string/allocation tests there are
# compile-only). Guards the emit_jit_stubs host-symbol-shadowing regression.
#
# Each program prints PASS on success. Usage: tests/jit/run.sh [stage-binary]
#
# Run against a BOOTSTRAP-linked compiler (the default /tmp/scalyc_stage2, same
# as the selfhosted suite): JIT region allocation resolves the runtime from the
# host's exported symbols, which the multi-object bootstrap link keeps global.
# The whole-program `opt -O2` build (scalyc/build/scalyc) auto-hides those
# symbols, so it can't yet JIT region-allocating programs (see emit_jit_stubs).
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
TIMEOUT_SECS=${TIMEOUT_SECS:-30}

pass=0; fail=0; failures=()
for f in tests/jit/*.scaly; do
  t=$(basename "$f" .scaly)
  out=$( ulimit -s 65520 2>/dev/null
         perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$STAGE" --jit "$f" 2>&1 )
  rc=$?
  if [ $rc -eq 0 ] && printf '%s' "$out" | grep -q '^PASS'; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: rc=$rc out='$out'")
  fi
done

echo "jit: $pass PASS, $fail FAIL"
if [ $fail -gt 0 ]; then
  for x in "${failures[@]}"; do echo "  FAIL $x"; done
  exit 1
fi
