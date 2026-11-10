#!/bin/bash
# tests/gpu/run.sh — the gate for `scalyc --gpu` and `--gpu-msl`
# (packages/scalyc/<version>/scalyc/compiler/GpuMsl.scaly, the emitter's
# emit_gpu_for, the standard library's gpu/gpu_darwin.c).
#
# A parallel `for` that is a kernel is handed to the graphics card, and the
# card must compute what the processor computes, BIT FOR BIT. This gate pins
#   (1) the source the printer writes, over the kernel shapes of
#       tests/gpu-plan/fixture.scaly, and what it refuses to write;
#   (2) where the emitter asks the card at all: with --gpu, for an Apple
#       target, and never in code that runs inside the compiler;
#   (3) on a machine with a card: every program here built with --gpu prints
#       the same with the card as with SCALY_GPU=off, the card demonstrably
#       ran (its trace), a round the processor would stop at stops the card's
#       run, and a program the processor traps in traps the same way.
# (3) is SKIPPED by name where there is no Metal device.
#
# A fixture's head says what its trace must hold:
#   ; cards: N       runs the card finished
#   ; declines: N    runs it gave back because a round stopped
#   ; no-rounds: N   loops of no round, which never reach it
#   ; expect-rc: N   the program's exit code, with and without the card
#
# Usage: tests/gpu/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
T=$(mktemp -d "${TMPDIR:-/tmp}/scaly_gpu.XXXXXX") || exit 1
trap 'rm -rf "${T:?}"' EXIT
pass=0; fail=0; skip=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }

# ---- (1) the printer ------------------------------------------------------
out=$("$STAGE" --gpu-msl tests/gpu-plan/fixture.scaly 2>&1); rc=$?
check "--gpu-msl rc=0 (got $rc)" '[ $rc -eq 0 ]'
check "the kernels' source matches fixture.msl" 'diff <(printf "%s\n" "$out") tests/gpu/fixture.msl >/dev/null'
n=$(printf '%s\n' "$out" | grep -c '^gpu-msl: .*: kernel$')
check "six kernels written (got $n)" '[ "$n" = "6" ]'
check "every kernel says safe math and no contraction" '[ "$(printf "%s\n" "$out" | grep -c "^#pragma METAL fp math_mode(safe)$")" = "6" ] && [ "$(printf "%s\n" "$out" | grep -c "^#pragma METAL fp contract(off)$")" = "6" ]'
check "a loop that is no kernel keeps its reason" 'printf "%s\n" "$out" | grep -q "fixture.scaly:91:1: no kernel: down is recursive$"'
ref=$("$STAGE" --gpu-msl tests/gpu/refused.scaly 2>&1)
check "unsigned to float is refused" 'printf "%s\n" "$ref" | grep -q "refused.scaly:8:5: refused: a cast from uint to float$"'
check "float to unsigned is refused" 'printf "%s\n" "$ref" | grep -q "refused.scaly:14:5: refused: a cast from float to uint$"'
# the parentheses around a number are a grouping whatever page the planner
# gives them: `let base (src[i] as f32) * scale` is a kernel's line
check "a parenthesised cast at the head of a binding is a kernel" '"$STAGE" --gpu-msl tests/gpu/shapes.scaly 2>&1 | grep -q "shapes.scaly:48:5: kernel$"'
check "--gpu-plan is not touched" '! "$STAGE" --gpu-plan tests/gpu-plan/fixture.scaly 2>&1 | grep -q "^gpu-msl:"'

# ---- (2) where the emitter asks the card ----------------------------------
ir() { "$STAGE" "$@" -S -o "$T/ir.ll" tests/gpu/shapes.scaly >/dev/null 2>&1 && grep -c 'scaly_gpu_run' "$T/ir.ll"; }
check "without --gpu no call of the host"        '[ "$(ir --target arm64-apple-macosx14.0)" = "0" ]'
check "with --gpu, an Apple target: the host"    '[ "$(ir --gpu --target arm64-apple-macosx14.0)" != "0" ]'
check "with --gpu, another target: no host"      '[ "$(ir --gpu --target x86_64-unknown-linux-gnu)" = "0" ]'
"$STAGE" --target arm64-apple-macosx14.0 -S -o "$T/plain.ll" tests/gpu/refused.scaly >/dev/null 2>&1
"$STAGE" --gpu --target arm64-apple-macosx14.0 -S -o "$T/gpu.ll" tests/gpu/refused.scaly >/dev/null 2>&1
check "a program of refused kernels alone emits as without --gpu" 'cmp -s "$T/plain.ll" "$T/gpu.ll"'
jit=$("$STAGE" --gpu --jit tests/gpu/refused.scaly 2>&1); jrc=$?
plain=$("$STAGE" --jit tests/gpu/refused.scaly 2>&1)
check "--gpu --jit runs on the processor (rc $jrc)" '[ $jrc -eq 0 ] && [ "$jit" = "$plain" ]'
jit=$("$STAGE" --gpu --jit tests/gpu/shapes.scaly 2>&1); jrc=$?
plain=$("$STAGE" --jit tests/gpu/shapes.scaly 2>&1)
check "--gpu --jit of kernels runs on the processor (rc $jrc)" '[ $jrc -eq 0 ] && [ "$jit" = "$plain" ]'

# ---- (3) the card against the processor -----------------------------------
have_card=0
if [ "$(uname -s)" = "Darwin" ] && "$STAGE" --gpu -o "$T/probe" tests/gpu/nested.scaly >/dev/null 2>&1; then
    SCALY_GPU_TRACE=1 "$T/probe" >/dev/null 2>"$T/probe.trace"
    grep -q '^gpu: card ' "$T/probe.trace" && have_card=1
fi
if [ $have_card -eq 0 ]; then
    skip=1
else
    for src in tests/gpu/*.scaly; do
        name=$(basename "$src" .scaly)
        want_cards=$(sed -n 's/^; cards: //p' "$src"); want_cards=${want_cards:-0}
        want_declines=$(sed -n 's/^; declines: //p' "$src"); want_declines=${want_declines:-0}
        want_empty=$(sed -n 's/^; no-rounds: //p' "$src"); want_empty=${want_empty:-0}
        want_rc=$(sed -n 's/^; expect-rc: //p' "$src"); want_rc=${want_rc:-0}
        if ! "$STAGE" --gpu -o "$T/$name" "$src" >"$T/$name.build" 2>&1; then
            check "$name builds with --gpu" false
            continue
        fi
        SCALY_GPU=off "$T/$name" >"$T/$name.cpu" 2>"$T/$name.cpu.err"; cpu_rc=$?
        SCALY_GPU_TRACE=1 SCALY_POISON=1 "$T/$name" >"$T/$name.gpu" 2>"$T/$name.trace"; gpu_rc=$?
        check "$name: exit code $want_rc on the processor (got $cpu_rc)" '[ $cpu_rc -eq $want_rc ]'
        check "$name: exit code $want_rc with the card (got $gpu_rc)" '[ $gpu_rc -eq $want_rc ]'
        check "$name: the card's output is the processor's" 'cmp -s "$T/$name.cpu" "$T/$name.gpu"'
        cards=$(grep -c '^gpu: card ' "$T/$name.trace")
        declines=$(grep -c '^gpu: processor (a round stopped)$' "$T/$name.trace")
        empty=$(grep -c '^gpu: processor (no round)$' "$T/$name.trace")
        other=$(grep '^gpu: ' "$T/$name.trace" | grep -v '^gpu: card ' | grep -v '(a round stopped)$' | grep -vc '(no round)$')
        check "$name: $want_cards runs on the card (got $cards)" '[ "$cards" = "$want_cards" ]'
        check "$name: $want_declines runs given back (got $declines)" '[ "$declines" = "$want_declines" ]'
        check "$name: $want_empty loops of no round (got $empty)" '[ "$empty" = "$want_empty" ]'
        check "$name: no kernel fails to compile or to run (got $other)" '[ "$other" = "0" ]'
    done
    # the processor alone, said: SCALY_GPU=off asks no card
    SCALY_GPU=off SCALY_GPU_TRACE=1 "$T/exact_float" >/dev/null 2>"$T/off.trace"
    check "SCALY_GPU=off keeps every loop on the processor" '! grep -q "^gpu: card " "$T/off.trace"'
    check "exact_float prints 4096 lines" '[ "$(wc -l < "$T/exact_float.gpu" | tr -d " ")" = "4096" ]'
fi

if [ $skip -eq 1 ]; then
    echo "gpu: $pass PASS, $fail FAIL, the card's runs SKIPPED (no Metal device)"
else
    echo "gpu: $pass PASS, $fail FAIL"
fi
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
