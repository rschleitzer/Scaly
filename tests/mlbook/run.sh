#!/bin/bash
# tests/mlbook/run.sh [compiler] -- Machine Learning with Scaly (docs/scaly-ml.xml):
# every program of the book runs and prints what its screen says
# (tests/book/run.sh does that, with the pool and with one worker), and the
# German book (docs/scaly-ml.de.xml) is the English one translated: the same
# chapters and sections, by id and in order, and in each the same programs
# and screens, character for character, and the same formulas (parity.py);
# every formula reaches the page as MathML a browser sets (mathml.py), and
# every picture's program draws its picture (docs/figures.py).
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT" || exit 1
rc=0
python3 "$HERE/parity.py" docs/scaly-ml.xml docs/scaly-ml.de.xml || rc=1
# the formulas as the stylesheet sets them (mathml.py), where the engine is
DAZZLE="${DAZZLE:-$ROOT/scalyc/build/dazzle}"
if [ -x "$DAZZLE" ]; then
  TMP="$(mktemp -d)"
  for book in scaly-ml.xml scaly-ml.de.xml; do
    (cd "$TMP" && SP_ENCODING=utf-8 \
       "$DAZZLE" -t sgml -d "$ROOT/docs/dsssl/scaly-book.dsl" "$ROOT/docs/$book") || rc=1
  done
  python3 "$HERE/mathml.py" "$TMP"/*.htm || rc=1
  rm -rf "$TMP"
else
  echo "mathml: SKIPPED (no engine at $DAZZLE -- ./mkp builds it)"
fi
CHAPTERS=($(python3 "$HERE/parity.py" --chapters docs/scaly-ml.xml))
# every picture's program (docs/ml/fig) runs and writes its picture
# (docs/figures.py)
FIG="$(mktemp -d)"
python3 docs/figures.py "$FIG" "$("$ROOT/tools/scaly-of.sh" "$BIN")" docs/ml/fig/*.scaly || rc=1
rm -rf "$FIG"
BOOK_NAME=mlbook tests/book/run.sh "$BIN" "${CHAPTERS[@]}" || rc=1
exit $rc
