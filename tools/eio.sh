#!/bin/bash
# Compile the evented-I/O backend shim for THIS host into an object file
# (default /tmp/eio.o). One C source covers both backends — kqueue on
# darwin, epoll on linux — selected by the preprocessor, so no dispatch is
# needed here (unlike tools/fcontext.sh, which picks the ABI's .S file).
# The backend cannot live in Scaly code: the committed seed ships one
# scaly.ll for all four targets. Every libscaly.a build archives this
# object next to fcontext.o (see tools/build-from-seed.sh,
# tools/bootstrap.sh, tools/install.sh).
#
# Usage: tools/eio.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/eio.o}
${CLANG:-clang} -O2 -c packages/scaly/0.1.0/scaly/fiber/eio.c -o "$OUT"
