#!/bin/bash
# Escape-checker negative/positive suite (RBMM blocker C).
#
# neg_*.scaly  : a reference into a Local ($) page escapes -> compiler MUST
#                reject (rc != 0) with an "escapes" diagnostic.
# pos_*.scaly  : a valid reference return (param / caller-page) -> MUST compile.
#
# Both the C++ stage-0 and a self-hosted stage binary must agree. Compile-only
# (-c --no-prelude); rejection happens during planning, before any link/run.
#
# Usage: tests/escape/run.sh [compiler]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
CC=${1:-$SCALY_STAGE_DEFAULT}
# ★The fixture loops are tests/escape/run.py since 2026-10-03 (its header has the
# groups and what each expects): the compiles run in parallel there, the summary
# line and the exit code are the loops'. The stack limit is raised HERE, once,
# and the compiles inherit it.
ulimit -s 65520 2>/dev/null
PY=$(command -v python3 || command -v python) || { echo "escape: no python3" >&2; exit 2; }
exec "$PY" tests/escape/run.py "$CC" "$(cd /tmp && pwd -W 2>/dev/null || echo /tmp)"
