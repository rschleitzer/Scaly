#!/bin/bash
# Self-hosted literate-test suite — runs the generated semantic tests in
# tests/selfhosted/ THROUGH THE SELF-HOSTED IN-PROCESS JIT (scalyc --jit).
#
# Value tests ("; mode: run") JIT-compile their synthesized @main and must
# print PASS; compile-only tests ("; mode: compile": string/char/json/float —
# planner-limited, never executable in any backend) only have to plan
# (--plan --no-prelude). Ground truth is the test's "; expect:" header (from
# the .sgm source), self-checked inside each program.
#
# This replaced the old AOT (compile->clang-link->run) harness: the semantic
# coverage is identical (same emit()), the JIT path is ~14x faster (no per-test
# link + no macOS first-exec validation), and the AOT object/link pipeline
# stays covered by tests/aot (aot_corpus.sh), tests/regress, seed.sh and the
# bootstrap. --jit is self-hosted-only (the C++ stage-0 has its own --test for
# these), so this defaults to the self-hosted stage-2 binary.
#
# Usage: tests/selfhosted/run.sh [stage-binary] [name-filter]
#   tests/selfhosted/run.sh                       # /tmp/scalyc_stage2
#   tests/selfhosted/run.sh /tmp/scalyc_stage2 choose
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
FILTER=$2
TIMEOUT_SECS=${TIMEOUT_SECS:-30}

pass=0; fail=0; skip=0
failures=()

run_with_stack() {
  # The JIT self-compiles the prelude into one module; keep the bootstrap
  # stack limit so the planner's deep recursion doesn't overflow.
  ( ulimit -s 65520 2>/dev/null; perl -e "alarm $TIMEOUT_SECS; exec @ARGV" -- "$@" )
}

for f in tests/selfhosted/*.scaly; do
  t=$(basename "$f" .scaly)
  if [ -n "$FILTER" ]; then case $t in *$FILTER*) ;; *) continue;; esac; fi
  mode=$(sed -n 's/^; mode: //p' "$f")

  if [ "$mode" = "compile" ]; then
    # Compile-only tests (string/char/json/float) are not executable in any
    # backend (planner-limited) — plan them with no prelude.
    run_with_stack "$STAGE" --plan --no-prelude "$f" >/dev/null 2>&1
    rc=$?
    if [ $rc -eq 0 ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$t PLAN rc=$rc"); fi
    continue
  fi

  # The value tests need the in-process JIT, which the Windows box does not
  # have (tests/platform.sh): they are counted, by name, as SKIP there.
  if ! scaly_jit_available; then skip=$((skip+1)); continue; fi
  out=$(run_with_stack "$STAGE" --jit "$f" 2>/dev/null)
  rc=$?
  if [ $rc -eq 0 ] && [ "$out" = "PASS" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t JIT rc=$rc out='$out'")
  fi
done

skipnote=""; [ "$skip" -gt 0 ] && skipnote=", $skip SKIP (JIT unavailable on Windows)"
echo "selfhosted: $pass PASS, $fail FAIL$skipnote ($STAGE)"
for line in "${failures[@]}"; do echo "  FAIL: $line"; done
[ $fail -eq 0 ]
