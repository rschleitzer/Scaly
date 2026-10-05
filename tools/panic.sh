#!/bin/bash
# Compile the panic catch-point shim for THIS host into an object file
# (default /tmp/panic.o). One C source for all four targets; nothing in it is
# OS-conditional, but `jmp_buf` is an OS- and arch-specific struct layout AND
# `setjmp` cannot be wrapped in a library function at all — see the header of
# packages/scaly/0.1.0/scaly/memory/panic.c.
# Every libscaly.a build archives this object next to fcontext.o and eio.o,
# and every scalyc/scalyls link lists it explicitly (see tools/seed.sh,
# tools/build-from-seed.sh, tools/verify-seed.sh, tools/install.sh).
#
# Usage: tools/panic.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/panic.o}
# ★`SCALY_ARCH` cross-builds this shim for another ARCH on the same host. The
# scripts otherwise read `uname -m`, i.e. the HOST, which is right for every
# normal build and is exactly what blocks the one useful cross case: an Apple
# Silicon box can produce and RUN x86_64-apple-darwin (universal SDK plus
# Rosetta 2), the only one of the four LP64 targets that has never had a
# runner. See tests/target/rosetta.sh.
#
# ★It is `SCALY_ARCH` and not `ARCH` on purpose: a bare `ARCH` is a name a
# developer environment may already EXPORT, and an exported name silently
# steers a script that only meant to read its own variable (the `LIB` trap,
# same shape).
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

${CLANG:-clang} $ARCHFLAG -O2 -c packages/scaly/0.1.0/scaly/memory/panic.c -o "$OUT"
