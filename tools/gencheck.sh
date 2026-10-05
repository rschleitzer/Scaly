#!/bin/bash
# Checks whether every GENERATED file still is what its generator produces.
#
# ★★★THE CLASS THIS GATE CATCHES: a conversion is made in the
# GENERATED FILE and not in the generator.  Until the next
# regeneration nobody notices, then it silently reverts.  Found twice
# on 2026-09-04:
#   * tscaly/Keywords.scaly  -- kw_eq converted to Slice[char], the generator
#     went on producing (pointer[const_char], int): 88 sites.
#   * dazzle/CharNames.scaly + Sdata.scaly -- pointer[Interpreter] -> ref and
#     the literal-cast sweep (` as u32`): 1830 diff lines.
#
# ★The openjade/opensp sources are missing on the development machine, so
# chartablegen cannot run against the real table.  The source is therefore
# RECONSTRUCTED FROM THE OUTPUT -- that still checks everything that is not
# the entry data itself (signatures, frame, number format), and that is
# exactly where both finds sat.
set -u
cd "$(dirname "$0")/.."
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
fail=0

# --- 1. (dropped 2026-10-05) -----------------------------------------------
# The generators that write their target file directly were tscaly's
# (packages/tscaly/tools/gen*.py). tscaly is a repository of its own
# (github.com/rschleitzer/tscaly); the run there: start every generator and
# compare the generated files before and after by checksum.

# --- 2. chartablegen: reconstruct the source from the output --------------
python3 - "$TMP" <<'PY'
import re, sys
tmp = sys.argv[1]
# The source's file name ends up IN THE HEAD of the output, so the
# reconstructed file must have exactly the name of the real reference table.
for concept, call, src in (('CharNames', 'def_char', 'charNames.h'),
                           ('Sdata', 'def_sdata_char', 'sdata.h')):
    f = f'packages/dazzle/0.1.0/dazzle/{concept}.scaly'
    pat = re.compile(rf'interp\.{call}\("([^"]+)",\s*0x([0-9A-Fa-f]+)\)')
    rows = [(m.group(2), m.group(1)) for m in pat.finditer(open(f, encoding='utf8').read())]
    with open(f'{tmp}/{src}', 'w') as o:
        for code, name in rows:
            o.write('  { 0x%s, "%s" },\n' % (code, name))
PY
for pair in "CharNames:charNames.h" "Sdata:sdata.h"; do
    c=${pair%%:*}; src=${pair#*:}
    python3 tools/chartablegen.py "$c" "$TMP/$src" > "$TMP/$c.scaly" 2>/dev/null
    if ! cmp -s "packages/dazzle/0.1.0/dazzle/$c.scaly" "$TMP/$c.scaly"; then
        echo "DIVERGENT  chartablegen.py -> $c.scaly"
        diff "packages/dazzle/0.1.0/dazzle/$c.scaly" "$TMP/$c.scaly" | head -6 | sed 's/^/           /'
        fail=1
    fi
done

[ $fail = 0 ] && echo "gencheck: OK — every generated file checked is generator output"
exit $fail
