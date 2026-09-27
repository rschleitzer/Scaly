#!/bin/bash
# Fiber suite (self-scaling stage 1) — exercises the context-switch
# primitives (milestone 1.1) end to end: compile with the given stage binary,
# link against /tmp/libscaly.a (which carries fcontext.o, the vendored
# AAPCS64 / SysV x86-64 switcher assembled by tools/fcontext.sh), run, and
# compare stdout to the test's "; Expected:" comment.
#
# Usage: tests/fiber/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1

# ---- platform (tests/platform.sh): on the Windows box the compiler links through
# clang + link.exe and resolves its OWN archive, $TMP/libscaly.lib, which Git Bash
# mounts as /tmp; binaries carry .exe; there is no /tmp/scalyc_stage2 and no `ar`
# top-up (tools/win-archive.sh builds the archive). POSIX is unchanged.
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
scaly_need_archive fiber "$STAGE" || exit 1

# The archive normally gains fcontext.o + eio.o + ctime.o when it is built
# (bootstrap.sh / build-from-seed.sh / install.sh). Top it up when an older
# archive predates the fiber/eio modules — both objects are self-contained,
# so adding them is always safe; a missing archive is left alone (the
# compile fails loudly anyway).
if [ "$SCALY_COFF" = 0 ] && [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^fcontext\.o$'; then
  tools/fcontext.sh /tmp/fcontext.o && ar rcs /tmp/libscaly.a /tmp/fcontext.o
fi
if [ "$SCALY_COFF" = 0 ] && [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^eio\.o$'; then
  tools/eio.sh /tmp/eio.o && ar rcs /tmp/libscaly.a /tmp/eio.o
fi
if [ "$SCALY_COFF" = 0 ] && [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^ctime\.o$'; then
  tools/ctime.sh /tmp/ctime.o && ar rcs /tmp/libscaly.a /tmp/ctime.o
  tools/panic.sh /tmp/panic.o && ar rcs /tmp/libscaly.a /tmp/panic.o
fi

# Per-test comment lines: "; Expected:" = exact stdout, optional
# "; ExpectedExit:" = exit code (default 0), optional "; ExpectedErr:" =
# substring that must appear on stderr (for crash-diagnostic tests).
pass=0; fail=0; failures=()
for f in tests/fiber/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  want_rc=$(sed -n 's/^; ExpectedExit: //p' "$f"); want_rc=${want_rc:-0}
  want_err=$(sed -n 's/^; ExpectedErr: //p' "$f")
  bin=/tmp/fiber_$t$SCALY_EXE; rm -f "$bin"
  if ! "$STAGE" -o "$bin" "$f" >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); continue
  fi
  out=$("$bin" 2>"/tmp/fiber_$t.err"); rc=$?
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
