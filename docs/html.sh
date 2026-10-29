#!/bin/bash
# The specification as a book on the web, set by the Scaly DSSSL engine
# (dazzle -t sgml) with dsssl/scaly-book.dsl: a page per chapter and appendix,
# index.html with the contents and searchindex.js, written into website/lang/
# beside what book/ holds -- book.css, book.js, theme.js and words.js (./mkp
# generates that one from scaly.sgm). The PDF there (pdf.sh) stays.
#
# The engine is the one ./mkp builds out of the dazzle checkout beside this
# repository (scalyc/build/dazzle).
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
[ -x "$DAZZLE" ] || { echo "html.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
rm -f ./*.htm index.html searchindex.js
"$DAZZLE" -t sgml -d dsssl/scaly-book.dsl scaly-spec.xml || exit 1
mkdir -p website/lang
rm -f website/lang/*.htm website/lang/index.html website/lang/searchindex.js
mv ./*.htm index.html searchindex.js website/lang/
cp book/book.css book/book.js book/theme.js book/words.js website/lang/
