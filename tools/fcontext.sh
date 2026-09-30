#!/bin/bash
# Assemble the fiber context-switch primitives for THIS host into an object
# file (default /tmp/fcontext.o). The .S sources live in the scaly package
# (packages/scaly/0.1.0/scaly/fiber/); one file per ABI — AAPCS64 and SysV
# x86-64 cover all four LP64 targets, darwin/linux differences are absorbed
# by cpp inside the files. Every libscaly.a build archives this object next
# to libscaly.o (see tools/build-from-seed.sh, tools/bootstrap.sh,
# tools/install.sh).
#
# The selection is per ABI, NOT per architecture — which is why Windows gets
# its own arm below rather than sharing the x86-64 file. Win64 differs from
# SysV in its argument registers, in keeping XMM6-15 callee-saved, in shadow
# space, and in the TIB stack bounds; the reasoning is written out at the top
# of fcontext_x86_64_windows.S. `uname -s` under Git Bash / MSYS reports
# MINGW64_NT-* or MSYS_NT-*, which is what the pattern below matches.
#
# Usage: tools/fcontext.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/fcontext.o}
FIB=packages/scaly/0.1.0/scaly/fiber
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*|Windows*) SRC=$FIB/fcontext_x86_64_windows.S ;;
  *)
    case "${SCALY_ARCH:-$(uname -m)}" in
      arm64|aarch64) SRC=$FIB/fcontext_arm64.S ;;
      x86_64|amd64)  SRC=$FIB/fcontext_x86_64.S ;;
      *) echo "fcontext: FAIL — unsupported architecture ${SCALY_ARCH:-$(uname -m)}"; exit 1 ;;
    esac
    ;;
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

${CLANG:-clang} $ARCHFLAG -c "$SRC" -o "$OUT"
