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
#       run, and a program the processor traps in traps the same way. The
#       card is FORCED there (SCALY_GPU=on): these loops are small.
#   (4) left alone, the processor's driver asks the card where it is about
#       to hand rounds to its other cores, and the host takes the loop where
#       the card pays (tests/gpu/gate/): a heavy loop goes to the card, a
#       light one stays, a small one is never asked, and the report says so.
#       These verdicts follow MEASURED time; the fixtures leave a factor of
#       ten on either side.
#   (5) the same through the other card, WebGPU (wgpu-native): the kernel in
#       WGSL (pinned as text beside Metal's), every program of (3) again, and
#       programs of its own (tests/gpu/webgpu/) -- whole numbers bit for bit,
#       floating point of a `relaxed` loop within a tolerance.
# (3) and (4) are SKIPPED by name where there is no Metal device, (5) where
# there is no wgpu-native.
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
check "a binding that widens is refused" 'printf "%s\n" "$ref" | grep -q "refused.scaly:9:5: refused: a int where a long stands$"'
# the parentheses around a number are a grouping, built nowhere: `let base
# (src[i] as f32) * scale` is a kernel's line (the planner gave them the
# page of a stored value, and the line read as a construction)
check "a parenthesised cast at the head of a binding is a kernel" '"$STAGE" --gpu-msl tests/gpu/shapes.scaly 2>&1 | grep -q "shapes.scaly:48:5: kernel$"'
check "--gpu-plan is not touched" '! "$STAGE" --gpu-plan tests/gpu-plan/fixture.scaly 2>&1 | grep -q "^gpu-msl:"'
# the same kernels in WGSL, for the loops a card under WebGPU may run: no
# 64-bit integer, and floating point only where the loop is relaxed
wout=$( { "$STAGE" --gpu-wgsl tests/gpu-plan/fixture.scaly; "$STAGE" --gpu-wgsl tests/gpu-plan/relaxed.scaly; } 2>&1)
check "the kernels' WGSL matches fixture.wgsl" 'diff <(printf "%s\n" "$wout") tests/gpu/fixture.wgsl >/dev/null'
n=$(printf '%s\n' "$wout" | grep -c '^gpu-wgsl: .*: kernel$')
check "seven kernels written in WGSL (got $n)" '[ "$n" = "7" ]'
check "floating point that is not relaxed is no kernel for webgpu" 'printf "%s\n" "$wout" | grep -q "relaxed.scaly:15:1: no kernel for webgpu: .*the loop is not relaxed$"'
check "a 64-bit integer is no kernel for webgpu" 'printf "%s\n" "$wout" | grep -q "relaxed.scaly:25:5: no kernel for webgpu: .*64-bit integer$"'
check "WGSL names no 64-bit type" '! printf "%s\n" "$wout" | grep -q "i64\|u64\|long"'

# ---- (2) where the emitter asks the card ----------------------------------
ir() { "$STAGE" "$@" -S -o "$T/ir.ll" tests/gpu/shapes.scaly >/dev/null 2>&1 && grep -c 'scaly_gpu_run' "$T/ir.ll"; }
check "without --gpu no call of the host"        '[ "$(ir --target arm64-apple-macosx14.0)" = "0" ]'
check "with --gpu, an Apple target: the host"    '[ "$(ir --gpu --target arm64-apple-macosx14.0)" != "0" ]'
check "with --gpu, another target: no host"      '[ "$(ir --gpu --target x86_64-unknown-linux-gnu)" = "0" ]'
# the card is asked at the loop's head only where the host says so, and at
# the two gates of the processor's driver, with what it measured
ir --gpu --target arm64-apple-macosx14.0 >/dev/null
check "the head asks the host whether the card is forced" 'grep -q "call i32 @scaly_gpu_forced()" "$T/ir.ll"'
check "a gate hands over what a round cost"               'grep -q "call i32 @scaly_gpu_run(.*i64 %parfor.p1k, i64 %parfor.i.cur, i64 %parfor.ncpu.chk" "$T/ir.ll"'
check "the head gate hands over the site's cost"          'grep -q "call i32 @scaly_gpu_run(.*i64 %parfor.sitep, i64 0, i64 %parfor.ncpu.chk" "$T/ir.ll"'
"$STAGE" --target arm64-apple-macosx14.0 -S -o "$T/plain.ll" tests/gpu/refused.scaly >/dev/null 2>&1
"$STAGE" --gpu --target arm64-apple-macosx14.0 -S -o "$T/gpu.ll" tests/gpu/refused.scaly >/dev/null 2>&1
check "a program of a refused kernel alone emits as without --gpu" 'cmp -s "$T/plain.ll" "$T/gpu.ll"'
jit=$("$STAGE" --gpu --jit tests/gpu/refused.scaly 2>&1); jrc=$?
plain=$("$STAGE" --jit tests/gpu/refused.scaly 2>&1)
check "--gpu --jit runs on the processor (rc $jrc)" '[ $jrc -eq 0 ] && [ "$jit" = "$plain" ]'
jit=$("$STAGE" --gpu --jit tests/gpu/shapes.scaly 2>&1); jrc=$?
plain=$("$STAGE" --jit tests/gpu/shapes.scaly 2>&1)
check "--gpu --jit of kernels runs on the processor (rc $jrc)" '[ $jrc -eq 0 ] && [ "$jit" = "$plain" ]'

# ---- (3) the card against the processor -----------------------------------
have_card=0
if [ "$(uname -s)" = "Darwin" ] && "$STAGE" --gpu -o "$T/probe" tests/gpu/nested.scaly >/dev/null 2>&1; then
    SCALY_GPU=on SCALY_GPU_TRACE=1 "$T/probe" >/dev/null 2>"$T/probe.trace"
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
        # SCALY_GPU=on: the card is tried at every loop's head, however small
        SCALY_GPU=on SCALY_GPU_TRACE=1 SCALY_POISON=1 "$T/$name" >"$T/$name.gpu" 2>"$T/$name.trace"; gpu_rc=$?
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

    # ---- (4) the gate: nobody says where to run ---------------------------
    # The same programs' small loops, left to themselves, never ask the card.
    SCALY_GPU_TRACE=1 "$T/shapes" >"$T/shapes.auto" 2>"$T/shapes.auto.trace"
    check "small loops left alone: no word from the host" '[ ! -s "$T/shapes.auto.trace" ]'
    check "small loops left alone: the same output" 'cmp -s "$T/shapes.cpu" "$T/shapes.auto"'
    for src in tests/gpu/gate/*.scaly; do
        name=gate_$(basename "$src" .scaly)
        min_cards=$(sed -n 's/^; cards-min: //p' "$src")
        exact_cards=$(sed -n 's/^; cards: //p' "$src")
        min_kept=$(sed -n 's/^; kept-min: //p' "$src"); min_kept=${min_kept:-0}
        if ! "$STAGE" --gpu -O2 -o "$T/$name" "$src" >"$T/$name.build" 2>&1; then
            check "$name builds with --gpu" false
            continue
        fi
        SCALY_GPU=off "$T/$name" >"$T/$name.cpu" 2>"$T/$name.cpu.err"; cpu_rc=$?
        SCALY_GPU_TRACE=1 SCALY_GPU_REPORT=1 "$T/$name" >"$T/$name.gpu" 2>"$T/$name.trace"; gpu_rc=$?
        check "$name: runs (rc $cpu_rc, $gpu_rc)" '[ $cpu_rc -eq 0 ] && [ $gpu_rc -eq 0 ]'
        check "$name: left to the gate, the processor's output" 'cmp -s "$T/$name.cpu" "$T/$name.gpu"'
        cards=$(grep -c '^gpu: card ' "$T/$name.trace")
        kept=$(grep -c '^gpu: processor (the card would not pay)$' "$T/$name.trace")
        if [ -n "$min_cards" ]; then
            check "$name: the card takes at least $min_cards runs (got $cards)" '[ "$cards" -ge "$min_cards" ]'
        fi
        if [ -n "$exact_cards" ]; then
            check "$name: $exact_cards runs on the card (got $cards)" '[ "$cards" = "$exact_cards" ]'
        fi
        check "$name: at least $min_kept runs kept on the processor (got $kept)" '[ "$kept" -ge "$min_kept" ]'
    done
    # the report: one line per loop that came to the host, with its place
    check "the report names the loop the card took"      'grep -q "^gpu: tests/gpu/gate/heavy.scaly:34: 1 on the card, 0 on the processor; the card .* ms for 3000 rounds" "$T/gate_heavy.trace"'
    check "the report names the loop the processor kept" 'grep -q "^gpu: tests/gpu/gate/light.scaly:31: 0 on the card, [1-9][0-9]* on the processor$" "$T/gate_light.trace"'
    check "a loop that never came to the gate is not in it" '! grep -q "light.scaly:10:" "$T/gate_light.trace"'
    # one worker: the card is weighed against one core, and pays the sooner
    SCALY_WORKERS=1 SCALY_GPU_TRACE=1 "$T/gate_heavy" >"$T/gate_heavy.one" 2>"$T/gate_heavy.one.trace"
    check "one worker: the card takes the heavy loop"    'grep -q "^gpu: card " "$T/gate_heavy.one.trace" && cmp -s "$T/gate_heavy.cpu" "$T/gate_heavy.one"'
    # off and on outrank the gate
    SCALY_GPU=off SCALY_GPU_TRACE=1 "$T/gate_heavy" >/dev/null 2>"$T/gate_heavy.off.trace"
    check "SCALY_GPU=off: the heavy loop on the processor" '[ ! -s "$T/gate_heavy.off.trace" ]'
    SCALY_GPU=on SCALY_GPU_TRACE=1 "$T/gate_light" >"$T/gate_light.on" 2>"$T/gate_light.on.trace"
    check "SCALY_GPU=on: every loop on the card, 3001 runs" '[ "$(grep -c "^gpu: card " "$T/gate_light.on.trace")" = "3001" ] && cmp -s "$T/gate_light.cpu" "$T/gate_light.on"'

    # ---- (5) the other card: WebGPU, through wgpu-native -------------------
    WGPU=""
    if command -v brew > /dev/null 2>&1 && p=$(brew --prefix wgpu-native 2> /dev/null) && [ -e "$p/lib/libwgpu_native.dylib" ]; then
        WGPU="$p"
    fi
    SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_TRACE=1 "$T/shapes" >/dev/null 2>"$T/wgpu.probe"
    if ! grep -q '^gpu: card ' "$T/wgpu.probe"; then
        skip_wgpu=1
    else
        # the host declares the library's structures itself: against its header
        if [ -n "$WGPU" ] && [ -e "$WGPU/include/webgpu.h" ]; then
            host=$(ls "$PWD"/packages/scaly/*/scaly/gpu/gpu_darwin.c | tail -1)
            check "the host's declarations are the header's" '${CLANG:-clang} -fsyntax-only -Wno-unused-function -I"$WGPU/include" -DSCALY_GPU_HOST="\"$host\"" tests/gpu/wgpu_abi.c 2>"$T/abi.err"'
        fi
        # every program of (3) again: what has a WGSL kernel runs on this
        # card, the rest on the processor, and the output is the processor's
        wcards=0
        for src in tests/gpu/*.scaly; do
            name=$(basename "$src" .scaly)
            want_rc=$(sed -n 's/^; expect-rc: //p' "$src"); want_rc=${want_rc:-0}
            SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_TRACE=1 "$T/$name" >"$T/$name.wgpu" 2>"$T/$name.wtrace"; wrc=$?
            check "$name under webgpu: exit code $want_rc (got $wrc)" '[ $wrc -eq $want_rc ]'
            check "$name under webgpu: the processor's output" 'cmp -s "$T/$name.cpu" "$T/$name.wgpu"'
            check "$name under webgpu: no kernel the card refuses" '! grep -q "^gpu: webgpu: \|(no kernel)$" "$T/$name.wtrace"'
            wcards=$((wcards + $(grep -c '^gpu: card ' "$T/$name.wtrace")))
        done
        check "those programs ran on this card too (got $wcards runs)" '[ "$wcards" -ge 20 ]'
        # its own programs: whole numbers bit for bit, relaxed floating
        # point within its tolerance
        for src in tests/gpu/webgpu/*.scaly; do
            name=wgpu_$(basename "$src" .scaly)
            min_cards=$(sed -n 's/^; cards-min: //p' "$src")
            exact_cards=$(sed -n 's/^; cards: //p' "$src")
            if ! "$STAGE" --gpu -o "$T/$name" "$src" >"$T/$name.build" 2>&1; then
                check "$name builds with --gpu" false
                continue
            fi
            SCALY_GPU=off "$T/$name" >"$T/$name.cpu" 2>/dev/null; cpu_rc=$?
            SCALY_GPU=on SCALY_GPU_TRACE=1 "$T/$name" >"$T/$name.metal" 2>/dev/null; metal_rc=$?
            SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_TRACE=1 "$T/$name" >"$T/$name.wgpu" 2>"$T/$name.wtrace"; wrc=$?
            check "$name: runs (rc $cpu_rc, $metal_rc, $wrc)" '[ $cpu_rc -eq 0 ] && [ $metal_rc -eq 0 ] && [ $wrc -eq 0 ]'
            check "$name: Metal prints what the processor prints" 'cmp -s "$T/$name.cpu" "$T/$name.metal"'
            check "$name: WebGPU prints what the processor prints" 'cmp -s "$T/$name.cpu" "$T/$name.wgpu"'
            cards=$(grep -c '^gpu: card ' "$T/$name.wtrace")
            if [ -n "$min_cards" ]; then
                check "$name: at least $min_cards runs under webgpu (got $cards)" '[ "$cards" -ge "$min_cards" ]'
            fi
            if [ -n "$exact_cards" ]; then
                check "$name: $exact_cards runs under webgpu (got $cards)" '[ "$cards" = "$exact_cards" ]'
            fi
            check "$name: no kernel the card refuses" '! grep -q "^gpu: webgpu: \|(no kernel)$" "$T/$name.wtrace"'
        done
        check "relaxed: nothing beyond the tolerance" '[ "$(cat "$T/wgpu_relaxed.wgpu")" = "0 beyond the tolerance, the counts within a thousandth: true" ]'
        # left to the gate, the heavy loop is not relaxed: it has no kernel
        # for this card and stays on the processor
        SCALY_GPU_BACKEND=webgpu SCALY_GPU_TRACE=1 "$T/gate_heavy" >"$T/gate_heavy.wgpu" 2>"$T/gate_heavy.wtrace"
        check "a loop that is not relaxed stays off this card" '[ ! -s "$T/gate_heavy.wtrace" ] && cmp -s "$T/gate_heavy.cpu" "$T/gate_heavy.wgpu"'
    fi
fi

if [ $skip -eq 1 ]; then
    echo "gpu: $pass PASS, $fail FAIL, the card's runs SKIPPED (no Metal device)"
elif [ "${skip_wgpu:-0}" -eq 1 ]; then
    echo "gpu: $pass PASS, $fail FAIL, WebGPU's runs SKIPPED (no wgpu-native: brew install wgpu-native)"
else
    echo "gpu: $pass PASS, $fail FAIL"
fi
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
