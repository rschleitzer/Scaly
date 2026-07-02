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
CC=${1:-/tmp/scalyc_stage2}
pass=0; fail=0; failures=()

for f in tests/escape/neg_*.scaly; do
  [ -e "$f" ] || continue
  t=$(basename "$f" .scaly)
  out=$( ( ulimit -s 65520; "$CC" -c --no-prelude -o /tmp/esc_$t.o "$f" ) 2>&1 ); rc=$?
  if [ $rc -ne 0 ] && echo "$out" | grep -q "escapes"; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: expected rejection, got rc=$rc")
  fi
done

for f in tests/escape/pos_*.scaly; do
  [ -e "$f" ] || continue
  t=$(basename "$f" .scaly)
  out=$( ( ulimit -s 65520; "$CC" -c --no-prelude -o /tmp/esc_$t.o "$f" ) 2>&1 ); rc=$?
  if [ $rc -eq 0 ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: expected compile, got rc=$rc: $out")
  fi
done

# Option B (2026-07-02h): container get-ref suite — needs the PRELUDE (real
# Vector.get/get_ptr), unlike the no-prelude neg_*/pos_* sets above.
# negp_* : a frame-local-page holder's interior ref escapes -> MUST reject.
# posp_* : promoted / sigiled / frame-only-use holder refs -> MUST compile.
for f in tests/escape/negp_*.scaly; do
  [ -e "$f" ] || continue
  t=$(basename "$f" .scaly)
  out=$( ( ulimit -s 65520; "$CC" -c -o /tmp/esc_$t.o "$f" ) 2>&1 ); rc=$?
  if [ $rc -ne 0 ] && echo "$out" | grep -q "escapes"; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: expected rejection, got rc=$rc")
  fi
done

for f in tests/escape/posp_*.scaly; do
  [ -e "$f" ] || continue
  t=$(basename "$f" .scaly)
  out=$( ( ulimit -s 65520; "$CC" -c -o /tmp/esc_$t.o "$f" ) 2>&1 ); rc=$?
  if [ $rc -eq 0 ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: expected compile, got rc=$rc: $out")
  fi
done

# Multi-file diagnostic LOCATION: the escape is in mfloc_helper.scaly (a
# sub-module of mfloc_main.scaly). The diagnostic must name the HELPER file at
# its real line (13), not the entry file. Guards per-diagnostic file tracking.
out=$( ( ulimit -s 65520; "$CC" -c --no-prelude -o /tmp/esc_mfloc.o tests/escape/mfloc_main.scaly ) 2>&1 ); rc=$?
if [ $rc -ne 0 ] && echo "$out" | grep -q "mfloc_helper.scaly:13:.*escapes"; then
  pass=$((pass+1))
else
  fail=$((fail+1)); failures+=("mfloc: expected mfloc_helper.scaly:13 escape, got rc=$rc: $out")
fi

# Multi-file Vector-3 (pass-to-storing-callee) diagnostic LOCATION: the escape
# lives in mfloc_call_helper.scaly's leak_call (c.stash(&bx), line 23). The
# interprocedural check is a post-pass that runs after the current file is
# restored, so it must name the HELPER file via PlannedFunction.file, not the
# entry file. Guards per-function file tracking.
out=$( ( ulimit -s 65520; "$CC" -c --no-prelude -o /tmp/esc_mfloc_call.o tests/escape/mfloc_call_main.scaly ) 2>&1 ); rc=$?
if [ $rc -ne 0 ] && echo "$out" | grep -q "mfloc_call_helper.scaly:23:.*escapes"; then
  pass=$((pass+1))
else
  fail=$((fail+1)); failures+=("mfloc_call: expected mfloc_call_helper.scaly:23 escape, got rc=$rc: $out")
fi

echo "escape: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
