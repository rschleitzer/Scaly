#!/usr/bin/env bash
# o2check.sh BEFORE_HOME COMPILER [OUT] -- prove a source rewrite changes no code.
#
# BEFORE_HOME is a directory holding a `packages/` copy of the tree as it was
# before the rewrite; the repository is the tree after it. Every package root
# is emitted from both with the SAME compiler, run through `opt -O2` (linkonce
# promoted to external, so nothing is inlined away), and compared function by
# function with o2norm.py -- instruction multisets, names masked. A cosmetic
# rewrite must come out with zero differences.
set -u
cd "$(dirname "$0")/../.."
BEFORE=$1; BIN=$2; OUT=${3:-$(mktemp -d)}
case $BIN in /*) ;; *) BIN=$PWD/$BIN ;; esac
source tools/llvm-env.sh >/dev/null 2>&1
mkdir -p "$OUT"
one() {
  local f=$1 n fl="" rc=0
  n=$(echo "$f" | tr '/' '_')
  [ "$f" = packages/scaly/0.1.0/scaly.scaly ] && fl=--no-prelude
  for side in old new; do
    local home=$PWD
    [ $side = old ] && home=$BEFORE
    ( cd "$home" && ulimit -s 65520 && SCALY_HOME="$home" "$BIN" -S --no-tests $fl -o "$OUT/$n.$side.ll" "$f" ) > "$OUT/$n.$side.err" 2>&1 || { echo "EMIT FAIL ($side) $f"; return 1; }
    sed 's/^define linkonce_odr /define /; s/^define weak_odr /define /' "$OUT/$n.$side.ll" > "$OUT/$n.$side.ext.ll"
    "$OPT" -O2 -S "$OUT/$n.$side.ext.ll" -o "$OUT/$n.$side.o2.ll" 2> "$OUT/$n.$side.opt.err" || { echo "OPT FAIL ($side) $f"; return 1; }
  done
  if python3 tools/ifmerge/o2norm.py "$OUT/$n.old.o2.ll" "$OUT/$n.new.o2.ll" > "$OUT/$n.cmp"; then
    echo "SAME     $f"
  else
    echo "DIFFERS  $f"; sed -n '1,12p' "$OUT/$n.cmp" | sed 's/^/    /'
    rc=1
  fi
  return $rc
}
rc=0
# O2CHECK_ROOTS narrows the run (a grep pattern over the root paths)
for f in packages/*/0.1.0/*.scaly; do
  if [ -n "${O2CHECK_ROOTS:-}" ]; then echo "$f" | grep -qE "$O2CHECK_ROOTS" || continue; fi
  one "$f" || rc=1
done
echo "o2check: $([ $rc = 0 ] && echo 'no code changed' || echo 'DIFFERENCES') (work in $OUT)"
exit $rc
