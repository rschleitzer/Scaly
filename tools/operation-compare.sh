#!/bin/bash
# tools/operation-compare.sh — the yardstick of the operation driver
# (Planner.operation#): every package
# root is emitted twice, by the old collapse alone (SCALYC_OPERATION_OLD=1)
# and with the driver in front of it (the default since 2026-10-18), and the
# two IR files must be the same bytes -- each difference is judged.
# With it the driver's counters: how many sequences it answered, the steps it
# took, and why it left the others to the old collapse.
#
# The roots are this tree's and, where the checkouts are there, those of
# ../dazzle ($DAZZLE_REPO) and ../tscaly ($TSCALY_REPO): each compiled from
# its own checkout (its ./packages is the project's), with SCALY_HOME here.
#
#   tools/operation-compare.sh [scalyc] [filter]    filter: a part of a root's path
#   OPERATION_KEEP=<dir>: keep the IR files and logs there
set -u
cd "$(dirname "$0")/.."
HOME_DIR="$(pwd)"
BIN=${1:-/tmp/scalyc_stage2}
FILTER=${2:-}
OUT=${OPERATION_KEEP:-$(mktemp -d)}
mkdir -p "$OUT"
[ -n "${OPERATION_KEEP:-}" ] || trap 'rm -rf "$OUT"' EXIT
DAZZLE=${DAZZLE_REPO:-../dazzle}
TSCALY=${TSCALY_REPO:-../tscaly}
roots() {
  for repo in "$HOME_DIR" "$DAZZLE" "$TSCALY"; do
    [ -d "$repo/packages" ] || continue
    repo="$(cd "$repo" && pwd)"
    for r in "$repo"/packages/*/[0-9]*/*.scaly; do
      echo "$repo ${r#$repo/}"
    done
  done
}
one() {
  repo="$1"; r="$2"
  n="$(basename "$repo")-$(echo "$r" | sed 's|^packages/||; s|/|-|g; s|\.scaly$||')"
  fl="--no-tests"
  case "$r" in packages/scaly/*/scaly.scaly) fl="--no-prelude --no-tests";; esac
  ( cd "$repo"; ulimit -s 65520
    SCALY_HOME="$HOME_DIR" SCALYC_OPERATION_OLD=1 "$BIN" -S $fl -o "$OUT/$n.off.ll" "$r" > "$OUT/$n.off.out" 2>&1; echo $? > "$OUT/$n.off.rc"
    SCALY_HOME="$HOME_DIR" SCALYC_OPERATION_CENSUS=1 "$BIN" -S $fl -o "$OUT/$n.on.ll" "$r" > "$OUT/$n.on.out" 2> "$OUT/$n.on.err"; echo $? > "$OUT/$n.on.rc" )
}
export -f one; export BIN OUT HOME_DIR
# The interfaces of the ports' library packages first, in dependency order,
# as their own builds lay them down (tscaly's tests/packages.sh): read from
# its sources a package tells the checks less, and tsdriver is refused on
# that (tools/interfaces.sh covers this tree's packages only).
port_interfaces() {
  repo="$1"; shift
  [ -d "$repo/packages" ] || return 0
  for p in "$@"; do
    root=$(ls "$repo"/packages/"$p"/[0-9]*/"$p".scaly 2>/dev/null | sort -V | tail -1)
    [ -n "$root" ] || continue
    ( cd "$repo"; ulimit -s 65520
      SCALY_HOME="$HOME_DIR" "$BIN" --emit-interface "${root#$repo/}" > /dev/null 2>&1 ) \
      || echo "operation-compare: no interface for $p" >&2
  done
}
port_interfaces "$DAZZLE" opensp pdf
port_interfaces "$TSCALY" tsbase tscaly
roots | grep -e "$FILTER" | xargs -P 6 -L 1 bash -c 'one $0 $1'
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
