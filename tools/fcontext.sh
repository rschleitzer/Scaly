#!/bin/bash
# Assemble the fiber context-switch primitives for THIS host into an object
# file (default /tmp/fcontext.o). The .S sources live in the scaly package
# (packages/scaly/0.1.0/scaly/fiber/); one file per ABI — AAPCS64 and SysV
# x86-64 cover all four LP64 targets, darwin/linux differences are absorbed
# by cpp inside the files. Every libscaly.a build archives this object next
# to libscaly.o (see tools/build-from-seed.sh, tools/bootstrap.sh,
# tools/install.sh).
#
# Usage: tools/fcontext.sh [output.o]
set -e
cd "$(dirname "$0")/.."

OUT=${1:-/tmp/fcontext.o}
case "$(uname -m)" in
  arm64|aarch64) SRC=packages/scaly/0.1.0/scaly/fiber/fcontext_arm64.S ;;
  x86_64|amd64)  SRC=packages/scaly/0.1.0/scaly/fiber/fcontext_x86_64.S ;;
  *) echo "fcontext: FAIL — unsupported architecture $(uname -m)"; exit 1 ;;
esac

${CLANG:-clang} -c "$SRC" -o "$OUT"
