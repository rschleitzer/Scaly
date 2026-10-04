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
#           holds a call to its precedence boundary (precedence_call.scaly)
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

echo "pending: $pass PASS, $fail FAIL"
[ $fail = 0 ]
