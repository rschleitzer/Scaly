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
check "head counts loops, parallel ones and kernels" 'printf "%s\n" "$got" | head -1 | grep -q "^gpu-plan: 16 for-loops, 14 parallel, 6 kernels$"'

# The kernels: both backends where the body computes in 32 bits.
check "i32 body, counter cast away: both backends"   'line 61 | grep -q "kernel (metal, webgpu): out a; in k; calls triple$"'
check "inner counted loop and a buffer read: both"   'line 66 | grep -q "kernel (metal, webgpu): out b; in a$"'
check "Slice parameter and a ref out parameter"      'line 49 | grep -q "kernel (metal, webgpu): out out; in k, xs$"'
check "program constant, a length, an if value"      'line 117 | grep -q "kernel (metal, webgpu): out q; in k, a.length, a; calls capped$"'
# Metal alone: a 64-bit integer, and floating point without `relaxed`.
check "64-bit integer: metal only"                   'line 76 | grep -q "kernel (metal): .*not webgpu: .*64-bit integer"'
check "floating point, not relaxed: metal only"      'line 81 | grep -q "kernel (metal): .*not webgpu: floating point"'

# No kernel, each for its own reason.
check "double"                                       'line 86 | grep -q "no kernel: e is a Vector\[double\]$"'
check "recursion"                                    'line 91 | grep -q "no kernel: down is recursive$"'
check "a String"                                     'line 101 | grep -q "no kernel: text_of allocates or throws$"'
check "a record"                                     'line 109 | grep -q "no kernel: builds a Pair$"'
check "a reduction into an outer name"               'line 125 | grep -q "no kernel: reduces into total$"'
check "a put beside the counter"                     'line 130 | grep -q "no kernel: a put at another index"'
check "reads what it writes"                         'line 134 | grep -q "no kernel: reads and writes m$"'
# A loop the task plan blocks is no kernel, with the task plan's reason.
check "a C function: blocked"                        'line 96 | grep -q "no kernel: blocked: extern call toupper$"'
check "a printing loop: blocked"                     'line 138 | grep -q "no kernel: blocked: "'
# An inner loop is a loop of its own and never a kernel beside its outer one.
check "the inner loop is reported for itself"        'line 69 | grep -q "no kernel: reduces into s$"'
n=$(printf '%s\n' "$got" | grep -c ': kernel (')
check "six kernels listed (got $n)" '[ "$n" = "6" ]'

# The task plan's report, which checked transcripts print, is not touched.
tp=$("$STAGE" --task-plan "$FIX" 2>&1)
check "--task-plan prints no gpu-plan line" '! printf "%s\n" "$tp" | grep -q "^gpu-plan:"'
check "--task-plan agrees on the parallel loops" 'printf "%s\n" "$tp" | head -1 | grep -q "16 for-loops, 14 parallelizable"'
both=$("$STAGE" --task-plan --gpu-plan "$FIX" 2>&1)
check "both reports together" '[ "$(printf "%s\n" "$both" | grep -c "^task-plan:")" = "17" ] && [ "$(printf "%s\n" "$both" | grep -c "^gpu-plan:")" = "17" ]'

echo "gpu-plan: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
