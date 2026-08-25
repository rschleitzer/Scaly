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

# ★`SCALY_ARCH` cross-builds this shim for another ARCH on the same host. The
# scripts otherwise read `uname -m`, i.e. the HOST, which is right for every
# normal build and is exactly what blocks the one useful cross case: an Apple
# Silicon box can produce and RUN x86_64-apple-darwin (universal SDK plus
# Rosetta 2), the only one of the four LP64 targets that has never had a
# runner. See tests/target/rosetta.sh.
#
# ★It is `SCALY_ARCH` and not `ARCH` on purpose: a bare `ARCH` is a name a
# developer environment may already EXPORT, and an exported name silently
# steers a script that only meant to read its own variable (the `LIB` trap in
# the root CLAUDE.md, same shape).
#
# ★`-arch` is a Darwin/clang facility, not a general cross-compile switch: it
# works because macOS ships a universal SDK. Setting SCALY_ARCH anywhere else
# is refused rather than half-honoured — cross-building the C shims on Linux
# needs a real cross toolchain, and a flag that quietly did nothing would make
# the resulting archive wrong in a way only the link reports.
ARCHFLAG=""
if [ -n "${SCALY_ARCH:-}" ]; then
    if [ "$(uname -s)" != "Darwin" ]; then
        echo "$(basename "$0"): FAIL — SCALY_ARCH is Darwin-only (universal SDK); host is $(uname -s)" >&2
        exit 1
    fi
    ARCHFLAG="-arch $SCALY_ARCH"
fi

${CLANG:-clang} $ARCHFLAG -O2 -c "$SRC" -o "$OUT"
