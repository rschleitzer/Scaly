#!/bin/bash
export SP_ENCODING=utf-8

# Generate RTF (a probe of the engines; the website hands out the PDF of pdf.sh)
dotnet run --project ~/repos/openjade-net/src/Jade -- -t rtf -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml 2>/dev/null
