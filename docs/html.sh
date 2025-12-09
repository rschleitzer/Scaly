#!/bin/bash
export SP_ENCODING=utf-8

# Generate HTML
openjade -t sgml -d dsssl-stylesheets/html/docbook.dsl scaly-spec.xml
mkdir -p lang
mv *.htm lang/
mv lang/book1.htm lang/index.html
sed -i '' 's/book1\.htm/index.html/g' lang/*.htm lang/*.html
