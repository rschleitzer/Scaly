#!/bin/bash
export SP_ENCODING=utf-8

# Generate RTF
dotnet run --project ~/repos/openjade-net/src/Jade -- -t rtf -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml 2>/dev/null
mkdir -p website/lang
mv scaly-spec.rtf website/lang/
