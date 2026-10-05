#!/bin/bash
# tools/operation-compare.sh — the yardstick of the operation driver
# (Planner.operation#): every package
# root is emitted twice, by the old collapse alone and with the driver in
# front of it (SCALYC_PENDING=1), and the two IR files must be the same bytes.
# With it the driver's counters: how many sequences it answered, the steps it
# took, and why it left the others to the old collapse.
#
#   tools/operation-compare.sh [scalyc] [filter]    filter: a part of a root's path
#   OPERATION_KEEP=<dir>: keep the IR files and logs there
set -u
cd "$(dirname "$0")/.."
BIN=${1:-/tmp/scalyc_stage2}
FILTER=${2:-}
OUT=${OPERATION_KEEP:-$(mktemp -d)}
mkdir -p "$OUT"
[ -n "${OPERATION_KEEP:-}" ] || trap 'rm -rf "$OUT"' EXIT
one() {
  r="$1"; n="$(echo "$r" | sed 's|packages/||; s|/0.1.0/|-|; s|\.scaly$||')"
  fl="--no-tests"; [ "$r" = packages/scaly/0.1.0/scaly.scaly ] && fl="--no-prelude --no-tests"
  ( ulimit -s 65520
    "$BIN" -S $fl -o "$OUT/$n.off.ll" "$r" > "$OUT/$n.off.out" 2>&1; echo $? > "$OUT/$n.off.rc"
    SCALYC_PENDING=1 SCALYC_OPERATION_CENSUS=1 "$BIN" -S $fl -o "$OUT/$n.on.ll" "$r" > "$OUT/$n.on.out" 2> "$OUT/$n.on.err"; echo $? > "$OUT/$n.on.rc" )
}
export -f one; export BIN OUT
ls packages/*/0.1.0/*.scaly | grep -e "$FILTER" | xargs -P 6 -n 1 -I{} bash -c 'one {}'
same=0; differ=0; failed=0
for rc in "$OUT"/*.off.rc; do
  n="$(basename "$rc" .off.rc)"
  if [ "$(cat "$rc")" != 0 ] || [ "$(cat "$OUT/$n.on.rc")" != 0 ]; then
    failed=$((failed+1)); echo "FAILED  $n (old rc $(cat "$rc"), driver rc $(cat "$OUT/$n.on.rc"))"
  elif cmp -s "$OUT/$n.off.ll" "$OUT/$n.on.ll"; then
    same=$((same+1))
  else
    differ=$((differ+1)); echo "DIFFERS $n"
  fi
done
echo "== $((same+differ+failed)) roots: $same identical, $differ differ, $failed failed"
cat "$OUT"/*.on.err | grep '^operation-census driver-' | awk '{c[$2]+=$3; if (!($2 in o)) {o[$2]=++n; k[n]=$2}} END {for (i=1;i<=n;i++) printf "%-44s %10d\n", k[i], c[k[i]]}'
[ $differ = 0 ] && [ $failed = 0 ]
