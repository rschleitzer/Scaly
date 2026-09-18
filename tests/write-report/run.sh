#!/bin/bash
# tests/write-report/run.sh — the gate for `scalyc --write-report`
# (packages/scalyc/0.1.0/scalyc/compiler/WriteCensus.scaly).
#
# The census is an INSTRUMENT, and an instrument that cannot fail is worse than
# none: this gate pins one site per write class, the transitive verdict through
# a callee's summary, and the NEGATIVE controls — functions that must come out
# `clean` (a reader, a construction on the caller page, a local container, an
# extern that only reads) and, in the frozen mode, the writes that must NOT be
# listed (the checker's own state, a fresh object, a function no entry reaches).
#
# Usage: tests/write-report/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
pass=0; fail=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }
has() { printf '%s\n' "$got" | grep -q -- "$1"; }

# --- the plain mode: every function's writes through its parameters --------
out=$("$STAGE" --plan --write-report tests/write-report/fixture.scaly 2>&1); rc=$?
check "fixture compiles rc=0 (got $rc)" '[ $rc -eq 0 ]'
got=$(printf '%s\n' "$out" | grep '^write-report: tests/write-report/fixture.scaly:')
check "w-field through the implicit this"      'has ": w-field bump_field writes this:Box$"'
check "bump_field is a direct writer"          'has ": fn bump_field writes this:Box direct$"'
check "w-call into Array.add"                  'has ": w-call push_item writes this:Box -> add$"'
check "push_item writes only transitively"     'has ": fn push_item writes this:Box transitive$"'
check "a sibling's summary propagates"         'has ": fn relay writes this:Box transitive$"'
check "a construction on ^this is a write"     'has ": a-construct make_on_this writes this:Box ^this$"'
check "w-deref through a pointer parameter"    'has ": w-deref poke writes p:pointer\[int\]$"'
check "w-global"                               'has ": w-global tick writes global counter$"'
check "w-extern: memset writes its first arg"  'has ": w-extern wipe writes p:pointer\[int\] -> memset$"'
check "a local alias of a parameter"           'has ": fn via_alias writes b:ref\[Box\] transitive$"'
check "a procedure is listed as proc"          'has ": proc set_v writes this:Box$"'
# negative controls
check "a reader is clean"                      'has ": fn peek clean$"'
check "a construction on the caller page is clean" 'has ": fn fresh clean$"'
check "a local container is clean"             'has ": fn local_only clean$"'
check "memcmp only reads: clean"               'has ": fn same clean$"'
check "an init writes its own new object: clean" 'has ": init init clean$"'
check "no call misaligned"                     'printf "%s\n" "$out" | head -1 | grep -q "misaligned calls 0"'

# --- the frozen mode: writes into shared data, by provenance -----------------
out=$(SCALYC_WRITE_FROZEN=TreeNode,Sym SCALYC_WRITE_ENTRY=Checker.check,Checker.check_fresh \
      "$STAGE" --plan --write-report tests/write-report/frozen.scaly 2>&1); rc=$?
check "frozen fixture compiles rc=0 (got $rc)" '[ $rc -eq 0 ]'
got=$(printf '%s\n' "$out" | grep '^write-report: tests/write-report/frozen.scaly:')
src=tests/write-report/frozen.scaly
line_of() { grep -n -- "$1" "$src" | head -1 | cut -d: -f1; }
check "a frozen parameter's field"             'has ":$(line_of "set n.flags: 7"):9: w-field check writes n:ref\[TreeNode\],frozen$"'
check "a container held in a frozen record"    'has ":$(line_of "s.decls.add(3)"):9: w-call check writes s:ref\[Sym\],frozen -> add$"'
check "a callee handed frozen data"            'has ":$(line_of "this.helper(n)"):9: w-call check writes n:ref\[TreeNode\],frozen -> helper$"'
check "a local alias inside the callee"        'has ":$(line_of "kids.add(4)"):9: w-call helper writes n:ref\[TreeNode\],frozen -> add$"'
check "a write under enter_shared is tagged"   'has ":$(line_of "set s.id: 9"):9: w-field check writes s:ref\[Sym\],frozen locked$"'
check "a field load is frozen"                 'has ":$(line_of "set_flags_of(r, 4)"):13: w-call check_fresh"'
check "a setter reached with frozen data"      'has ": w-field set_flags_of writes t:ref\[TreeNode\],frozen$"'
# negative controls
check "the checker's own state is not listed"  '! has ":$(line_of "set count: count + 1"):"'
check "its own container is not listed"        '! has ":$(line_of "seen.add(1)"):"'
check "a fresh object is not listed"           '! has ":$(line_of "set t.flags: 2"):"'
check "a setter handed a fresh object is not"  '! has ":$(line_of "set_flags_of(t, 3)"):"'
check "a function no entry reaches is not"     '! has ":$(line_of "set n.flags: 1"):"'
check "the head line reports the mode"         'printf "%s\n" "$out" | head -1 | grep -q "FROZEN: reachable"'

echo "write-report: $pass passed, $fail failed"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
