#!/bin/bash
export SP_ENCODING=utf-8

# Generate PDF with dazzle.net (https://github.com/rschleitzer/dazzle-net beside this checkout).
# It needs the OpenJade package dazzle.net pins; while that version is not yet
# published, pack it from ~/repos/openjade-net into a local feed and restore
# dazzle.net with `--source <feed>` first.
DAZZLE_NET="${DAZZLE_NET:-$HOME/repos/dazzle-net/src/cli}"
mkdir -p website/lang
dotnet run --project "$DAZZLE_NET" -- -t pdf -o website/lang/scaly-spec.pdf \
    -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml
