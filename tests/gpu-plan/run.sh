#!/bin/bash
# tests/gpu-plan/run.sh — the gate for `scalyc --gpu-plan`
# (packages/scalyc/<version>/scalyc/compiler/GpuKernel.scaly).
#
# The report says which parallel `for` a graphics card could run, and for
# which backends. This gate pins (1) the exact report over a fixture carrying
# one loop per verdict, and (2) each verdict by its line, so that a changed
# wording shows as one failure and a changed DECISION as another: a loop the
# pass wrongly calls a kernel would later be emitted for a card that computes
# something else.
#
# Usage: tests/gpu-plan/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
FIX=tests/gpu-plan/fixture.scaly
pass=0; fail=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }
line() { printf '%s\n' "$got" | grep "fixture.scaly:$1:"; }

out=$("$STAGE" --gpu-plan "$FIX" 2>&1); rc=$?
check "report rc=0 (got $rc)" '[ $rc -eq 0 ]'
got=$(printf '%s\n' "$out" | grep '^gpu-plan:')
check "report matches fixture.expected" 'diff <(printf "%s\n" "$got") tests/gpu-plan/fixture.expected >/dev/null'
check "nothing but the report is printed" '[ "$out" = "$got" ]'
check "head counts loops, parallel ones and kernels" 'printf "%s\n" "$got" | head -1 | grep -q "^gpu-plan: 16 for-loops, 14 parallel, 9 kernels$"'

# The kernels: both backends where the body computes in 32 bits.
check "i32 body, counter cast away: both backends"   'line 61 | grep -q "kernel (metal, webgpu): out a; in k; calls triple$"'
check "inner counted loop and a buffer read: both"   'line 66 | grep -q "kernel (metal, webgpu): out b; in a$"'
check "Slice parameter and a ref out parameter"      'line 49 | grep -q "kernel (metal, webgpu): out out; in k, xs$"'
check "a record of numbers, built and read"           'line 109 | grep -q "kernel (metal, webgpu): out r; in k$"'
check "program constant, a length, an if value"      'line 117 | grep -q "kernel (metal, webgpu): out q; in k, a.length, a; calls capped$"'
# Metal alone: a 64-bit integer, and floating point without `relaxed`.
check "64-bit integer: metal only"                   'line 76 | grep -q "kernel (metal): .*not webgpu: .*64-bit integer"'
check "floating point, not relaxed: metal only"      'line 81 | grep -q "kernel (metal): .*not webgpu: floating point"'

# No kernel, each for its own reason.
check "double"                                       'line 86 | grep -q "no kernel: e is a Vector\[double\]$"'
check "recursion"                                    'line 91 | grep -q "no kernel: down is recursive$"'
check "a String"                                     'line 101 | grep -q "no kernel: in text_of: the result is a String$"'
# A reduction is a kernel: the card gives each round's part, the processor folds.
check "a reduction into an outer name"               'line 125 | grep -q "kernel (metal, webgpu): reduces total; in a$"'
check "a put beside the counter"                     'line 130 | grep -q "no kernel: a put at another index"'
check "reads what it writes"                         'line 134 | grep -q "no kernel: reads and writes m$"'
# A loop the task plan blocks is no kernel, with the task plan's reason.
check "a C function: blocked"                        'line 96 | grep -q "no kernel: blocked: extern call toupper$"'
check "a printing loop: blocked"                     'line 138 | grep -q "no kernel: blocked: "'
# An inner loop is a loop of its own and never a kernel beside its outer one.
check "the inner loop is reported for itself"        'line 69 | grep -q "kernel (metal, webgpu): reduces s; in i, a$"'
n=$(printf '%s\n' "$got" | grep -c ': kernel (')
check "nine kernels listed (got $n)" '[ "$n" = "9" ]'

# The task plan's report, which checked transcripts print, is not touched.
tp=$("$STAGE" --task-plan "$FIX" 2>&1)
check "--task-plan prints no gpu-plan line" '! printf "%s\n" "$tp" | grep -q "^gpu-plan:"'
check "--task-plan agrees on the parallel loops" 'printf "%s\n" "$tp" | head -1 | grep -q "16 for-loops, 14 parallelizable"'
both=$("$STAGE" --task-plan --gpu-plan "$FIX" 2>&1)
check "both reports together" '[ "$(printf "%s\n" "$both" | grep -c "^task-plan:")" = "17" ] && [ "$(printf "%s\n" "$both" | grep -c "^gpu-plan:")" = "17" ]'

# `relaxed`: floating point may go to webgpu; nothing else changes.
got=$("$STAGE" --gpu-plan tests/gpu-plan/relaxed.scaly 2>&1 | grep '^gpu-plan:')
line() { printf '%s\n' "$got" | grep "relaxed.scaly:$1:"; }
check "relaxed: six loops, six kernels"             'printf "%s\n" "$got" | head -1 | grep -q "^gpu-plan: 6 for-loops, 6 parallel, 6 kernels$"'
check "floating point, not relaxed: metal only"     'line 15 | grep -q "kernel (metal): .*not webgpu: floating point, and the loop is not relaxed$"'
check "relaxed before the loop: both backends"      'line 17 | grep -q "kernel (metal, webgpu): out b; calls half$"'
check "relaxed before a block: its loops, both"     'line 21 | grep -q "kernel (metal, webgpu): out c; calls half$"'
check "whole numbers in the block: as without"      'line 23 | grep -q "kernel (metal, webgpu): out d$"'
check "a 64-bit integer stays metal's, relaxed or not" 'line 25 | grep -q "kernel (metal): .*not webgpu: .*64-bit integer$"'
check "after the block the word no longer holds"    'line 28 | grep -q "kernel (metal): .*the loop is not relaxed$"'

# CardVectors (scaly/card.scaly): a buffer and an output like a Vector's, a put
# also at an offset every round shares; a loop naming one writes CardVectors
# alone and reduces nothing -- and under --gpu one that is no kernel is rc 4.
got=$("$STAGE" --gpu-plan tests/gpu-plan/card.scaly 2>&1 | grep '^gpu-plan:')
line() { printf '%s\n' "$got" | grep "card.scaly:$1:"; }
check "card: seven loops, four kernels"            'printf "%s\n" "$got" | head -1 | grep -q "^gpu-plan: 7 for-loops, 7 parallel, 4 kernels$"'
check "card: a put at the counter"                  'line 19 | grep -q "kernel (metal, webgpu): out b; in a$"'
check "card: a put at an offset rounds share"       'line 22 | grep -q "kernel (metal, webgpu): out cache; in at, a$"'
check "card: a vector in memory beside it"          'line 25 | grep -q "no kernel: writes a vector in memory beside a CardVector: plain$"'
check "card: a reduction beside it"                 'line 29 | grep -q "no kernel: reduces beside a CardVector: write the result into a CardVector of one element$"'
check "card: the reduction as one element"         'line 32 | grep -q "kernel (metal, webgpu): out one; in n, a$"'
check "card: the exponential in a relaxed loop"    'line 44 | grep -q "kernel (metal, webgpu): out e; in a$"'
check "card: and not in one that is not"          'line 47 | grep -q "no kernel: calls a function whose body is not here: expf$"'
cw=$("$STAGE" --gpu-wgsl tests/gpu-plan/card.scaly 2>&1)
check "card: the offset is checked, then written"   'printf "%s\n" "$cw" | grep -q "if (t2 >= arrayLength(&o0)) { F = 1u; } else { o0\[t2\] = t3; }"'
check "card: a high word of the offset faults"      'printf "%s\n" "$cw" | grep -q "if (u0_h != 0u) { F = 1u; }"'
check "card: a sum past 32 bits faults"             'printf "%s\n" "$cw" | grep -q "if (t2 < t0) { F = 1u; }"'
check "card: the card's own exponential"            'printf "%s\n" "$cw" | grep -q "= exp(t0);"'
check "card: metal holds no offset put yet"         '"$STAGE" --gpu-msl tests/gpu-plan/card.scaly 2>&1 | grep -q "card.scaly:22:.*refused: a put at an offset"'
T=$(mktemp -d)
rout=$("$STAGE" --gpu --target wasm32-wasip1 -c -o "$T/refused.o" tests/gpu-plan/card_refused.scaly 2>&1); rrc=$?
rm -rf "$T"
check "card: --gpu refuses a CardVector loop that is no kernel (rc $rrc)" '[ $rrc -eq 4 ]'
check "card: and says where and why"                'printf "%s\n" "$rout" | grep -q "card_refused.scaly:11: error: a loop over a CardVector must run on the card, and this one cannot: reduces beside a CardVector"'

echo "gpu-plan: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
