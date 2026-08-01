#!/bin/bash
# Compile the civil-time shim for THIS host into an object file (default
# /tmp/ctime.o). One C source for all four targets; nothing in it is
# OS-conditional, but `struct tm` (a libc struct layout) and the variadic
# sscanf/sprintf calls put it on the C side of the containment rule — see the
# header of packages/scaly/0.1.0/scaly/time/ctime.c.
# Every libscaly.a build archives this object next to fcontext.o and eio.o,
# and every scalyc/scalyls link lists it explicitly (see tools/seed.sh,
# tools/build-from-seed.sh, tools/verify-seed.sh, tools/install.sh).
#
# Usage: tools/ctime.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/ctime.o}
${CLANG:-clang} -O2 -c packages/scaly/0.1.0/scaly/time/ctime.c -o "$OUT"
