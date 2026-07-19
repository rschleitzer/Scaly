#!/usr/bin/env bash
# tests/dazzle/codegen/run.sh — byte-exact openjade drop-in check.
#
#   tests/dazzle/codegen/run.sh [scalyc-binary]
#
# Regenerates real mkp DSSSL codegen with the dazzle CLI (the `openjade -t sgml
# -d <spec.dsl> <doc.sgm>` invocations from ./mkp) and asserts every generated
# file comes back BYTE-IDENTICAL to the committed openjade output. This is the
# survival gate for the dazzle port: full DSSSL pipeline — catalog-resolved
# STYLE-SHEET DTD, external .scm entity expansion, builtins.dsl prolog,
# construction rules, `make`/`file` flow objects, the RE->newline output sink —
# reproducing James Clark's engine to the byte over the project's own corpus.

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"
set -u

DZ="$(mktemp -d)/dazzle"
SNAP="$(mktemp -d)"
trap 'rm -rf "$(dirname "$DZ")" "$SNAP"' EXIT

if ! tests/dazzle/build-cli.sh "$DZ" "$BIN" > "$SNAP/build.log" 2>&1; then
  echo "dazzle-codegen: FAIL (build)"; tail -8 "$SNAP/build.log"; exit 1
fi

# (spec.dsl, doc.sgm, generated-files...) — the ./mkp openjade lines.
run_case() {
  local spec="$1" doc="$2"; shift 2
  local targets=("$@")
  # snapshot the committed goldens
  local i=0
  for f in "${targets[@]}"; do cp "$f" "$SNAP/g$i"; i=$((i+1)); done
  if ! SCALY_HOME="$ROOT" "$DZ" -t sgml -d "$spec" "$doc" > "$SNAP/stdout" 2>"$SNAP/stderr"; then
    echo "dazzle-codegen: FAIL ($spec rc=$?)"; cat "$SNAP/stderr"; return 1
  fi
  # diff each regenerated file against its golden, then restore the golden
  local rc=0; i=0
  for f in "${targets[@]}"; do
    if ! diff -q "$SNAP/g$i" "$f" >/dev/null 2>&1; then
      echo "dazzle-codegen: FAIL — $f differs from openjade golden"; diff "$SNAP/g$i" "$f" | head -6; rc=1
    fi
    cp "$SNAP/g$i" "$f"; i=$((i+1))
  done
  return $rc
}

fail=0
run_case codegen/test-choose.dsl tests/choose.sgm \
  docs/scaly/generated-choose.xml tests/selfhosted/choose__*.scaly || fail=1
run_case codegen/scaly.dsl scaly.sgm \
  packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly \
  packages/scalyc/0.1.0/scalyc/compiler/parser.scaly \
  packages/scalyls/0.1.0/scalyls/grammar.scaly || fail=1
run_case codegen/test-expressions.dsl tests/expressions.sgm \
  docs/scaly/generated-expressions.xml tests/selfhosted/expressions__*.scaly || fail=1
run_case codegen/test-definitions.dsl tests/definitions.sgm \
  docs/scaly/generated-definitions.xml tests/selfhosted/definitions__*.scaly || fail=1
run_case codegen/test-controlflow.dsl tests/controlflow.sgm \
  docs/scaly/generated-controlflow.xml tests/selfhosted/controlflow__*.scaly || fail=1

if [ "$fail" -ne 0 ]; then exit 1; fi
echo "dazzle-codegen: PASS"
