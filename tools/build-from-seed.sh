#!/bin/bash
# Build scalyc from the committed .ll seed — NO C++ toolchain required.
#
# The seed is the self-hosted Scaly compiler shipped as its own emitted LLVM IR
# (main.ll + scalyc.ll + scaly.ll under seed/). A SINGLE seed serves every 64-bit
# little-endian LP64 target: the IR carries no target triple and bakes layout in
# from fixed LP64 constants, so llc retargets it and the built compiler reads its
# host triple at runtime. This script turns that IR into a working `scalyc` using
# only LLVM 18 and a C compiler for the final link — the C++ stage-0 (build.sh) is
# not involved.
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
SEED="seed"

if [ ! -f "$SEED/scalyc.ll" ]; then
    echo "build-from-seed: FAIL — no committed seed found under seed/."
    exit 1
fi

if [ -f "$SEED/SHA256SUMS" ]; then
    ( cd "$SEED" && shasum -a 256 -c SHA256SUMS >/dev/null ) \
        && echo "seed checksums OK" \
        || { echo "build-from-seed: FAIL — seed checksum mismatch"; exit 1; }
fi

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# Whole-program `opt -O2` between the seed IR and llc (~20% faster compiler).
# Requires llvm-link + opt (both ship with LLVM 18) and the seed's external
# @main (every other function is linkonce_odr — discardable — so the entry
# anchors GlobalDCE; the modules carry a `target datalayout` line so opt's
# constant folding agrees with llc). Set SCALYC_NO_OPT=1 to skip, and the
# per-module llc path is the automatic fallback when opt/llvm-link are absent.
use_opt=0
if [ "${SCALYC_NO_OPT:-0}" != "1" ] && [ -n "$OPT" ] && [ -n "$LLVM_LINK" ]; then
    if grep -q '^define i64 @main(' "$SEED/main.ll"; then
        use_opt=1
    else
        echo "build-from-seed: seed main is not external — skipping opt"
    fi
fi

if [ "$use_opt" = "1" ]; then
    "$LLVM_LINK" "$SEED/main.ll" "$SEED/scalyc.ll" "$SEED/scaly.ll" -o "$WORK/scalyc_linked.bc"
    "$OPT" -O2 "$WORK/scalyc_linked.bc" -o "$WORK/scalyc_opt.bc"
    # -relocation-model=pic: x86-64 Linux links executables as PIE, which rejects
    # llc's default (static) R_X86_64_32 absolute relocations. PIC is the default
    # on Mach-O, so this is a no-op on macOS and harmless on aarch64.
    "$LLC" -relocation-model=pic -filetype=obj "$WORK/scalyc_opt.bc" -o "$WORK/scalyc_all.o"
    SCALYC_OBJS=("$WORK/scalyc_all.o")
    echo "build-from-seed: whole-program opt -O2 applied"
else
    for f in main scalyc scaly; do
        "$LLC" -relocation-model=pic -filetype=obj "$SEED/$f.ll" -o "$WORK/$f.o"
    done
    SCALYC_OBJS=("$WORK/main.o" "$WORK/scalyc.o" "$WORK/scaly.o")
fi

# On Linux, stock GNU ld (BFD) fails to link libLLVM-18 ("failed to set dynamic
# section sizes: bad value"); lld handles it. Use lld when present. macOS ld64
# links fine, so leave it alone there.
LINKARGS=()
if [ "$(uname -s)" = "Linux" ]; then
    for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld ld.lld-18; do
        p=$(command -v "$c" 2>/dev/null || true)
        [ -n "$p" ] && { LINKARGS+=("-fuse-ld=$p"); break; }
    done
fi

mkdir -p "$(dirname "$OUT")"
${CLANG:-clang} "${LINKARGS[@]}" "${SCALYC_OBJS[@]}" \
    -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT"

echo "build-from-seed: OK — $OUT (from seed/, no C++)"

# Build the runtime archive the self-hosted compiler links every program
# against (/tmp/libscaly.a — the path is fixed in cli.scaly). It supplies the
# RBMM runtime; --no-prelude keeps print/println out of it to avoid duplicate
# symbols, and --no-tests drops the test_* orchestrators (which reference
# uninstantiated generics and would otherwise leave undefined symbols in the
# single archive object). Built with the compiler we just produced — C++-free.
"$OUT" -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly
ar rcs /tmp/libscaly.a /tmp/libscaly.o
echo "build-from-seed: runtime archive /tmp/libscaly.a ready"

# Build the scalyls language server from its seed, when committed. scalyls is
# a separate program with its own two roots (scalyls_main.ll + scalyls.ll);
# it depends on the scalyc + scaly packages, whose bodies come from the
# compiler seed objects already built above — same recipe as tools/seed.sh.
# Lands beside the compiler so tools/install.sh and the VS Code extension can
# find it.
# SCALYC_SEED_NO_SCALYLS=1 skips the language server — bootstrap/cycle roots
# only need the compiler, and the scalyls whole-program opt pass re-optimizes
# the entire scalyc+scaly superset module (~1min it would spend for nothing).
if [ "${SCALYC_SEED_NO_SCALYLS:-0}" != "1" ] && [ -f "$SEED/scalyls.ll" ] && [ -f "$SEED/scalyls_main.ll" ]; then
    LSOUT="$(dirname "$OUT")/scalyls"
    if [ "$use_opt" = "1" ] && grep -q '^define i64 @main(' "$SEED/scalyls_main.ll"; then
        # Same whole-program shape as the compiler: scalyls' own two roots
        # plus the scalyc + scaly package bodies (the compiler's main.ll is
        # excluded — scalyls_main.ll provides this program's @main anchor).
        "$LLVM_LINK" "$SEED/scalyls_main.ll" "$SEED/scalyls.ll" \
            "$SEED/scalyc.ll" "$SEED/scaly.ll" -o "$WORK/scalyls_linked.bc"
        "$OPT" -O2 "$WORK/scalyls_linked.bc" -o "$WORK/scalyls_opt.bc"
        "$LLC" -relocation-model=pic -filetype=obj "$WORK/scalyls_opt.bc" -o "$WORK/scalyls_all.o"
        SCALYLS_OBJS=("$WORK/scalyls_all.o")
    else
        for f in scalyls_main scalyls; do
            "$LLC" -relocation-model=pic -filetype=obj "$SEED/$f.ll" -o "$WORK/$f.o"
        done
        if [ ! -f "$WORK/scalyc.o" ]; then
            for f in scalyc scaly; do
                "$LLC" -relocation-model=pic -filetype=obj "$SEED/$f.ll" -o "$WORK/$f.o"
            done
        fi
        SCALYLS_OBJS=("$WORK/scalyls_main.o" "$WORK/scalyls.o" "$WORK/scalyc.o" "$WORK/scaly.o")
    fi
    ${CLANG:-clang} "${LINKARGS[@]}" "${SCALYLS_OBJS[@]}" \
        -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$LSOUT"
    echo "build-from-seed: OK — $LSOUT (language server)"
fi
