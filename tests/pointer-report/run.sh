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
check "mixed ref/pointer parameter lists reported: walk + mixed + Source.init + fill_slots + provenance (got $n)" '[ "$n" = "5" ]'
check "nothing unresolved in the fixture"  'printf "%s\n" "$got" | head -1 | grep -q "arg-unresolved 0"'

# A construction's args carry no receiver while the initializer's input does:
# the buffer is read against `contents`, and a pointer into a ref init param
# is the hazard.
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:120:.*arg pointer\[u8\] -> Source.init(contents)")
check "construction arg aligned to init param (got $n)" '[ "$n" = "1" ]'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:121:.*h-pointer-into-ref pointer\[Origin\] -> Anchor.init(target)")
check "pointer into a ref init param is a hazard (got $n)" '[ "$n" = "1" ]'
check "no init argument read against the wrong slot" '! printf "%s\n" "$got" | grep -q "init(origin)"'

# A VALUE stored through a `pointer[ref[X]]` slot is the hazard; a reference
# rebinds the slot (2026-09-14) and the Option slot is an ordinary store.
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:1[23][0-9]:.*h-store-through-ref-slot pointer\[ref\[Leaf\]\]")
check "value stores through a pointer[ref[X]] slot are hazards, arith + plain (got $n)" '[ "$n" = "2" ]'
check "a reference stored into a pointer[ref[X]] slot rebinds: store-arith" 'printf "%s\n" "$got" | grep -q "fixture.scaly:131:5: store-arith pointer\[ref\[Leaf\]\] base=param(buf)$"'
n=$(printf '%s\n' "$got" | grep -c "fixture.scaly:13[0-9]:.*store-arith pointer\[Option\[ref\[Leaf\]\]\]")
check "store into a pointer[ref[X]?] slot is an ordinary store-arith (got $n)" '[ "$n" = "1" ]'

# Provenance of a buffer walk's base and the length in reach: the class is
# unchanged (the union's dedupe never sees the detail), the detail says where
# the walked pointer came from.
check "param walk names its integer neighbour"        'printf "%s\n" "$got" | grep -q "fixture.scaly:26:15: deref-arith pointer\[u8\] base=param(raw) len=n$"'
check "field walk names the concept length"           'printf "%s\n" "$got" | grep -q "fixture.scaly:141:9: deref-arith pointer\[int\] base=field(Grid.cells) len=Grid.count$"'
check "allocated local walk says local(p<-alloc)"     'printf "%s\n" "$got" | grep -q "fixture.scaly:146:5: store-arith pointer\[u8\] base=local(buf<-alloc)$"'
check "local from a call says local(p<-call(f))"      'printf "%s\n" "$got" | grep -q "fixture.scaly:148:11: deref-arith pointer\[u8\] base=local(view<-call(get_buffer))$"'
check "call-result walk names receiver and its length" 'printf "%s\n" "$got" | grep -q "fixture.scaly:149:16: deref-arith pointer\[u8\] base=call(get_buffer) recv=Array len=Array.length$"'
check "Option slot store keeps its provenance"        'printf "%s\n" "$got" | grep -q "fixture.scaly:132:5: store-arith pointer\[Option\[ref\[Leaf\]\]\] base=param(obuf)$"'
check "stack array walk says local(p<-stack[N])"       'printf "%s\n" "$got" | grep -q "fixture.scaly:157:5: store-arith pointer\[u8\] base=local(stack_buf<-stack\[4\])$"'
check "fallback length (integer param before) is marked ?" 'printf "%s\n" "$got" | grep -q "fixture.scaly:157:25: deref-arith pointer\[u8\] base=param(src) len=n?$"'

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
