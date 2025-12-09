#!/bin/bash
export SP_ENCODING=utf-8

# Generate FOT (Flow Object Tree)
dotnet run --project ~/repos/openjade-net/src/Jade -- -t fot -o scaly-spec.fot -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml
