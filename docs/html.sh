#!/bin/bash
# The specification as HTML, set by the Scaly DSSSL engine (dazzle -t sgml)
# with the DocBook stylesheets.
#
# The engine is the one ./mkp builds out of the dazzle checkout beside this
# repository (scalyc/build/dazzle); it reads its DSSSL prolog and catalog from
# that checkout, which SCALY_HOME names for the run.
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
DAZZLE_REPO="${DAZZLE_REPO:-../../dazzle}"
[ -x "$DAZZLE" ] || { echo "html.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
rm -f ./*.htm
SCALY_HOME="$(cd "$DAZZLE_REPO" && pwd)" "$DAZZLE" -t sgml -d dsssl-stylesheets/html/docbook.dsl scaly-spec.xml || exit 1
mkdir -p website/lang
mv ./*.htm website/lang/
mv website/lang/book1.htm website/lang/index.html
sed -i.bak 's/book1\.htm/index.html/g' website/lang/*.htm website/lang/*.html && rm -f website/lang/*.bak
