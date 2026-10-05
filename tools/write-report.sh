#!/bin/bash
# tools/write-report.sh — run `scalyc --write-report` over every package root
# and summarise the union (tools/write-report.py).
#
# The question it measures: how many `function`s write through what their
# parameters reach (R1, "a function is pure by rule") — directly, or only
# because a callee does. Each root reports every body it PLANS, so a stdlib
# generic that only dazzle instantiates is reported by dazzle's root, filed
# under the stdlib file; only the union over all roots is the census.
#
# Usage: tools/write-report.sh [stage-binary] [out-dir]
#   stage-binary  default /tmp/scalyc_stage2 (a compiler carrying the flag)
#   out-dir       default /tmp/write-report; per-root reports land there as
#                 <package>_<root>.txt, the union as union.txt
cd "$(dirname "$0")/.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
OUT=${2:-/tmp/write-report}
mkdir -p "$OUT"
# The union is built from THIS run's per-root files, never from a glob of
# $OUT (tools/pointer-report.sh has the story of the glob that read itself).
# Dependency order: a root reads the summaries (SCALYC_WRITE_SUMMARIES) of
# every body the roots before it planned, so a call into another package is
# judged by that package's own body instead of being unknown.
SUMMARIES="$OUT/summaries.tsv"
rm -f "$SUMMARIES"
export SCALYC_WRITE_SUMMARIES="$SUMMARIES"
ORDER="scaly opensp scalyc dazzle scalyls scalygpu"
LIST=""
for p in $ORDER; do
  LIST="$LIST packages/$p/0.1.0/$p.scaly"
done
for r in packages/*/0.1.0/*.scaly; do
  case " $LIST " in *" $r "*) ;; *) LIST="$LIST $r" ;; esac
done
ROOTS=""
for r in $LIST; do
  b=$(echo "$r" | sed 's#packages/##; s#/0.1.0/#_#; s#\.scaly$##')
  ( ulimit -s 65520; "$STAGE" --plan --no-prelude --no-tests --write-report "$r" ) > "$OUT/$b.txt" 2>&1
  rc=$?
  ROOTS="$ROOTS $OUT/$b.txt"
  head_line=$(grep -m1 '^write-report: [0-9]' "$OUT/$b.txt")
  printf '%-24s rc=%d %s\n' "$b" "$rc" "$(echo "$head_line" | cut -c15-260)"
done
# shellcheck disable=SC2086
python3 tools/write-report.py "$OUT/union.txt" $ROOTS
