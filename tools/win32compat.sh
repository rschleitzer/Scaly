#!/bin/bash
# Compile the Win64 POSIX-compatibility shim into an object (stage 7, brocken 3).
#
# Windows ONLY. The twelve symbols in posixcompat_windows.c exist natively on every
# other target, so this object is archived into libscaly.a on Windows and does
# not exist elsewhere — unlike eio.o/ctime.o/fcontext.o, which every target
# builds. The source guards itself with #ifdef _WIN32 as well, so compiling it
# by accident on a POSIX host is harmless rather than an error.
#
# Usage: tools/win32compat.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/win32compat.o}
SRC=packages/scaly/0.1.1/scaly/win32/posixcompat_windows.c

# Link libraries the caller must add when this object is in the link:
#   ws2_32   WSAPoll (poll)
# Everything else it uses is kernel32 + the CRT, both linked by default.
${CLANG:-clang} -O2 -c "$SRC" -o "$OUT"
