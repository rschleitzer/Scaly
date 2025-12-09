#!/bin/bash
export SP_ENCODING=utf-8

# Generate HTML
openjade -t sgml -d dsssl-stylesheets/html/docbook.dsl scaly-spec.xml
mkdir -p website/lang
mv *.htm website/lang/
mv website/lang/book1.htm website/lang/index.html
sed -i '' 's/book1\.htm/index.html/g' website/lang/*.htm website/lang/*.html

# Generate RTF
openjade -t rtf -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml 2>/dev/null
mv scaly-spec.rtf website/lang/
