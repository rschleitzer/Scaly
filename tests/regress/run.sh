#!/bin/bash
# Self-hosted regression suite — fixes NOT present in the frozen C++ stage-0.
#
# Step-C/D-era corrections (s192+) sometimes make the self-hosted compiler
# strictly MORE correct than stage-0 (e.g. use-aliased free-function call
# mangling: stage-0 silently drops the call). Such cases can't live in the
# stage-0-referenced AOT corpus or the stage-0-baselined selfhosted suite, so
# they run only against a self-hosted stage binary. Each test compiles+runs
# and must print PASS.
#
# Usage: tests/regress/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$(tools/scaly-of.sh "$STAGE")
# Every fixture is built with the stage's `build` command, its runtime out of
# the build cache. Build one program first, alone: the fixtures run in parallel,
# and each would otherwise compile the runtime into an empty cache at once.
"$SCALY" build tests/tool/hello.scaly -o "/tmp/rt_warm_$$$SCALY_EXE" > /dev/null 2>&1
rm -f "/tmp/rt_warm_$$$SCALY_EXE"
# ★The fixture loop is tests/regress/run.py since 2026-10-03 (its header has
# the branches and their markers): `xargs -P bash -c run_one` started a bash and
# some ten processes per fixture around the one compile and the one run, and Git
# Bash emulates every fork -- on the Windows box the median fixture spent 1.3 s
# in that set-up against 0.7 s compiling (tests/win32/WINDOWS-BOX.md §8). The
# verdicts, the summary line and the exit code are the loop's. Fixtures run in
# parallel (REGRESS_JOBS, default every core); each owns its binary rt_<name>
# in the temp directory, and every compile links from an object named by its
# PID, so concurrent fixtures share nothing.
LLVM_LINK=()
[ -n "${LLVM_LIBDIR:-}" ] || source tools/llvm-env.sh > /dev/null
[ -n "${LLVM_LIBDIR:-}" ] && LLVM_LINK=(-L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME")
JOBS=${REGRESS_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
PY=$(command -v python3 || command -v python) || { echo "regress: no python3" >&2; exit 2; }
exec "$PY" tests/regress/run.py "$STAGE" "$SCALY" "$SCALY_EXE" "$(cd /tmp && pwd -W 2>/dev/null || echo /tmp)" "$JOBS" ${LLVM_LINK[@]+"${LLVM_LINK[@]}"}
