#!/bin/bash
# The specification as PDF, set by the Scaly DSSSL engine (dazzle -t pdf).
#
# The engine is the one ./mkp builds out of the dazzle checkout beside this
# repository (scalyc/build/dazzle).
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
[ -x "$DAZZLE" ] || { echo "pdf.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
mkdir -p website/lang
"$DAZZLE" -t pdf -o website/lang/scaly-spec.pdf \
    -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml
