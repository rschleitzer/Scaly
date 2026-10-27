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
pass=0; fail=0
for f in "$TMP"/steps/*.scaly; do
  name=$(basename "$f" .scaly)
  out=$(SCALY_HOME="$ROOT" "$SCALY" run "$f" < /dev/null 2>&1); rc=$?
  if [ "$rc" = 0 ] && [ -n "$out" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    echo "FAIL $name: rc=$rc $(echo "$out" | head -3 | tr '\n' '|')"
  fi
done
echo "tour: $pass PASS, $fail FAIL"
[ "$fail" = 0 ]
