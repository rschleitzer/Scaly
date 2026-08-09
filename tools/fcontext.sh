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
# of fcontext_x86_64_win.S. `uname -s` under Git Bash / MSYS reports
# MINGW64_NT-* or MSYS_NT-*, which is what the pattern below matches.
#
# Usage: tools/fcontext.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/fcontext.o}
FIB=packages/scaly/0.1.0/scaly/fiber
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*|Windows*) SRC=$FIB/fcontext_x86_64_win.S ;;
  *)
    case "$(uname -m)" in
      arm64|aarch64) SRC=$FIB/fcontext_arm64.S ;;
      x86_64|amd64)  SRC=$FIB/fcontext_x86_64.S ;;
      *) echo "fcontext: FAIL — unsupported architecture $(uname -m)"; exit 1 ;;
    esac
    ;;
esac

${CLANG:-clang} -c "$SRC" -o "$OUT"
