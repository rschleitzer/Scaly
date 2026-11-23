#!/bin/bash
# tests/mlbook/run.sh [compiler] -- Machine Learning with Scaly (docs/scaly-ml.xml):
# every program of the book runs and prints what its screen says
# (tests/book/run.sh does that, with the pool and with one worker), and the
# German book (docs/scaly-ml.de.xml) is the English one translated: the same
# chapters and sections, by id and in order, and in each the same programs
# and screens, character for character, and the same formulas (parity.py);
# every formula reaches the page as MathML a browser sets (mathml.py),
# every picture's program draws its picture (docs/figures.py), and both
# books set as PDF with their formula font (docs/ml-pdf.sh's way), and
# every program beside the book (docs/ml/prog/) prints what it must.
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
# the books as PDF (docs/ml-pdf.sh's way, beside those pictures): the engine
# sets both without a word on stderr, with the formulas' font embedded
if [ -x "$DAZZLE" ]; then
  cp docs/ml/shot/*.png "$FIG/fig/"
  FONTS="$ROOT/docs/book/fonts"
  for d in /System/Library/Fonts/Supplemental /System/Library/Fonts /Library/Fonts /usr/share/fonts; do
    [ -d "$d" ] && FONTS="$FONTS:$d"
  done
  for book in scaly-ml scaly-ml.de; do
    if ! (cd "$FIG" && DAZZLE_FONTS="$FONTS" SP_ENCODING=utf-8 "$DAZZLE" -t pdf -o "$book.pdf" \
            -d "$ROOT/docs/dsssl/scaly-ml-print.dsl" "$ROOT/docs/$book.xml" 2> "$book.err") \
       || [ -s "$FIG/$book.err" ]; then
      echo "mlbook: the PDF $book does not set"; head -5 "$FIG/$book.err"; rc=1
    elif ! grep -aq "/FontFile3" "$FIG/$book.pdf"; then
      echo "mlbook: the PDF $book has no formula font"; rc=1
    fi
  done
  [ $rc -eq 0 ] && echo "pdf: both books set"
else
  echo "pdf: SKIPPED (no engine at $DAZZLE)"
fi
rm -rf "$FIG"
# every picture the books show has its program (docs/ml/fig) or is a picture
# of a page (docs/ml/shot)
for ref in $(grep -ho 'fileref="fig/[^"]*"' docs/ml/field.xml docs/ml/de/field.xml | sed 's/fileref="fig\/\(.*\)"/\1/' | sort -u); do
  [ -f "docs/ml/fig/${ref%.*}.scaly" ] || [ -f "docs/ml/shot/$ref" ] || { echo "mlbook: no source for fig/$ref"; rc=1; }
done
# the programs beside the book (docs/ml/prog/), each against what it says it
# matches: circle.scaly, the circle learned with matrices, prints what the
# field's own program prints for it (`field 0 500`), digit for digit
SCALY="$("$ROOT/tools/scaly-of.sh" "$BIN")"
"$SCALY" run docs/ml/prog/circle.scaly > "$FIG.circle" 2>&1
"$SCALY" run web/field/field.scaly 0 500 > "$FIG.field" 2>&1
if [ -s "$FIG.circle" ] && cmp -s "$FIG.circle" "$FIG.field"; then
  echo "prog: circle.scaly prints what field 0 500 prints"
else
  echo "mlbook: circle.scaly does not print what field 0 500 prints"; diff "$FIG.field" "$FIG.circle" | head -5; rc=1
fi
# the Python versions beside it (chapter 1 says so): circle.py prints what
# circle.scaly prints up to the 200th step, digit for digit (after that the
# system's tanh and Scaly's differ in a last place), and both Python versions
# get the same test points right
"$SCALY" run docs/ml/prog/circle.scaly > "$FIG.circle" 2>&1
if command -v python3 > /dev/null; then
  python3 docs/ml/prog/circle.py > "$FIG.py" 2>&1
  if [ "$(head -3 "$FIG.py")" = "$(head -3 "$FIG.circle")" ] && [ "$(tail -1 "$FIG.py")" = "$(tail -1 "$FIG.circle")" ]; then
    echo "prog: circle.py prints what circle.scaly prints to step 200, and its last line"
  else
    echo "mlbook: circle.py does not print what circle.scaly prints"; diff "$FIG.circle" "$FIG.py" | head -5; rc=1
  fi
  if python3 -c "import numpy" 2> /dev/null; then
    python3 docs/ml/prog/circle_numpy.py > "$FIG.py" 2>&1
    if [ "$(tail -1 "$FIG.py")" = "$(tail -1 "$FIG.circle")" ]; then
      echo "prog: circle_numpy.py gets the same test points right"
    else
      echo "mlbook: circle_numpy.py ends otherwise: $(tail -1 "$FIG.py")"; rc=1
    fi
  else
    echo "prog: circle_numpy.py SKIPPED (no numpy)"
  fi
  rm -f "$FIG.py"
else
  echo "prog: circle.py SKIPPED (no python3)"
fi
# tape.scaly, the circle learned with a tape (chapter 4), prints what
# tests/mlbook/tape.expected holds -- chapter 4's table shows three of its
# lines beside circle.scaly's, so a line that moves owes a look at it --,
# every loss within a few last places of circle.scaly's (the sums are
# rounded in another order), and the same last line
SCALY_POISON=1 "$SCALY" run docs/ml/prog/tape.scaly > "$FIG.tape" 2>&1
if ! cmp -s "$FIG.tape" tests/mlbook/tape.expected; then
  echo "mlbook: tape.scaly's numbers moved (tests/mlbook/tape.expected):"; diff tests/mlbook/tape.expected "$FIG.tape" | head -8; rc=1
elif python3 - "$FIG.circle" "$FIG.tape" <<'NEAR'
import re, sys
a, b = (open(f).read().split('\n') for f in sys.argv[1:3])
num = re.compile(r'[0-9.]+e?-?[0-9]*')
ok = len(a) == len(b) and a[-2] == b[-2] and a[-2].startswith('test right')
for x, y in zip(a[:-2], b[:-2]):
    nx, ny = x.split(), y.split()
    ok = ok and len(nx) == len(ny) and all(
        u == v or (num.fullmatch(u) and abs(float(u) - float(v)) <= 1e-14 * abs(float(u)))
        for u, v in zip(nx, ny))
sys.exit(0 if ok else 1)
NEAR
then
  echo "prog: tape.scaly prints what circle.scaly prints, to the last places"
else
  echo "mlbook: tape.scaly does not print what circle.scaly prints"; diff "$FIG.circle" "$FIG.tape" | head -5; rc=1
fi
rm -f "$FIG.tape"
rm -f "$FIG.circle" "$FIG.field"
# the model page's numbers (model_check.py: web/model/model.scaly's routines
# on web/model/index.html's start), which chapter 2's boxes tell in words:
#   descent       two of six right, all six after a good twenty steps, the
#                 goal after a good eighty
#   descent_slow  with the rate 0.01 "much slower"
#   saturated     w1 = w2 = -3: almost all wrong, stops after a hundred steps
#   disc_plain    a disc and a ring for the plain neuron: it stops, no headway
#   squares       given x² and y²: all right after a good twenty steps, the
#                 goal after a good two hundred
#   xor           the network: half right, all twelve after nearly thirty
#                 steps, the goal after a good hundred
# A line that moves owes a look at the sentence that tells it.
if NODE=$(tools/node.sh 2> /dev/null); then
  if python3 tests/mlbook/model_check.py "$NODE" "$SCALY" > "$FIG.model" 2>&1 && cmp -s "$FIG.model" tests/mlbook/model.expected; then
    echo "prog: the model page's numbers are the ones chapter 2 tells"
  else
    echo "mlbook: the model page's numbers moved (tests/mlbook/model.expected):"; diff tests/mlbook/model.expected "$FIG.model" | head -8; rc=1
  fi
  rm -f "$FIG.model"
else
  echo "prog: the model page's numbers SKIPPED (no node that starts)"
fi
# every program beside the book builds (product.scaly prints times, so it is
# built, not run)
for p in docs/ml/prog/*.scaly; do
  if ! "$SCALY" build "$p" -o "$FIG.prog" > "$FIG.prog.log" 2>&1; then
    echo "mlbook: $p does not build"; head -5 "$FIG.prog.log"; rc=1
  fi
done
rm -f "$FIG.prog" "$FIG.prog.log"
# every piece of a program the books show (role="fragment") stands in a
# program beside the book (docs/ml/prog/), character for character
python3 - "${CHAPTERS[@]}" docs/ml/de/*.xml <<'PY' || rc=1
import glob, html, re, sys
progs = [open(p).read() for p in glob.glob('docs/ml/prog/*.scaly')]
bad = n = 0
for f in sys.argv[1:]:
    for m in re.finditer(r'<programlisting role="fragment">(.*?)</programlisting>', open(f).read(), re.S):
        n += 1
        if not any(html.unescape(m.group(1)) in p for p in progs):
            bad += 1
            print('mlbook: a piece of %s stands in no program of docs/ml/prog: %s' % (f, m.group(1).split('\n')[0]))
print('prog: %d pieces of programs, each in its program' % n if not bad else '', end='' if bad else '\n')
sys.exit(1 if bad else 0)
PY
# the chapters that hold a whole program (the code of the pages is not the
# book's to run, and a piece of a program, role="fragment", is checked
# above with the program it is cut from)
LISTED=()
for c in "${CHAPTERS[@]}"; do grep '<programlisting' "$c" | grep -vq 'role="fragment"' && LISTED+=("$c"); done
if [ ${#LISTED[@]} -gt 0 ]; then
  BOOK_NAME=mlbook tests/book/run.sh "$BIN" "${LISTED[@]}" || rc=1
else
  echo "mlbook: no program in the book"
fi
exit $rc
