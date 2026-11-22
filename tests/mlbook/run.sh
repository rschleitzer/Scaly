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
rm -f "$FIG.circle" "$FIG.field"
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
