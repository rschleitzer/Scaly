#!/bin/bash
# tools/pointer-report.sh — run `scalyc --pointer-report` over every package
# root and print the deduplicated union.
#
# Each root reports the sites of every body it PLANS, which for a generic
# concept means the roots that instantiate it — a stdlib generic method that
# only dazzle instantiates is reported by dazzle's root, filed under the
# stdlib file it was written in. The union over all 22 roots, deduplicated by
# file:line:col and class, is therefore the tree-wide census; a single root's
# head line is not.
#
# Usage: tools/pointer-report.sh [stage-binary] [out-dir]
#   stage-binary  default /tmp/scalyc_stage2 (a compiler carrying the flag)
#   out-dir       default /tmp/pointer-report; per-root reports land there as
#                 <package>_<root>.txt, the union as union.txt
cd "$(dirname "$0")/.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
OUT=${2:-/tmp/pointer-report}
mkdir -p "$OUT"
for r in packages/*/0.1.0/*.scaly; do
  b=$(echo "$r" | sed 's#packages/##; s#/0.1.0/#_#; s#\.scaly$##')
  # dazzle needs the wide stack (CLAUDE.md: stage2 cannot compile it at 8 MB)
  ( ulimit -s 65520; "$STAGE" --plan --no-prelude --no-tests --pointer-report "$r" ) > "$OUT/$b.txt" 2>&1
  rc=$?
  head_line=$(grep -m1 '^pointer-report: [0-9]' "$OUT/$b.txt")
  printf '%-24s rc=%d %s\n' "$b" "$rc" "$(echo "$head_line" | cut -c17-70)"
done
# Union: file:line:col plus class, one line per distinct site.
cat "$OUT"/*.txt | grep '^pointer-report: .*:[0-9]*:[0-9]*: ' \
  | sed -E 's/^(pointer-report: [^:]+:[0-9]+:[0-9]+: [a-z-]+) .*/\1/' | sort -u > "$OUT/union.txt"
echo "union: $(wc -l < "$OUT/union.txt") distinct sites -> $OUT/union.txt"
echo "-- by class"
awk '{print $NF}' "$OUT/union.txt" | sort | uniq -c | sort -rn
echo "-- by package"
sed -E 's#^pointer-report: packages/([^/]+)/.*: ([a-z-]+)$#\1#' "$OUT/union.txt" | sort | uniq -c | sort -rn
