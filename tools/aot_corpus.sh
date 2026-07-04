#!/bin/bash
# AOT corpus: compile each tests/aot test with the given stage binary, run it,
# and compare stdout to the test's "; Expected:" comment (the ground truth).
#
# WHY compare to "; Expected:" rather than to a freshly-built stage-0 binary:
# each fresh binary's FIRST execution pays ~280ms of macOS first-exec
# validation (AMFI/Gatekeeper + dyld launch-closure build for a never-seen
# executable). Building + running a SECOND (stage-0 reference) binary per test
# doubled the suite's wall time for no extra coverage — the Expected comment is
# absolute ground truth (catches a stage-2 miscompile even if stage-0 shares
# it), and cycle.sh already proves stage-1 == stage-2 emission. Tests WITHOUT
# an "; Expected:" comment (exit-code / value tests, e.g. scalyc_test_exit,
# ref_string_item) cross-check against a reference binary built by the
# compiler at scalyc/build/scalyc (the seed-built compiler since the C++
# stage-0 was retired); when that binary is absent the cross-check is skipped.
#
# Usage: tools/aot_corpus.sh <stage-binary> [label]
cd "$(dirname "$0")/.."
STAGE=$1; NAME=${2:-$1}
pass=0; fail=0; failed=""
for f in tests/aot/*.scaly; do
  t=$(basename $f .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  if [ -n "$expected" ]; then
    # Ground-truth path: build + run the stage binary only, compare stdout.
    rm -f /tmp/aot_new_$t
    $STAGE -o /tmp/aot_new_$t $f >/dev/null 2>&1
    if [ $? -ne 0 ]; then fail=$((fail+1)); failed="$failed $t(compile)"; continue; fi
    newout=$(/tmp/aot_new_$t 2>/dev/null); newrc=$?
    if [ "$newrc" = "0" ] && [ "$newout" = "$expected" ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1)); failed="$failed $t"
    fi
  else
    # No Expected comment -> cross-check against the reference compiler.
    rm -f /tmp/aot_ref_$t /tmp/aot_new_$t
    if [ -x ./scalyc/build/scalyc ]; then
      ./scalyc/build/scalyc -o /tmp/aot_ref_$t $f >/dev/null 2>&1
      refrc_c=$?
    else
      refrc_c=1
    fi
    $STAGE -o /tmp/aot_new_$t $f >/dev/null 2>&1
    newrc_c=$?
    if [ $refrc_c -ne 0 ]; then continue; fi  # not a runnable reference
    refout=$(/tmp/aot_ref_$t 2>/dev/null); refrc=$?
    newout=$(/tmp/aot_new_$t 2>/dev/null); newrc=$?
    if [ "$newrc_c" = "0" ] && [ "$refout" = "$newout" ] && [ "$refrc" = "$newrc" ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1)); failed="$failed $t"
    fi
  fi
done
echo "$NAME: $pass PASS, $fail FAIL$failed"
