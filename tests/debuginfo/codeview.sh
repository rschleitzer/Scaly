# Sourced by tests/debuginfo/run.sh on Windows: the claims of its DWARF checks
# 1 to 4d, asked of the .pdb the link wrote beside the program ($PDB), with
# llvm-pdbutil ($ORACLE). Same numbering, CodeView's words:
#   a parameter        S_LOCAL ... flags = param
#   a variable         S_LOCAL in the procedure's symbols
#   DW_OP_deref        the variable's type is a REFERENCE (`String&`): the slot
#                      holds a pointer and the debugger reads through it
#   DW_AT_name         the LF_STRUCTURE's name, `Vector<int>` in template syntax
"$ORACLE" dump --symbols "$PDB" >/tmp/debuginfo_pdb_symbols.txt 2>/dev/null
"$ORACLE" dump --types   "$PDB" >/tmp/debuginfo_all.txt 2>/dev/null

# proc_symbols <name>: one procedure's symbols, nested blocks and inline sites
# included, up to its own S_END
proc_symbols() {
  awk -v name="$1" '
    !on && /S_[GL]PROC32 / && index($0, "`" name "`") { on = 1 }
    on { print }
    on && /S_([GL]PROC32|BLOCK32|INLINESITE) / { depth++ }
    on && /S_(END|INLINESITE_END) / { if (--depth == 0) exit }
  ' /tmp/debuginfo_pdb_symbols.txt
}
# pdb_locals: one line per S_LOCAL, `name | type=... (T), flags = ...`
pdb_locals() {
  awk '/S_LOCAL / { match($0, /`[^`]*`/); n = substr($0, RSTART + 1, RLENGTH - 2)
                     getline; sub(/^ */, ""); print n " | " $0 }'
}
proc_symbols shift >/tmp/debuginfo_shift.txt
proc_symbols main  >/tmp/debuginfo_main.txt
pdb_locals </tmp/debuginfo_shift.txt >/tmp/debuginfo_shift_locals.txt
pdb_locals </tmp/debuginfo_main.txt  >/tmp/debuginfo_main_locals.txt

# 1. PARAMETERS
n=$(grep -c 'flags = param' /tmp/debuginfo_shift_locals.txt)
if [ "$n" = "3" ]; then ok; else bad "shift has $n parameters (S_LOCAL, flags = param), expected 3"; fi
need "parameter p missing"              '^p | .*flags = param'    /tmp/debuginfo_shift_locals.txt
need "parameter dx missing"             '^dx | .*flags = param'   /tmp/debuginfo_shift_locals.txt
need "parameter flag missing"           '^flag | .*flags = param' /tmp/debuginfo_shift_locals.txt
need "parameter p is not typed Point"   '^p | .*(Point'           /tmp/debuginfo_shift_locals.txt
need "parameter flag is not typed bool" '^flag | .*(bool)'        /tmp/debuginfo_shift_locals.txt

# 2. A `let` inside a function body.
need "let inner missing from shift" '^inner | ' /tmp/debuginfo_shift_locals.txt

# 3. and 3b. Top-level `let`s and the variables of a `for`, in main.
for v in pt base name out; do
  need "top-level let $v missing from main" "^$v | " /tmp/debuginfo_main_locals.txt
done
for v in k each; do
  need "loop variable $v missing from main" "^$v | " /tmp/debuginfo_main_locals.txt
done

# 4. String: the type, its member, and the indirection -- a REFERENCE type
#    here, which is what DW_OP_deref is in DWARF.
need "no LF_STRUCTURE named String" 'LF_STRUCTURE .*`String`'   /tmp/debuginfo_all.txt
need "String has no data member"    'LF_MEMBER \[name = `data`' /tmp/debuginfo_all.txt
need "String local 'name' is not a reference (String&), so it reads one indirection short" \
     '^name | .*(String&)' /tmp/debuginfo_main_locals.txt

# 4b. The SIGNATURE carries the return type (`finish` reports it).
need "shift has no return type in its signature" 'type = `0x[0-9A-F]* (__int64 (' /tmp/debuginfo_shift.txt

# 4c. The union's TAG: an enumeration over ALL variants -- and with an
#     UNDERLYING type: `void` there and lldb, reading the .pdb, could not size
#     the enum and dropped the `tag` member (2026-11-15).
need "no LF_ENUM for the union tag"                   'LF_ENUM .*`Pick`'         /tmp/debuginfo_all.txt
need "tag enumeration lacks the Chosen variant"       'LF_ENUMERATE \[Chosen = 0' /tmp/debuginfo_all.txt
need "tag enumeration lacks the PAYLOAD-LESS variant" 'LF_ENUMERATE \[Nothing = 1' /tmp/debuginfo_all.txt
if grep -A1 'LF_ENUM .*`Pick`' /tmp/debuginfo_all.txt | grep 'underlying type' | grep -qv '(void)'; then
  ok
else
  bad "tag enumeration has no underlying type (void): lldb drops the tag member"
fi

# 4d. Every instantiation its own name. `Vector<` pins it: VectorIterator<...>
#     has `Iterator` after `Vector`.
vnames=$(grep -oE 'LF_STRUCTURE .*`Vector<[^`]*>`' /tmp/debuginfo_all.txt | grep -oE '`[^`]*`' | sort -u | wc -l | tr -d ' ')
if [ "$vnames" -ge 2 ]; then ok; else bad "only $vnames distinct Vector instantiation names, expected >= 2"; fi
