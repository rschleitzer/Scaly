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
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$(tools/scaly-of.sh "$STAGE")

# Per-test comment lines: "; Expected:" = exact stdout, optional
# "; ExpectedExit:" = exit code (default 0), optional "; ExpectedErr:" =
# substring that must appear on stderr (for crash-diagnostic tests),
# optional "; Timeout: <s>" = killed after that many seconds (a test whose
# failure mode is a hang; the kill's rc 142 is then the verdict).
# ★The fixture loop is tests/fiber/run.py since 2026-10-03 (its header has the
# markers, `; Isolated:` among them): every build and the programs that do not
# measure time run in parallel (FIBER_JOBS, default every core), the isolated
# ones alone afterwards. Verdicts, summary line and exit code are the loop's.
# One build first, alone: the builds run in parallel, and each would otherwise
# compile the runtime into an empty build cache at once.
"$SCALY" build tests/tool/hello.scaly -o "/tmp/fb_warm_$$$SCALY_EXE" > /dev/null 2>&1
rm -f "/tmp/fb_warm_$$$SCALY_EXE"
JOBS=${FIBER_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
PY=$(command -v python3 || command -v python) || { echo "fiber: no python3" >&2; exit 2; }
exec "$PY" tests/fiber/run.py "$SCALY" "$SCALY_EXE" "$(cygpath -ms /tmp 2>/dev/null || echo /tmp)" "$JOBS"
