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
# ★The engine writes its `file` flow objects relative to its WORKING
# DIRECTORY, so each case runs in a scratch directory of its own and the
# outputs are compared there with the committed files. It used to run in the
# tree: every generated file was overwritten in place and the golden copied
# back afterwards — and in tools/bar.sh this suite runs BESIDE the compiler
# lane, whose selfhosted suite then read a file the engine had just truncated
# ("Error: Could not read input file", one test in one bar, 2026-09-27); a run
# stopped inside the window left the tree changed. The tree is read only now.
run_case() {
  local spec="$1" doc="$2"; shift 2
  local out; out=$(mktemp -d "$SNAP/case.XXXXXX")
  local f
  for f in "$@"; do mkdir -p "$out/$(dirname "$f")"; done
  if ! (cd "$out" && SCALY_HOME="$ROOT" "$DZ" -t sgml -d "$ROOT/$spec" "$ROOT/$doc" > "$SNAP/stdout" 2>"$SNAP/stderr"); then
    echo "dazzle-codegen: FAIL ($spec)"; cat "$SNAP/stderr"; return 1
  fi
  local rc=0
  for f in "$@"; do
    if [ ! -f "$out/$f" ]; then
      echo "dazzle-codegen: FAIL — $f was not generated"; rc=1
    elif ! diff -q "$f" "$out/$f" >/dev/null 2>&1; then
      echo "dazzle-codegen: FAIL — $f differs from openjade golden"; diff "$f" "$out/$f" | head -6; rc=1
    fi
  done
  return $rc
}

fail=0
run_case codegen/test-choose.dsl tests/choose.sgm \
  docs/scaly/generated-choose.xml tests/selfhosted/choose__*.scaly || fail=1
# ★The tmLanguage.json is the FOURTH output of this spec and was missing from
# this list until 2026-08-11 — generated on every run, compared on none. It
# went unnoticed because it is the one output nothing else reads: a wrong
# parser.scaly fails to compile, a wrong grammar.scaly fails the LSP suite,
# but a wrong syntax-highlighting table is only ever seen by a human in an
# editor. **An output whose only consumer is a person needs a byte gate more
# than the others, not less.** language-configuration.json joined as the FIFTH
# output for that same reason: it is read by the editor alone, and a broken
# wordPattern degrades double-click and rename quietly rather than loudly.
run_case codegen/scaly.dsl scaly.sgm \
  packages/scalyc/0.1.0/scalyc/compiler/Syntax.scaly \
  packages/scalyc/0.1.0/scalyc/compiler/parser.scaly \
  packages/scalyls/0.1.0/scalyls/grammar.scaly \
  editors/vscode/syntaxes/scaly.tmLanguage.json \
  editors/vscode/language-configuration.json || fail=1
run_case codegen/test-expressions.dsl tests/expressions.sgm \
  docs/scaly/generated-expressions.xml tests/selfhosted/expressions__*.scaly || fail=1
run_case codegen/test-definitions.dsl tests/definitions.sgm \
  docs/scaly/generated-definitions.xml tests/selfhosted/definitions__*.scaly || fail=1
run_case codegen/test-controlflow.dsl tests/controlflow.sgm \
  docs/scaly/generated-controlflow.xml tests/selfhosted/controlflow__*.scaly || fail=1

if [ "$fail" -ne 0 ]; then exit 1; fi
echo "dazzle-codegen: PASS"
