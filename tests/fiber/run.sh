#!/bin/bash
# Fiber suite (self-scaling stage 1) — exercises the context-switch
# primitives (milestone 1.1) end to end: build with the given stage binary's
# `build` command (the runtime and its switcher -- the vendored AAPCS64 /
# SysV x86-64 / Win64 fcontext, chosen by file name -- come out of the build
# cache), run, and compare stdout to the test's "; Expected:" comment.
#
# Usage: tests/fiber/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1

# ---- platform (tests/platform.sh): binaries carry .exe on the Windows box.
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}

# Per-test comment lines: "; Expected:" = exact stdout, optional
# "; ExpectedExit:" = exit code (default 0), optional "; ExpectedErr:" =
# substring that must appear on stderr (for crash-diagnostic tests),
# optional "; Timeout: <s>" = killed after that many seconds (a test whose
# failure mode is a hang; the kill's rc 142 is then the verdict).
pass=0; fail=0; failures=()
for f in tests/fiber/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  want_rc=$(sed -n 's/^; ExpectedExit: //p' "$f"); want_rc=${want_rc:-0}
  want_err=$(sed -n 's/^; ExpectedErr: //p' "$f")
  bin=/tmp/fiber_$t$SCALY_EXE; rm -f "$bin"
  if ! "$STAGE" build "$f" -o "$bin" >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); continue
  fi
  limit=$(sed -n 's/^; Timeout: //p' "$f")
  if [ -n "$limit" ]; then
    out=$(perl -e 'alarm shift; exec @ARGV' "$limit" "$bin" 2>"/tmp/fiber_$t.err"); rc=$?
  else
    out=$("$bin" 2>"/tmp/fiber_$t.err"); rc=$?
  fi
  ok=1
  [ "$rc" = "$want_rc" ] || ok=0
  [ "$out" = "$expected" ] || ok=0
  # -F: the expected text is a LITERAL, not a pattern. Without it a message
  # containing brackets -- `Vector[]: index out of bounds` -- makes grep fail
  # with "brackets not balanced" and the test go red for the wrong reason.
  if [ -n "$want_err" ] && ! grep -qF "$want_err" "/tmp/fiber_$t.err"; then ok=0; fi
  if [ "$ok" = "1" ]; then
    pass=$((pass+1))
  else
    # the first stderr line too: a CI log shows only this summary, and a
    # test that exits through a failed check names the check on stderr
    # (forkjoin_emit failed once on Linux CI with nothing else to go on)
    err1=$(head -1 "/tmp/fiber_$t.err" 2>/dev/null)
    fail=$((fail+1)); failures+=("$t: rc=$rc '$out'${err1:+ stderr: '$err1'}")
  fi
done
echo "fiber: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
