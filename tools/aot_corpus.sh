#!/bin/bash
# AOT corpus: compile each tests/aot test with stage-0 (reference) and the
# given stage binary, run both, compare stdout and exit code.
# Usage: tools/aot_corpus.sh <stage-binary> [label]
cd "$(dirname "$0")/.."
STAGE=$1; NAME=${2:-$1}
pass=0; fail=0; failed=""
for f in tests/aot/*.scaly; do
  t=$(basename $f .scaly)
  rm -f /tmp/aot_ref_$t /tmp/aot_new_$t
  ./scalyc/build/scalyc -o /tmp/aot_ref_$t $f >/dev/null 2>&1
  refrc_c=$?
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
done
echo "$NAME: $pass PASS, $fail FAIL$failed"
