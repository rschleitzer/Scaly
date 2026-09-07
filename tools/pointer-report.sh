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
# ★★★The union is built from THIS run's per-root files, named here, and never
# from a glob of $OUT. The glob was the defect: the directory is also where
# union.txt and walks.txt land, and where earlier sessions left files of their
# own (detail.txt, decl_rest.txt), so `cat $OUT/*.txt` counted them as sites.
# Measured 2026-09-07 — it reported 11121 where this run's 22 roots hold 7004,
# 4117 of them from two stale files, and the number GROWS every run because
# union.txt reads itself back in. A census whose residue nobody can refute is
# worth nothing, so the instrument may not contaminate its own input.
ROOTS=""
for r in packages/*/0.1.0/*.scaly; do
  b=$(echo "$r" | sed 's#packages/##; s#/0.1.0/#_#; s#\.scaly$##')
  # dazzle needs the wide stack (CLAUDE.md: stage2 cannot compile it at 8 MB)
  ( ulimit -s 65520; "$STAGE" --plan --no-prelude --no-tests --pointer-report "$r" ) > "$OUT/$b.txt" 2>&1
  rc=$?
  ROOTS="$ROOTS $OUT/$b.txt"
  head_line=$(grep -m1 '^pointer-report: [0-9]' "$OUT/$b.txt")
  printf '%-24s rc=%d %s\n' "$b" "$rc" "$(echo "$head_line" | cut -c17-70)"
done
# Union: file:line:col plus class, one line per distinct site.
cat $ROOTS | grep '^pointer-report: .*:[0-9]*:[0-9]*: ' \
  | sed -E 's/^(pointer-report: [^:]+:[0-9]+:[0-9]+: [a-z-]+) .*/\1/' | sort -u > "$OUT/union.txt"
echo "union: $(wc -l < "$OUT/union.txt") distinct sites -> $OUT/union.txt"
echo "-- by class"
awk '{print $NF}' "$OUT/union.txt" | sort | uniq -c | sort -rn
echo "-- by package"
sed -E 's#^pointer-report: packages/([^/]+)/.*: ([a-z-]+)$#\1#' "$OUT/union.txt" | sort | uniq -c | sort -rn
# Buffer walks (deref-arith / store-arith / arith) by the PROVENANCE of the
# walked pointer: `base=param(p) len=n`, `base=local(p<-alloc)`,
# `base=field(T.f) len=T.n`, `base=call(f) recv=T len=T.n`. The full detail
# lines, deduplicated by site, land in walks.txt; the histogram below folds
# the names away and keeps the kind, the kind of a local's initializer, and
# whether a length is in reach.
cat $ROOTS | grep -E '^pointer-report: .*:[0-9]+:[0-9]+: (deref-arith|store-arith|arith) ' \
  | sort -u > "$OUT/walks.txt"
echo "-- buffer walks by provenance ($(wc -l < "$OUT/walks.txt") sites -> $OUT/walks.txt)"
sed -E 's/.* base=//; s/ recv=[^ ]+//; s/ len=.*/ len/; s/local\([A-Za-z_0-9]+<-([a-z]+)(\([^)]*\)|\[[^]]*\])?\)/local(<-\1)/; s/(param|field|call|global|local)\([^)<]*\)/\1(..)/' "$OUT/walks.txt" \
  | sort | uniq -c | sort -rn
echo "-- buffer walks by pointee"
sed -E 's/^pointer-report: [^ ]+ (deref-arith|store-arith|arith) (pointer\[[^ ]*\]).*/\2/' "$OUT/walks.txt" | sort | uniq -c | sort -rn | head -12
