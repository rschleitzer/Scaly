#!/bin/bash
# x86_64-apple-darwin: EMIT, LINK and RUN — the one LP64 target that never had
# a runner, verified on an Apple Silicon host.
#
# `tests/target/run.sh` checks object EMISSION for all four LP64 targets plus
# the two Windows COFF ones, which is a claim about the object format and says
# nothing about whether the code RUNS. seed/README.md records the gap honestly:
# `x86_64-apple-darwin` is marked best-effort, its two halves each covered
# elsewhere — the x86_64 System-V codegen by `x86_64-linux-gnu`, Mach-O/darwin
# by `arm64-apple-darwin`. ★"Both halves are covered" is an argument, not a
# measurement; this script is the measurement.
#
# It works because three facts line up, and it is worth naming all three
# because losing any one of them takes the gate with it:
#   1. the committed seed is TRIPLE-LESS, so `--target` retargets it;
#   2. macOS ships a UNIVERSAL SDK, so `clang -arch x86_64` links here;
#   3. Rosetta 2 executes the result.
# Nothing here needs Intel hardware. On a machine where any of the three is
# missing the script SKIPS rather than fails — a gate that cannot run must not
# look like a gate that ran.
#
# The C shims are selected by `uname -m`, i.e. the HOST, which is why they take
# a `SCALY_ARCH` override (tools/{fcontext,eio,ctime}.sh); without it the
# archive would carry arm64 objects and the link would fail.
#
# Usage: tests/target/rosetta.sh [compiler]
#   compiler   default scalyc/build/scalyc (needs the -O2 build, not stage2)
set -u
cd "$(dirname "$0")/../.."

SCALYC="${1:-scalyc/build/scalyc}"
TRIPLE=x86_64-apple-darwin
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

skip() { echo "rosetta: SKIP — $1"; exit 0; }

[ "$(uname -s)" = "Darwin" ] || skip "not a Darwin host (uname -s = $(uname -s))"
[ -x "$SCALYC" ]             || skip "no compiler at $SCALYC (run ./build.sh)"

# Rosetta 2 is what RUNS the result. Probe it with a three-line C program
# rather than by asking for a version: the question is whether an x86_64
# Mach-O executes here, and only running one answers it.
echo 'int main(void){return 42;}' > "$WORK/probe.c"
if ! clang -arch x86_64 "$WORK/probe.c" -o "$WORK/probe" 2>/dev/null; then
    skip "clang cannot target x86_64 (no universal SDK)"
fi
"$WORK/probe"; [ $? = 42 ] || skip "x86_64 binaries do not execute here (Rosetta 2 absent?)"

# ---- the x86_64 runtime archive -------------------------------------------
# Plain `-c`, never `-c -O2`: with no main anchor, GlobalDCE deletes every
# linkonce_odr body and the object comes out empty. The
# whole-program opt route is for the shipped archive, not for this check.
echo "rosetta: building the $TRIPLE runtime archive"
"$SCALYC" --target "$TRIPLE" -c --no-prelude --no-tests \
          -o "$WORK/libscaly.o" packages/scaly/0.1.1/scaly.scaly \
    || { echo "rosetta: FAIL — runtime cross-emit"; exit 1; }
for s in fcontext eio ctime panic; do
    SCALY_ARCH=x86_64 "tools/$s.sh" "$WORK/$s.o" \
        || { echo "rosetta: FAIL — $s.sh with SCALY_ARCH=x86_64"; exit 1; }
done
ar rcs "$WORK/libscaly.a" "$WORK/libscaly.o" "$WORK/fcontext.o" \
       "$WORK/eio.o" "$WORK/ctime.o" "$WORK/panic.o" || { echo "rosetta: FAIL — ar"; exit 1; }

# ---- the AOT corpus, as ground truth --------------------------------------
# Compare against each test's `; Expected:` comment, exactly as
# tools/aot_corpus.sh does: the comment is absolute ground truth, so a
# miscompile is caught even when every stage shares it. Tests without one are
# exit-code/value tests whose reference binary is host-native, and `link: llvm`
# tests need the LLVM libraries for THIS arch — both are counted as skipped
# rather than silently dropped.
pass=0; fail=0; skipped=0; failed=""
for f in tests/aot/*.scaly; do
    t=$(basename "$f" .scaly)
    expected=$(sed -n 's/^; Expected: //p' "$f")
    if [ -z "$expected" ] || grep -q '^; link: llvm' "$f"; then
        skipped=$((skipped+1)); continue
    fi
    if ! "$SCALYC" --target "$TRIPLE" -c -o "$WORK/$t.o" "$f" >/dev/null 2>&1; then
        fail=$((fail+1)); failed="$failed $t(emit)"; continue
    fi
    if ! clang -arch x86_64 "$WORK/$t.o" "$WORK/libscaly.a" -lm -o "$WORK/$t" 2>/dev/null; then
        fail=$((fail+1)); failed="$failed $t(link)"; continue
    fi
    out=$("$WORK/$t" 2>/dev/null); rc=$?
    if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
        pass=$((pass+1))
    else
        fail=$((fail+1)); failed="$failed $t"
    fi
done

# ---- SIMD phase 6 on x86: the lookup/join fixture as an x86_64 program, with
# SSSE3 (pshufb behind the saturating add), with AVX2 (vpshufb over 256 bits)
# and on the x86-64 baseline (the compare-select form). Rosetta 2 runs SSE up
# to 4.2 everywhere and AVX2 on recent macOS (26 measured); an AVX2 binary
# that dies of SIGILL means the translator, not the code, and is skipped.
simd_fail=""
for cpu in -mcpu=core2 -mcpu=x86-64-v3 ""; do
    if ! "$SCALYC" --target "$TRIPLE" $cpu -c -o "$WORK/simd.o" tests/regress/simd_lookup_join.scaly >/dev/null 2>&1 \
       || ! clang -arch x86_64 "$WORK/simd.o" "$WORK/libscaly.a" -lm -o "$WORK/simd" 2>/dev/null; then
        simd_fail="$simd_fail simd_lookup_join(${cpu:-baseline}, build)"
        continue
    fi
    simd_out=$("$WORK/simd" 2>/dev/null); simd_rc=$?
    if [ "$cpu" = -mcpu=x86-64-v3 ] && [ "$simd_rc" = 132 ]; then
        echo "rosetta: SKIP simd_lookup_join(x86-64-v3) -- this Rosetta has no AVX2"
        continue
    fi
    [ "$simd_out" = "PASS" ] || simd_fail="$simd_fail simd_lookup_join(${cpu:-baseline})"
done
if [ -n "$simd_fail" ]; then
    fail=$((fail+1)); failed="$failed$simd_fail"
else
    pass=$((pass+1))
fi

# A corpus that shrank to nothing would report a proud green, so the count is
# asserted rather than printed — the same reason tools/aot_corpus.sh had to
# stop ending on `echo`.
if [ "$pass" -lt 40 ]; then
    echo "rosetta: FAIL — only $pass tests ran; the corpus or the filter is broken"
    exit 1
fi

echo "rosetta: $TRIPLE  PASS=$pass  FAIL=$fail  (skipped $skipped)"
if [ "$fail" != 0 ]; then
    echo "rosetta: FAILED:$failed"
    exit 1
fi
echo "rosetta: PASS — $TRIPLE emits, links and RUNS ($pass AOT tests under Rosetta 2)"
