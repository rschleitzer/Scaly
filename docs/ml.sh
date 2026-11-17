#!/bin/bash
# Machine Learning with Scaly on the web, in English and in German, set by
# the Scaly DSSSL engine with the specification's stylesheet
# (dsssl/scaly-book.dsl) into website/ml/: the English pages as field.htm,
# index.html and searchindex.js, the German ones beside them as
# field.de.htm, index.de.html and searchindex.de.js, and book.css, book.js,
# theme.js and words.js as html.sh puts them, and fonts/ with the formulas'
# font. Then figures.py runs the pictures' programs (ml/fig/) and puts their
# pictures into fig/ as PNG, and runnable.py gives the listings that run their Run button
# (SCALY, default scalyc/build/scaly).
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
[ -x "$DAZZLE" ] || { echo "ml.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
OUT=website/ml
rm -rf "$OUT"
mkdir -p "$OUT"
for book in scaly-ml.xml scaly-ml.de.xml; do
  rm -f ./*.htm index*.html searchindex*.js
  "$DAZZLE" -t sgml -d dsssl/scaly-book.dsl "$book" || exit 1
  mv ./*.htm index*.html searchindex*.js "$OUT/"
done
cp book/book.css book/book.js book/theme.js book/words.js "$OUT/"
cp -R book/fonts "$OUT/"
SCALY="${SCALY:-../scalyc/build/scaly}"
[ -x "$SCALY" ] || { echo "ml.sh: no tool at $SCALY for the pictures and the Run buttons -- ./build.sh builds it"; exit 1; }
python3 figures.py "$OUT" "$SCALY" ml/fig/*.scaly || exit 1
python3 runnable.py "$OUT" "$SCALY" || exit 1
