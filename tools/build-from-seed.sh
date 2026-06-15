#!/bin/bash
# Build scalyc from the committed .ll seed — NO C++ toolchain required.
#
# The seed is the self-hosted Scaly compiler shipped as its own emitted LLVM IR
# (main.ll + scalyc.ll + scaly.ll, one set per target triple under seed/<triple>/).
# This script turns that IR back into a working `scalyc` using only LLVM 18 and a
# C compiler for the final link — the C++ stage-0 (build.sh) is not involved.
#
# Usage: tools/build-from-seed.sh [output-binary]
#   output-binary   default scalyc/build/scalyc (the path the rest of the repo
#                   and the tutorial expect)
#
# Requirements:
#   - llc from LLVM 18 (it accepts the seed's mul/ptrtoint-GEP constexprs that
#     some system clangs reject). Override detection with LLVM18=/path.
#   - any clang/cc for the final object link.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "build-from-seed: FAIL — LLVM 18 not found"; exit 1; }

OUT=${1:-scalyc/build/scalyc}
TRIPLE=$(${CLANG:-clang} -dumpmachine | sed 's/[0-9.]*$//')
SEED="seed/$TRIPLE"

if [ ! -f "$SEED/scalyc.ll" ]; then
    echo "build-from-seed: FAIL — no committed seed for this host triple ($TRIPLE)."
    echo "Available seeds:"; ls seed/ 2>/dev/null | sed 's/^/  /'
    echo "Mint one for this platform with tools/seed.sh (needs the C++ stage-0 once)."
    exit 1
fi

if [ -f "$SEED/SHA256SUMS" ]; then
    ( cd "$SEED" && shasum -a 256 -c SHA256SUMS >/dev/null ) \
        && echo "seed checksums OK ($TRIPLE)" \
        || { echo "build-from-seed: FAIL — seed checksum mismatch"; exit 1; }
fi

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
for f in main scalyc scaly; do
    "$LLC" -filetype=obj "$SEED/$f.ll" -o "$WORK/$f.o"
done

mkdir -p "$(dirname "$OUT")"
${CLANG:-clang} "$WORK/main.o" "$WORK/scalyc.o" "$WORK/scaly.o" \
    -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT"

echo "build-from-seed: OK — $OUT (from seed/$TRIPLE, no C++)"

# Build the runtime archive the self-hosted compiler links every program
# against (/tmp/libscaly.a — the path is fixed in cli.scaly). It supplies the
# RBMM runtime; --no-prelude keeps print/println out of it to avoid duplicate
# symbols. Built with the compiler we just produced, so this stays C++-free.
"$OUT" -c --no-prelude -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly
ar rcs /tmp/libscaly.a /tmp/libscaly.o
echo "build-from-seed: runtime archive /tmp/libscaly.a ready"
