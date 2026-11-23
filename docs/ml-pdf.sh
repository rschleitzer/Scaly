#!/bin/bash
# Machine Learning with Scaly as PDF, in English and in German, set by the
# Scaly DSSSL engine (dazzle -t pdf) with the print layer
# dsssl/scaly-ml-print.dsl over the DocBook print stylesheet, into
# website/ml/scaly-ml.pdf and scaly-ml.de.pdf. The pictures are those of the
# web pages, so docs/ml.sh runs first; the book is set in website/ml, where
# they lie in fig/. The formulas' font is the web pages' (book/fonts), and
# so is the text's, Latin Modern Roman; the others are the system's.
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
[ -x "$DAZZLE" ] || { echo "ml-pdf.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
DAZZLE="$(cd "$(dirname "$DAZZLE")" && pwd)/$(basename "$DAZZLE")"
[ -d website/ml/fig ] || { echo "ml-pdf.sh: no pictures in website/ml/fig -- run ml.sh first"; exit 1; }
HERE="$(pwd)"
FONTS="$HERE/book/fonts"
for d in /System/Library/Fonts/Supplemental /System/Library/Fonts /Library/Fonts /usr/share/fonts /usr/local/share/fonts; do
  [ -d "$d" ] && FONTS="$FONTS:$d"
done
export DAZZLE_FONTS="$FONTS"
cd website/ml || exit 1
for book in scaly-ml scaly-ml.de; do
  "$DAZZLE" -t pdf -o "$book.pdf" -d "$HERE/dsssl/scaly-ml-print.dsl" "$HERE/$book.xml" || exit 1
done
