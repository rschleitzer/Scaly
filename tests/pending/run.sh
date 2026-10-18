#!/bin/bash
# tests/pending/run.sh — the pending-type experiment (SCALYC_PENDING=1,
# Planner.pending_rewrite#): operator precedence as a protocol written in
# Scaly (protocol.scaly), folded away before planning.
#   value   use.scaly (operators) answers what direct.scaly (the calls written
#           out by hand) answers
#   ir      the IR of protocol + use.scaly equals, byte for byte past the
#           module's name, the IR of a program with NO protocol and the calls
#           written out -- nothing of the protocol reaches the plan
#   off     without the switch the operators are not answered by the protocol
#   driver  the operation driver (Planner.operation#) reads a right side that
#           holds a call to its precedence boundary (precedence_call.scaly),
#           and a grouping with a member hop as what it groups
#           (grouping_hop.scaly), and an operator a record does not
#           declare as rc 4 (undeclared_operator.scaly)
# Not in the bar: an experiment. Usage: tests/pending/run.sh [scalyc]
set -u
cd "$(dirname "$0")/../.."
BIN=${1:-/tmp/scalyc_stage2}
TOOL="$(tools/scaly-of.sh "$BIN")"
W=$(mktemp -d)
trap 'rm -rf "$W"' EXIT
pass=0; fail=0
ok()  { pass=$((pass+1)); }
bad() { fail=$((fail+1)); echo "FAIL $1"; }

cat tests/pending/protocol.scaly tests/pending/use.scaly > "$W/use.scaly"
{ sed -n '/^define Num/,/^}/p' tests/pending/protocol.scaly | grep -v -E '^    operator|^        (Addend|Factor)\[|^$|; the raw'
  cat tests/pending/direct.scaly; } > "$W/plain.scaly"

want="14 20"
got=$(SCALYC_PENDING=1 "$TOOL" run "$W/use.scaly" 2>&1 | head -1)
[ "$got" = "$want" ] && ok || bad "value: use answered '$got', expected '$want'"
got=$("$TOOL" run "$W/plain.scaly" 2>&1 | head -1)
[ "$got" = "$want" ] && ok || bad "value: direct answered '$got', expected '$want'"

SCALYC_PENDING=1 "$BIN" -S -o "$W/use.ll" "$W/use.scaly" > "$W/use.log" 2>&1 \
  && "$BIN" -S -o "$W/plain.ll" "$W/plain.scaly" > "$W/plain.log" 2>&1 \
  && cmp -s <(sed '1,2d' "$W/use.ll") <(sed '1,2d' "$W/plain.ll") \
  && ok || bad "ir: the folded program differs from the hand-written one"

"$BIN" -S -o "$W/off.ll" "$W/use.scaly" > "$W/off.log" 2>&1 && bad "off: the protocol answered without the switch" || ok

# the driver (Planner.operation#): a right side with a call in it ends at the
# next weaker operator -- the old collapse runs past it
got=$(SCALYC_PENDING=1 "$TOOL" run tests/pending/precedence_call.scaly 2>&1 | tr '\n' ' ')
[ "$got" = "T1 F2 " ] && ok || bad "driver: precedence_call answered '$got', expected 'T1 F2'"

# the driver: `(a)` is `a` -- a grouping with a member hop works on the
# grouped value, not on a copy
got=$(SCALYC_PENDING=1 "$TOOL" run tests/pending/grouping_hop.scaly 2>&1 | tr '\n' ' ')
[ "$got" = "PASS " ] && ok || bad "driver: grouping_hop answered '$got', expected 'PASS'"

# the driver: an operator a record does not declare is rc 4 at its line --
# five errors, none for a field's value or for the identity test
SCALYC_PENDING=1 "$BIN" -S -o "$W/undeclared.ll" tests/pending/undeclared_operator.scaly > "$W/undeclared.log" 2>&1
rc=$?
lines=$(grep -c 'error: .*declares no' "$W/undeclared.log")
lines14=$(grep -c ':14:5: error: Pt declares no operator' "$W/undeclared.log")
lines23=$(grep -c ":23:5: error: no operator '+' takes int and Pt" "$W/undeclared.log")
[ $rc = 4 ] && [ "$lines" = 5 ] && [ "$lines14" = 1 ] && [ "$lines23" = 1 ] && ok || bad "driver: undeclared_operator rc $rc, $lines errors (expected rc 4, 5)"

echo "pending: $pass PASS, $fail FAIL"
[ $fail = 0 ]
