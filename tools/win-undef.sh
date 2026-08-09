#!/bin/bash
# The Windows substrate work list, MEASURED — every symbol a Win64 link of the
# Scaly runtime must resolve, classified by who owes it.
#
# Why this exists as an instrument rather than a paragraph: the list is the
# stage-7 backlog, it shrinks as brocken land, and a number in a document goes
# stale silently. Run it to see what is left.
#
# What it measures, and the constraint that makes the number large: the runtime
# archive is ONE object with ONE .text section (no per-function COMDATs), so a
# COFF linker cannot discard anything — a program that touches the runtime AT
# ALL must resolve every symbol below, not just the ones on its own call path.
# That is the same "the archive is one object" mechanic as the -lm rule in
# CLAUDE.md, and it is why "link something that uses the runtime" is not a
# single rung: it forces brocken 1, 2 and 3 at once.
#
# Usage: tools/win-undef.sh [compiler] [triple]
cd "$(dirname "$0")/.." || exit 1
SC=${1:-scalyc/build/scalyc}
TRIPLE=${2:-x86_64-pc-windows-msvc}
OBJ=/tmp/win_undef_$$.o

# shellcheck disable=SC1091
source tools/llvm-env.sh > /dev/null 2>&1
NM="${LLVM18:-/opt/homebrew/opt/llvm@18}/bin/llvm-nm"
command -v llvm-nm > /dev/null 2>&1 && NM=$(command -v llvm-nm)

"$SC" -c --target "$TRIPLE" --no-prelude --no-tests -o "$OBJ" \
      packages/scaly/0.1.0/scaly.scaly > /dev/null 2>&1 \
  || { echo "win-undef: FAIL — cross-emit for $TRIPLE failed"; exit 1; }

ALL=$("$NM" -u "$OBJ" | sed 's/^ *U //' | sort -u)
rm -f "$OBJ"

# Ours: the shim symbols, owed by brocken 1 (fcontext) and 2 (IOCP).
OURS=$(printf '%s\n' "$ALL" | grep '^scaly_')
# Present in the MSVC CRT under this exact name, or under an underscore alias
# the CRT also exports — no work beyond linking.
CRT='^(abort|atexit|exit|fclose|fopen|fread|free|fwrite|getenv|memcmp|memcpy|memset|rewind|strcmp|strlen|strdup|write|close|access|mkdir|rmdir|expf|logf|powf|sqrtf|tanhf|_fltused|_tls_index)$'
NEEDS=$(printf '%s\n' "$ALL" | grep -v '^scaly_' | grep -vE "$CRT")
HAVE=$(printf '%s\n' "$ALL" | grep -v '^scaly_' | grep -E "$CRT")

n() { printf '%s\n' "$1" | grep -c . ; }

echo "win-undef: $(n "$ALL") undefined symbols for $TRIPLE"
echo
echo "  A. ours — brocken 1 (fcontext) + 2 (IOCP):  $(n "$OURS")"
printf '%s\n' "$OURS" | sed 's/^/       /'
echo
echo "  B. POSIX-only, need a Win32 equivalent — brocken 3:  $(n "$NEEDS")"
printf '%s\n' "$NEEDS" | sed 's/^/       /'
echo
echo "  C. in the MSVC CRT already, nothing owed:  $(n "$HAVE")"
printf '%s\n' "$HAVE" | tr '\n' ' ' | fold -s -w 68 | sed 's/^/       /'
echo
echo "  NOTE aligned_alloc is class B and carries a trap: MSVC has no"
echo "       aligned_alloc, only _aligned_malloc, and its memory must be"
echo "       released with _aligned_free — free() on it is undefined."
