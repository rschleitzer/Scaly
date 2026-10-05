#!/bin/bash
# The specification, both ways: HTML and PDF, by the Scaly DSSSL engine.
cd "$(dirname "$0")" || exit 1
./html.sh && ./pdf.sh
