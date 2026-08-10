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
# Windows gets its OWN file rather than a third #ifdef arm: kqueue and epoll
# differ only in their backend half and share the rest verbatim, while IOCP
# shares nothing — a completion model instead of a readiness one, different
# socket calls, different error reporting. The reasoning is written out at the
# top of eio_win.c. Selection is per OS here, as tools/fcontext.sh selects per
# ABI; `uname -s` under Git Bash / MSYS reports MINGW64_NT-* or MSYS_NT-*.
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*|Windows*) SRC=packages/scaly/0.1.0/scaly/fiber/eio_win.c ;;
  *)                             SRC=packages/scaly/0.1.0/scaly/fiber/eio.c ;;
esac

${CLANG:-clang} -O2 -c "$SRC" -o "$OUT"
