#!/bin/bash
# tests/task-plan/run.sh — the gate for the slots a parallel `for` may write
# (packages/scalyc/<version>/scalyc/compiler/TaskPlanner.scaly, put_is_disjoint).
#
# A round may `put` into a vector made before the loop only where the compiler
# can show that no two rounds write one slot, and that the round reads the
# vector at no other slot. A loop wrongly called parallel computes a result
# that depends on timing, at rc 0 -- so this gate pins (1) the exact report
# over a fixture carrying one loop per shape, (2) each DECISION by its line,
# apart from the wording, and (3) that the program prints the same line on
# one worker and on all of them.
#
# Usage: tests/task-plan/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
FIX=tests/task-plan/fixture.scaly
pass=0; fail=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }
line() { printf '%s\n' "$got" | grep "fixture.scaly:$1:"; }
par() { line "$1" | grep -q ": parallel$"; }
blk() { line "$1" | grep -q ": blocked: $2"; }

out=$("$STAGE" --task-plan "$FIX" 2>&1); rc=$?
check "report rc=0 (got $rc)" '[ $rc -eq 0 ]'
got=$(printf '%s\n' "$out" | grep '^task-plan:')
check "report matches fixture.expected" 'diff <(printf "%s\n" "$got") tests/task-plan/fixture.expected >/dev/null'
check "head counts the loops" 'printf "%s\n" "$got" | head -1 | grep -q "^task-plan: 29 for-loops, 16 parallelizable$"'

# One slot per round, shown.
check "the counter"                                   'par 47'
check "its own slot read, and the length"             'par 56'
check "mirrored: bound minus the counter"             'par 65'
check "a stride and an offset inside it"              'par 71'
check "a row, the column a for's counter"             'par 85'
check "a row, the column a while's counter"           'par 93'
check "tiles, the condition leaving room"             'par 135'
check "two loops in the round, one row"               'par 157'
check "the counter as the column of its own bound"    'par 169'
check "two vectors, a form each"                      'par 179'
check "a vector the round made"                       'par 191'
check "a field of the receiver"                       'par 20'

# Not shown: the loop keeps its order.
check "half the counter"                              'blk 50 "put into an external vector: its index is not one slot"'
check "the neighbour's slot read"                     'blk 53 "reads beside the slot it puts into: v$"'
check "two puts, two indices"                         'blk 59 "put into an external vector: a second index$"'
check "a cast that narrows"                           'blk 68 "put into an external vector: its index"'
check "times a name alone"                            'blk 74 "put into an external vector: its index"'
check "an element, which can repeat"                  'blk 81 "put into an external vector: its index"'
check "a counter over another bound"                  'blk 89 "put into an external vector: its index"'
check "the put after the counter's step"              'blk 103 "put into an external vector: its index"'
check "the counter written a second time"             'blk 113 "put into an external vector: its index counts with j$"'
check "the counter begun at a name"                   'blk 125 "put into an external vector: its index"'
check "tiles whose condition leaves no room"          'blk 147 "put into an external vector: its index"'
check "through a name the body bound"                 'blk 185 "put into an external vector: through the alias alias$"'
check "a put that is no slot store"                   'blk 200 "mutating call on loop-external state: put$"'

# The program says the same on one worker and on all.
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
if "$STAGE" -o "$T/fixture$SCALY_EXE" "$FIX" >"$T/build" 2>&1; then
  one=$(SCALY_WORKERS=1 "$T/fixture$SCALY_EXE" 2>&1); r1=$?
  all=$("$T/fixture$SCALY_EXE" 2>&1); r2=$?
  check "runs (rc $r1, $r2)" '[ $r1 -eq 0 ] && [ $r2 -eq 0 ]'
  check "one worker and all print the same" '[ "$one" = "$all" ]'
  check "the line it prints" '[ "$one" = "0 4 17 1 6 64" ]'
else
  check "the fixture builds" 'false'
fi

echo "task-plan: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
