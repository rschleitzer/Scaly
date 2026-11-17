#!/bin/bash
# tests/mlbook/run.sh [compiler] -- Machine Learning with Scaly (docs/scaly-ml.xml):
# every program of the book runs and prints what its screen says
# (tests/book/run.sh does that, with the pool and with one worker), and the
# German book (docs/scaly-ml.de.xml) is the English one translated: the same
# chapters and sections, by id and in order, and in each the same programs
# and screens, character for character (parity.py).
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT" || exit 1
rc=0
python3 "$HERE/parity.py" docs/scaly-ml.xml docs/scaly-ml.de.xml || rc=1
CHAPTERS=($(python3 "$HERE/parity.py" --chapters docs/scaly-ml.xml))
BOOK_NAME=mlbook tests/book/run.sh "$BIN" "${CHAPTERS[@]}" || rc=1
exit $rc
