#!/bin/bash
export SP_ENCODING=utf-8

# Generate FOT (Flow Object Tree)
openjade -t fot -o scaly-spec.fot -d dsssl-stylesheets/print/docbook.dsl scaly-spec.xml
