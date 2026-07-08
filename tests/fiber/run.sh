#!/bin/bash
# Fiber suite (self-scaling stage 1) — exercises the context-switch
# primitives (milestone 1.1) end to end: compile with the given stage binary,
# link against /tmp/libscaly.a (which carries fcontext.o, the vendored
# AAPCS64 / SysV x86-64 switcher assembled by tools/fcontext.sh), run, and
# compare stdout to the test's "; Expected:" comment.
#
# Usage: tests/fiber/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}

# The archive normally gains fcontext.o + eio.o when it is built
# (bootstrap.sh / build-from-seed.sh / install.sh). Top it up when an older
# archive predates the fiber/eio modules — both objects are self-contained,
# so adding them is always safe; a missing archive is left alone (the
# compile fails loudly anyway).
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^fcontext\.o$'; then
  tools/fcontext.sh /tmp/fcontext.o && ar rcs /tmp/libscaly.a /tmp/fcontext.o
fi
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^eio\.o$'; then
  tools/eio.sh /tmp/eio.o && ar rcs /tmp/libscaly.a /tmp/eio.o
fi

pass=0; fail=0; failures=()
for f in tests/fiber/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin=/tmp/fiber_$t; rm -f "$bin"
  if ! "$STAGE" -o "$bin" "$f" >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); continue
  fi
  out=$("$bin" 2>/dev/null); rc=$?
  if [ "$rc" = "0" ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: rc=$rc '$out'")
  fi
done
echo "fiber: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
