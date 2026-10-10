#!/bin/bash
# tools/operation-census.sh — the operation census over every package root:
# what the operand sequences of the tree are,
# counted by the planner where it plans them (SCALYC_OPERATION_CENSUS=1,
# Planner.operation_census#). One line per root and the sum.
#
#   tools/operation-census.sh [scalyc] [-v]     -v: every root's own counts
#   OPERATION_SITES=<file>: write every counted place (slot file:offset)
# A root reports the sequences of every body it PLANS -- its own package and
# the generic bodies it instantiates -- so a sum counts stdlib generics once
# per root that uses them. The numbers say how often a shape occurs, not how
# many source lines hold it.
set -u
cd "$(dirname "$0")/.."
BIN=${1:-/tmp/scalyc_stage2}
VERBOSE=${2:-}
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
one() {
  r="$1"; n="$(echo "$r" | sed 's|packages/||; s|/0.1.0/|-|; s|\.scaly$||')"
  fl="--no-tests"; [ "$r" = packages/scaly/0.1.1/scaly.scaly ] && fl="--no-prelude --no-tests"
  ( ulimit -s 65520; SCALYC_OPERATION_CENSUS=1 "$BIN" -S $fl -o "$OUT/$n.ll" "$r" > "$OUT/$n.out" 2> "$OUT/$n.err" )
  echo $? > "$OUT/$n.rc"
}
export -f one; export BIN OUT
ls packages/*/0.1.0/*.scaly | xargs -P 6 -n 1 -I{} bash -c 'one {}'
bad=0
for rc in "$OUT"/*.rc; do [ "$(cat "$rc")" = 0 ] || { bad=$((bad+1)); echo "FAILED: $(basename "$rc" .rc)"; }; done
[ "$VERBOSE" = "-v" ] && for e in "$OUT"/*.err; do echo "== $(basename "$e" .err)"; grep '^operation-census ' "$e" | sed 's/^operation-census //'; done
echo "== $(ls "$OUT"/*.rc | wc -l | tr -d ' ') roots: planned (summed over the roots) and places (each source place once)"
cat "$OUT"/*.err | grep '^operation-census ' | awk '{c[$2]+=$3; if (!($2 in o)) {o[$2]=++n; k[n]=$2}} END {for (i=1;i<=n;i++) print k[i], c[k[i]]}' > "$OUT/sum.txt"
cat "$OUT"/*.err | grep '^operation-site ' | sort -u | awk '{c[$2]++} END {for (s in c) print s, c[s]}' > "$OUT/places.txt"
# the slot a counter's name stands for, as Planner.operation_census_report# lists them
slots="function+tuple:5 function+value:6 function+vector:18 type+tuple:7 type+vector:8 value+vector:9 dotted+tuple:14 name+tuple:15 prefix-at-start:10 prefix-after-operator:11 prefix-after-function:12 method+tuple:16 method+empty-tuple:17 method+value:19 method-bare:13"
printf "%-24s %10s %10s\n" "" planned places
while read -r name count; do
  slot=""; for e in $slots; do [ "${e%%:*}" = "$name" ] && slot="${e##*:}"; done
  places="-"; [ -n "$slot" ] && places=$(awk -v s="$slot" '$1 == s {print $2}' "$OUT/places.txt") && [ -z "$places" ] && places=0
  printf "%-24s %10d %10s\n" "$name" "$count" "$places"
done < "$OUT/sum.txt"
[ -n "${OPERATION_SITES:-}" ] && cat "$OUT"/*.err | grep '^operation-site ' | sort -u > "$OPERATION_SITES" && echo "places written to $OPERATION_SITES"
[ $bad = 0 ]
