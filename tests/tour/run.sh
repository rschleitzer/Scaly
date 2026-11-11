#!/bin/bash
# tests/tour/run.sh [compiler] -- the tour's programs run. Every
# <programlisting> of the tutorial's "A Tour of Scaly" section
# (docs/scaly/chapters/tutorial.xml, section tut-tour) is a step of the tour
# on the web page (web/repl/tour.html), with a Run button: each must be a
# complete program that `scaly run` runs with rc 0 and that prints. A listing
# marked role="fragment" is prose -- part of a program, two files, a package
# line -- and is not run. Found when it was written: the tour's procedure
# example had not compiled since the write rules (R1, R2), and its other
# programs printed nothing.
#
# Then the Scaly Playground's own chapters (docs/scaly-playground.xml, its
# chapters under docs/playground/), whose programs may also draw instead of
# printing: each runs in a scratch directory (a drawing is a file), and must
# end with rc 0 and print or draw. And the playground in German
# (docs/scaly-playground.de.xml, docs/playground/de/: the tour translated,
# then the chapters): every section must have the id of its English twin
# and as many programs (parity.py), and its programs run alike.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
export SCALY_CACHE="$TMP/cache"

python3 "$HERE/listings.py" docs/scaly/chapters/tutorial.xml tut-tour "$TMP/steps" || exit 1
for chapter in docs/playground/*.xml docs/playground/de/*.xml; do
  case "$chapter" in */de/*) prefix=de- ;; *) prefix= ;; esac
  for section in $(sed -n 's/^  <section xml:id="\([^"]*\)">.*/\1/p' "$chapter"); do
    python3 "$HERE/listings.py" "$chapter" "$section" "$TMP/steps/pg-$prefix$section" empty || exit 1
  done
done
python3 "$HERE/parity.py" 'docs/scaly/chapters/tutorial.xml#tut-tour' docs/playground/de/tour.xml || exit 1
for chapter in docs/playground/*.xml; do
  python3 "$HERE/parity.py" "$chapter" "docs/playground/de/$(basename "$chapter")" || exit 1
done
pass=0; fail=0
for f in "$TMP"/steps/*.scaly "$TMP"/steps/pg-*/*.scaly; do
  name=$(basename "$f" .scaly)
  rm -rf "$TMP/cwd"; mkdir -p "$TMP/cwd"
  out=$(cd "$TMP/cwd" && SCALY_HOME="$ROOT" "$SCALY" run "$f" < /dev/null 2>&1); rc=$?
  drew=$(ls "$TMP/cwd" 2>/dev/null)
  if [ "$rc" = 0 ] && { [ -n "$out" ] || [ -n "$drew" ]; }; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    echo "FAIL $name: rc=$rc $(echo "$out" | head -3 | tr '\n' '|')"
  fi
done
echo "tour: $pass PASS, $fail FAIL"
[ "$fail" = 0 ]
