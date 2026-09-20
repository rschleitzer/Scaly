#!/bin/bash
# Compile the Win32 POSIX-compatibility shim into an object file (default
# /tmp/posixcompat.o). Windows-only by construction: it defines the POSIX
# names the MSVC CRT lacks or gets wrong (`creat`, `close`, `dlopen`, the
# aligned-alloc pair, the stack-limit question — see the header of
# packages/scaly/0.1.0/scaly/win32/posixcompat.c), so on any other host there
# is nothing to compile and the script refuses rather than emitting an empty
# object something would link without noticing.
# The sibling scripts (tools/fcontext.sh, eio.sh, ctime.sh, panic.sh) select
# their Windows source themselves; this one exists because the fifth shim has
# no POSIX counterpart to select from. tools/win-lto.sh and the stage links on
# the Windows box list it beside the other four (CI's windows job: pc.o).
#
# Usage: tools/posixcompat.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/posixcompat.o}
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*|Windows*) ;;
  *) echo "posixcompat: FAIL — Windows-only shim; host is $(uname -s)" >&2; exit 1 ;;
esac
${CLANG:-clang} -O2 -c packages/scaly/0.1.0/scaly/win32/posixcompat.c -o "$OUT"
