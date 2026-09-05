#!/bin/bash
# tests/pointer-report/run.sh — the gate for `scalyc --pointer-report`
# (packages/scalyc/0.1.0/scalyc/compiler/PointerCensus.scaly).
#
# The census is an INSTRUMENT: a residue number nobody can refute is worth
# nothing, so this gate pins (1) the exact report over a fixture carrying one
# site per class, and (2) the negative controls — the shapes the census must
# NOT list: a borrowed `ref`, a `*opt` on `ref[T]?`, a `this` parameter, and
# a generic body's site, which sits in plan.functions once per instantiation
# and must come out ONCE.
#
# Usage: tests/pointer-report/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
FIX=tests/pointer-report/fixture.scaly
pass=0; fail=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }

out=$("$STAGE" --plan --pointer-report "$FIX" 2>&1); rc=$?
check "compile rc=0 (got $rc)" '[ $rc -eq 0 ]'
# The prelude is part of a program root and its own pointer sites are real;
# the fixture pins only its own lines.
got=$(printf '%s\n' "$out" | grep '^pointer-report:' | grep -v 'prelude.scaly')
check "report matches fixture.expected" 'diff <(printf "%s\n" "$got") tests/pointer-report/fixture.expected >/dev/null'

# Negative controls, each by the fixture line it must NOT name.
check "ref field (line 13) not listed"        '! printf "%s\n" "$got" | grep -q "fixture.scaly:13:"'
check "ref param r not listed"                 '! printf "%s\n" "$got" | grep -q "walk(r)"'
check "this parameter not listed"              '! printf "%s\n" "$got" | grep -q "(this)"'
check "*opt on ref[T]? (line 38) not listed"   '! printf "%s\n" "$got" | grep -q "fixture.scaly:38:"'
check "*opt is COUNTED"                        'printf "%s\n" "$got" | head -1 | grep -q "optional-ref derefs (not listed) 1"'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:47:9: deref")
check "generic body deref listed exactly once (got $n)" '[ "$n" = "1" ]'
n=$(printf '%s\n' "$got" | grep -c "field pointer\[T\] Box.p")
check "generic field listed exactly once (got $n)" '[ "$n" = "1" ]'
check "no body left unfiled"                   'printf "%s\n" "$got" | head -1 | grep -q "bodies unfiled 0"'
# A `*(p + i)` is ONE site, never a deref plus an arith.
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:26:")
check "*(raw + 2) is one deref-arith line (got $n)" '[ "$n" = "1" ]'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:29:")
check "set *(raw + 1) is one store-arith line (got $n)" '[ "$n" = "1" ]'
# A string literal's cstr wrap is the planner's cast, not the author's.
check "literal cstr wrap not a cast"           '! printf "%s\n" "$got" | grep -q "cast String"'

# Alias handles: the named handle is the accepted form; a site typed through
# `define ThingRef pointer[OpaqueThing]` is counted, never listed.
check "alias-handle sites not listed"          '! printf "%s\n" "$got" | grep -q "OpaqueThing"'
check "alias-handle sites are COUNTED (3)"     'printf "%s\n" "$got" | head -1 | grep -q "alias handles (not listed) 3"'

# Step 3: the callee's DECLARED parameter decides the class.
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:91:.*arg pointer\[u8\] -> takes_ptr(p)")
check "pointer into pointer param is arg -> callee(param) (got $n)" '[ "$n" = "1" ]'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:92:.*h-pointer-into-ref pointer\[Leaf\] -> takes_ref(r)")
check "pointer into ref[T]? param is a hazard (got $n)" '[ "$n" = "1" ]'
check "bare null into ref[T]? (line 93) not a site" '! printf "%s\n" "$got" | grep -q "fixture.scaly:93:"'
check "method receiver (line 90) not an arg site" '! printf "%s\n" "$got" | grep -q "fixture.scaly:90:"'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:95:.*h-slice-into-pointer Slice\[u8\] -> take(p)")
check "Slice into a generic method's pointer param is a hazard (got $n)" '[ "$n" = "1" ]'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:96:.*arg-extern pointer\[void\] -> memcpy(")
check "pointer into an extern is arg-extern, both args (got $n)" '[ "$n" = "2" ]'
n=$(printf "%s\n" "$got" | grep -c ": h-mixed-params ")
check "mixed ref/pointer parameter lists reported: walk + mixed (got $n)" '[ "$n" = "2" ]'
check "nothing unresolved in the fixture"  'printf "%s\n" "$got" | head -1 | grep -q "arg-unresolved 0"'

# Diagnostics gate ordering: the report must survive a root with a hard
# diagnostic (that is the half-converted root the census exists for).
bad=$(mktemp /tmp/ptrrep_bad.XXXXXX.scaly)
printf 'function f(p: pointer[int]) returns int\n{\n    let q *p\n    NoSuch(q)\n    q\n}\n' > "$bad"
bout=$("$STAGE" --plan --pointer-report "$bad" 2>&1); brc=$?
check "report printed before a hard diagnostic (rc=$brc)" '[ $brc -ne 0 ] && printf "%s\n" "$bout" | grep -q "^pointer-report: .*param pointer\[int\] f(p)"'
rm -f "$bad"

echo "pointer-report: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
