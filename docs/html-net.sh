#!/bin/bash
export SP_ENCODING=utf-8

# Generate HTML
dotnet run --project ~/repos/openjade-net/src/Jade -- -t sgml -d dsssl-stylesheets/html/docbook.dsl scaly-spec.xml
mkdir -p website/lang
mv *.htm website/lang/
mv website/lang/book1.htm website/lang/index.html
sed -i '' 's/book1\.htm/index.html/g' website/lang/*.htm website/lang/*.html
