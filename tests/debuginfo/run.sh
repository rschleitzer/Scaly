#!/bin/bash
# -g / DWARF gate: `let` bindings, function PARAMETERS and String locals must
# reach the DWARF, and -g must stay fully gated.
#
# Why this suite exists at all: there was no -g test anywhere in tests/ until
# 2026-08-11, and the three things it checks had ALL rotted into place unnoticed
# — measured on that day's binary, the whole tree emitted 0
# DW_TAG_formal_parameter across 49 subprograms, no `let` ever appeared (in a
# language whose top level is nothing but `let`), and a String local was dropped
# silently because di_type_for answered null and the declare site's `if dty <>
# null` guard threw the variable away.
#
# ★The oracle is llvm-dwarfdump, not lldb, so the suite runs on the Linux CI
# leg: lldb is optional and only adds the value checks. The DWARF lives in a
# .dSYM on macOS (cli.scaly runs dsymutil) and inside the executable elsewhere,
# so the dump target is chosen accordingly.
#
# ★What a green run does NOT prove is that -g is emission-neutral — that belongs
# to tools/cycle.sh and to comparing -S output between two compilers. What it
# DOES prove locally is that a non-g compile carries no debug metadata at all,
# which is the other half of the same claim.
#
# Negative control (how to show this gate can fail): run it against a compiler
# built from a seed that predates the fix — `tests/debuginfo/run.sh
# scalyc/build/scalyc` right after a `tools/build-from-seed.sh` on the old seed
# reports the three missing classes one by one. Note that ./build.sh does NOT
# rebuild the compiler from source; it re-links the committed seed .ll, so a
# compiler change needs tools/bootstrap.sh.
#
# Usage: tests/debuginfo/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
SRC=tests/debuginfo/probe.scaly
OUT=/tmp/debuginfo_probe$SCALY_EXE
pass=0; fail=0; failures=()

ok()   { pass=$((pass+1)); }
bad()  { fail=$((fail+1)); failures+=("$1"); }

need() { # need <description> <pattern> <haystack-file>
  if grep -q -- "$2" "$3"; then ok; else bad "$1"; fi
}

DWARFDUMP=$(command -v llvm-dwarfdump || true)
if [ -z "$DWARFDUMP" ]; then
  # tools/llvm-env.sh knows where the pinned LLVM lives.
  # shellcheck disable=SC1091
  . tools/llvm-env.sh >/dev/null 2>&1 || true
  [ -n "$LLVM_PREFIX" ] && [ -x "$LLVM_PREFIX/bin/llvm-dwarfdump" ] && DWARFDUMP="$LLVM_PREFIX/bin/llvm-dwarfdump"
fi
if [ -z "$DWARFDUMP" ]; then
  echo "debuginfo: SKIP (no llvm-dwarfdump)"
  exit 0
fi

rm -rf "$OUT" "$OUT.dSYM" /tmp/debuginfo_nog.ll
"$STAGE" -g -o "$OUT" "$SRC" >/tmp/debuginfo_build.log 2>&1
if [ ! -x "$OUT" ]; then
  echo "debuginfo: FAIL — -g compile produced no binary"
  sed -n '1,20p' /tmp/debuginfo_build.log
  exit 1
fi

# 0. The -g binary must still be a CORRECT program. A debug build that changes
#    behaviour is worse than no debug build.
got=$("$OUT" 2>/dev/null)
if [ "$got" = "34" ]; then ok; else bad "-g binary printed '$got', expected '34'"; fi

# The DWARF container: .dSYM on macOS, the executable itself elsewhere.
TARGET="$OUT"
[ -d "$OUT.dSYM" ] && TARGET="$OUT.dSYM"

"$DWARFDUMP" --name=shift --show-children "$TARGET" >/tmp/debuginfo_shift.txt 2>/dev/null
"$DWARFDUMP" --name=main  --show-children "$TARGET" >/tmp/debuginfo_main.txt  2>/dev/null
"$DWARFDUMP" "$TARGET" >/tmp/debuginfo_all.txt 2>/dev/null

# 1. PARAMETERS: all three of shift, as formal parameters and correctly typed.
n=$(grep -c DW_TAG_formal_parameter /tmp/debuginfo_shift.txt)
if [ "$n" = "3" ]; then ok; else bad "shift has $n DW_TAG_formal_parameter, expected 3"; fi
need "parameter p missing"            '"p"'      /tmp/debuginfo_shift.txt
need "parameter dx missing"           '"dx"'     /tmp/debuginfo_shift.txt
need "parameter flag missing"         '"flag"'   /tmp/debuginfo_shift.txt
need "parameter p is not typed Point" '"Point"'  /tmp/debuginfo_shift.txt
need "parameter flag is not typed bool" '"bool"' /tmp/debuginfo_shift.txt

# 2. A `let` inside a function body.
need "let inner missing from shift"   '"inner"'  /tmp/debuginfo_shift.txt

# 3. Top-level `let`s — the synthetic main's scope.
for v in pt base name out; do
  need "top-level let $v missing from main" "\"$v\"" /tmp/debuginfo_main.txt
done

# 4. String: the type itself, its member, and — the half that is easy to get
#    backwards — the DEREF in the location. `name` holds a POINTER to the
#    String, so a location without DW_OP_deref would read the pointer's own
#    slot as if it were the object: silently wrong by one indirection.
need "no DW_TAG_structure_type named String" '"String"' /tmp/debuginfo_all.txt
need "String has no data member"             '"data"'   /tmp/debuginfo_all.txt
if grep -A3 '"name"' /tmp/debuginfo_main.txt | grep -q DW_OP_deref \
   || grep -B3 '"name"' /tmp/debuginfo_main.txt | grep -q DW_OP_deref; then
  ok
else
  bad "String local 'name' has no DW_OP_deref in its location"
fi

# 4b. The SIGNATURE: element 0 of the DISubroutineType is the return type, and
#     without it lldb's `finish` cannot report a returned value.
need "shift has no DW_AT_type (return type)" 'DW_AT_type' /tmp/debuginfo_shift.txt

# 4c. A union's TAG is an enumeration over ALL variants, including the
#     payload-less ones — the payload union holds members only for variants that
#     HAVE a payload, so its member index is not the tag and no reader can
#     reconstruct the mapping. Nothing/1 is the entry that proves it.
"$DWARFDUMP" --name=Pick --show-children "$TARGET" >/tmp/debuginfo_pick.txt 2>/dev/null
need "no DW_TAG_enumeration_type for the union tag" 'DW_TAG_enumeration_type' /tmp/debuginfo_pick.txt
need "tag enumeration lacks the Chosen variant"     '"Chosen"'  /tmp/debuginfo_pick.txt
need "tag enumeration lacks the PAYLOAD-LESS variant" '"Nothing"' /tmp/debuginfo_pick.txt

# 4d. Every generic INSTANTIATION needs its own DW_AT_name. lldb uniques types
#     by name, so two Vector[T] both called "Vector" collapse into one and the
#     survivor's element type is used for both — elements then read at the wrong
#     stride, silently. The fixture instantiates Vector over int AND (via String)
#     over char, so two distinct names must exist.
#
#     ★The pattern must pin Vector EXACTLY. `VectorI` also matches
#     "VectorIterator[char]", so an unanchored count passed against a compiler
#     that collapsed every Vector into one — it was counting the iterators.
#     Accept both spellings: the readable `Vector[T]` di_display_name normally
#     produces, and the mangled `_Z6VectorI...E` it falls back to when a
#     PlannedType arrives without its generics.
vnames=$(grep -oE 'DW_AT_name[^"]*"(Vector\[[^]]*\]|_Z6VectorI[^"]*E)"' /tmp/debuginfo_all.txt | sort -u | wc -l | tr -d ' ')
if [ "$vnames" -ge 2 ]; then ok; else bad "only $vnames distinct Vector instantiation names, expected >= 2"; fi

# 5. -g must be GATED: a non-g compile carries no debug metadata whatsoever.
"$STAGE" -S -o /tmp/debuginfo_nog.ll "$SRC" >/dev/null 2>&1
for pat in '#dbg_declare' 'DILocalVariable' 'DISubprogram' '!dbg'; do
  if grep -q -- "$pat" /tmp/debuginfo_nog.ll; then
    bad "non-g emission contains $pat"
  else
    ok
  fi
done

# 6. Optional: read the VALUES. A location that resolves to garbage is worse
#    than a missing variable, and only a debugger can tell the difference.
if command -v lldb >/dev/null 2>&1; then
  # Line 33 is `set running: ...` inside shift — the parameters and `inner` are
  # all bound there. Line 45 is `let out shift(...)`, after every top-level
  # binding. Keep both in step with probe.scaly; a breakpoint on a comment or on
  # the declaration line itself reads UNINITIALIZED memory and looks like a
  # compiler defect.
  lldb -b -o "b probe.scaly:33" -o run -o "frame variable" -o finish "$OUT" \
    >/tmp/debuginfo_lldb.txt 2>&1
  need "lldb: parameter p.x is not 10"  'x = 10'   /tmp/debuginfo_lldb.txt
  need "lldb: parameter p.y is not 20"  'y = 20'   /tmp/debuginfo_lldb.txt
  need "lldb: parameter dx is not 7"    'dx = 7'   /tmp/debuginfo_lldb.txt
  need "lldb: parameter flag is not true" 'flag = true' /tmp/debuginfo_lldb.txt
  need "lldb: let inner is not 17"      'inner = 17' /tmp/debuginfo_lldb.txt
  need "lldb: finish reports no return value" 'Return value' /tmp/debuginfo_lldb.txt

  lldb -b -o "b probe.scaly:45" -o run -o "frame variable" "$OUT" \
    >/tmp/debuginfo_lldb2.txt 2>&1
  need "lldb: let base is not 7"        'base = 7' /tmp/debuginfo_lldb2.txt
  need "lldb: let out is not 34"        'out = 34' /tmp/debuginfo_lldb2.txt
  # Bare DWARF: the buffer is varint-prefixed, so the text arrives with a length
  # byte in front. Decoding it is the FORMATTER's job (checked below); here the
  # gate only asserts the bytes are reachable and correct.
  need "lldb: String local does not read as 'probe'" 'probe' /tmp/debuginfo_lldb2.txt
  # The union tag prints its VARIANT NAME with no formatter involved.
  need "lldb: union tag does not print as Chosen" 'tag = Chosen' /tmp/debuginfo_lldb2.txt
  # ★A region-allocated `var` keeps a POINTER slot while its DIType is the
  #  OBJECT, so the declare needs DW_OP_deref. Without it the debugger reads the
  #  slot holding the pointer AS the object — measured: length = 31092379704.
  #  `length = 3` is the whole assertion.
  #  ★Anchored: an unanchored `length = 3` is a SUBSTRING of the very garbage it
  #  is meant to catch (the pre-fix reading was length = 31092379704), so the
  #  check passed against a compiler that had the defect. An assertion that
  #  cannot fail is worse than none.
  need "lldb: region-allocated var reads through one indirection too few" \
       'length = 3$' /tmp/debuginfo_lldb2.txt

  # 7. The FORMATTERS (tools/lldb/scaly.py): a String reads as text with the
  #    length prefix gone, and a Vector's elements become children at the right
  #    stride. The stride half is what caught the type-name collapse.
  lldb -b -o "command script import tools/lldb/scaly.py" \
       -o "b probe.scaly:45" -o run -o "frame variable name items" "$OUT" \
    >/tmp/debuginfo_fmt.txt 2>&1
  need "formatter: not loaded"                 'formatters loaded'  /tmp/debuginfo_fmt.txt
  need "formatter: String is not decoded"      'name = "probe"'     /tmp/debuginfo_fmt.txt
  need "formatter: Vector count missing"       '3 element(s)'       /tmp/debuginfo_fmt.txt
  need "formatter: Vector element 0 is not 7"  '\[0\] = 7'          /tmp/debuginfo_fmt.txt
  need "formatter: Vector element 2 is not 9"  '\[2\] = 9'          /tmp/debuginfo_fmt.txt
fi

# 8. The EDITOR path: lldb-dap driven over the protocol, with the same launch
#    attributes and the same initCommands the VS Code extension sends. A wrong
#    key in editors/vscode/package.json cannot be caught by a typecheck and would
#    otherwise fail at debug time in someone's editor.
DAP=""
for c in lldb-dap lldb-dap-21 /opt/homebrew/opt/llvm@21/bin/lldb-dap \
         /usr/lib/llvm-21/bin/lldb-dap \
         /Applications/Xcode.app/Contents/Developer/usr/bin/lldb-dap; do
  p=$(command -v "$c" 2>/dev/null || true)
  [ -n "$p" ] && { DAP="$p"; break; }
done
if [ -n "$DAP" ] && command -v python3 >/dev/null 2>&1; then
  if python3 tests/debuginfo/dap_smoke.py "$DAP" "$OUT" \
       "$PWD/$SRC" 45 "$PWD/tools/lldb/scaly.py" >/tmp/debuginfo_dap.txt 2>&1; then
    ok
  else
    while read -r l; do bad "${l#dap: FAIL }"; done < <(grep '^dap: FAIL' /tmp/debuginfo_dap.txt)
    grep -q '^dap: FAIL' /tmp/debuginfo_dap.txt || bad "lldb-dap smoke test failed: $(tail -1 /tmp/debuginfo_dap.txt)"
  fi
fi

if [ "$fail" = 0 ]; then
  echo "debuginfo: $pass PASS, 0 FAIL"
  exit 0
fi
echo "debuginfo: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  - $f"; done
exit 1
