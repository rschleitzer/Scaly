#!/bin/bash
# The specification as PDF, set by the Scaly DSSSL engine (dazzle -t pdf).
#
# The engine is the one ./mkp builds out of the dazzle checkout beside this
# repository (scalyc/build/dazzle); it reads its DSSSL prolog and catalog from
# that checkout, which SCALY_HOME names for the run.
cd "$(dirname "$0")" || exit 1
export SP_ENCODING=utf-8

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
DAZZLE_REPO="${DAZZLE_REPO:-../../dazzle}"
[ -x "$DAZZLE" ] || { echo "pdf.sh: no engine at $DAZZLE -- run ./mkp with the dazzle checkout beside this one"; exit 1; }
mkdir -p website/lang
SCALY_HOME="$(cd "$DAZZLE_REPO" && pwd)" "$DAZZLE" -t pdf -o website/lang/scaly-spec.pdf \
    -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml
