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
    # Whole-program opt, but keep the RBMM runtime + stdlib bodies present AND
    # exported so the in-process ORC JIT (--jit/--run) can resolve them from the
    # host image. The emitter emits runtime/stdlib calls as declare-only (the
    # body is expected from libscaly.a at AOT link time); in JIT there is no
    # link step, so those declarations must resolve to real functions the scalyc
    # binary exports (emit_jit_stubs dlsym's them and only null-stubs the truly
    # absent ones). Two textual IR edits bracket opt to guarantee that:
    #   1. linkonce_odr -> weak_odr BEFORE llvm-link. A linkonce_odr body with
    #      no surviving external reference is DCE'd — first by llvm-link itself
    #      (it discards unreferenced linkonce_odr on merge), then by opt's inliner
    #      + GlobalDCE (StringBuilder's ctor, Page.get/get_capacity, ... get fully
    #      inlined into the compiler and dropped). weak_odr is non-discardable, so
    #      every runtime body is retained out-of-line even after inlining. The
    #      conversion MUST happen on each input BEFORE llvm-link, not on the linked
    #      module after: stdlib functions the COMPILER never calls but a JIT'd
    #      dependency package does (e.g. File.write_from_string — dazzle uses it,
    #      scalyc doesn't) are unreferenced in the linked set and llvm-link drops
    #      them before a post-link sed could protect them, so --jit then can't
    #      resolve them ("failed to materialize"). Converting first keeps the whole
    #      stdlib JIT-visible. Without this the JIT crashes on an uninitialized
    #      StringBuilder (missing ctor body) or a missing dep-only stdlib symbol.
    #   2. strip (local_)unnamed_addr AFTER opt. A linkonce/weak_odr function
    #      carrying unnamed_addr is `weak_def_can_be_hidden`, which macOS ld
    #      collapses to a LOCAL symbol — dlsym and the ORC process generator then
    #      miss it. Removing the attribute emits a plain `.weak_definition`, kept
    #      in the export trie. opt already ran with the attribute present, so no
    #      optimization is lost; only the symbol-table emission changes.
    # AOT/-c is unaffected (real bodies come from libscaly.a). The bootstrap
    # multi-object link keeps these symbols global on its own; this is only the
    # whole-program build's equivalent. ~6% larger binary, emission unchanged.
    for f in main scalyc scaly; do
        sed 's/^define linkonce_odr /define weak_odr /' "$SEED/$f.ll" > "$WORK/${f}_weak.ll"
    done
    "$LLVM_LINK" -S "$WORK/main_weak.ll" "$WORK/scalyc_weak.ll" "$WORK/scaly_weak.ll" -o "$WORK/scalyc_linked.ll"
    "$OPT" -O2 -S "$WORK/scalyc_linked.ll" -o "$WORK/scalyc_opt.ll"
    sed '/^define /s/\(local_\)\?unnamed_addr //g' "$WORK/scalyc_opt.ll" > "$WORK/scalyc_export.ll"
    # -relocation-model=pic: x86-64 Linux links executables as PIE, which rejects
    # llc's default (static) R_X86_64_32 absolute relocations. PIC is the default
    # on Mach-O, so this is a no-op on macOS and harmless on aarch64.
    "$LLC" -relocation-model=pic -filetype=obj "$WORK/scalyc_export.ll" -o "$WORK/scalyc_all.o"
    SCALYC_OBJS=("$WORK/scalyc_all.o")
    echo "build-from-seed: whole-program opt -O2 applied (runtime kept JIT-visible)"
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
    # ELF executables put a global symbol in .dynsym (dlsym-visible) only when
    # explicitly exported. The whole-program build's runtime bodies (weak_odr,
    # see above) live in .symtab but the in-process JIT resolves them via
    # dlsym/ORC, which searches .dynsym — so export them. macOS keeps weak defs
    # in the export trie already (no flag needed there). Harmless on the -O0
    # fallback path too.
    LINKARGS+=("-rdynamic")
fi

# The fiber context-switch primitives (vendored assembly, selected by host
# arch) and the evented-I/O backend shim (kqueue/epoll, selected by cpp) —
# scaly's Fiber/Io procedures reference them, so every link that includes
# the scaly package needs both objects. The civil-time shim rides along: the
# compiler calls none of it, but a dazzle stylesheet run through --jit
# resolves the time primitives out of the compiler process, so the symbols
# must be in the binary (see packages/scaly/0.1.0/scaly/time/ctime.c).
tools/fcontext.sh "$WORK/fcontext.o"
tools/eio.sh "$WORK/eio.o"
tools/ctime.sh "$WORK/ctime.o"

# -lm AFTER the objects: the stdlib's tensor tape kernels call tanhf/expf/sqrtf/
# logf/powf, and on Linux those live in a separate libm (macOS has them in
# libSystem, linked by default). A left-to-right ELF linker only pulls what is
# still undefined, so the library must FOLLOW its references. Harmless on macOS
# (/usr/lib/libm.dylib re-exports libSystem).
mkdir -p "$(dirname "$OUT")"
${CLANG:-clang} "${LINKARGS[@]}" "${SCALYC_OBJS[@]}" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" \
    -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm -o "$OUT"

echo "build-from-seed: OK — $OUT (from seed/, no C++)"

# Build the runtime archive the self-hosted compiler links every program
# against (/tmp/libscaly.a — the path is fixed in cli.scaly). It supplies the
# RBMM runtime; --no-prelude keeps print/println out of it to avoid duplicate
# symbols, and --no-tests drops the test_* orchestrators (which reference
# uninstantiated generics and would otherwise leave undefined symbols in the
# single archive object). Built with the compiler we just produced — C++-free.
# fcontext.o adds the fiber context-switch primitives (vendored assembly,
# packages/scaly/0.1.0/scaly/fiber/) — clang assembles the host's ABI file.
# -O2 (5.3): the archive's opt level is what AOT programs feel for every
# non-generic stdlib body (the tensor tape kernels above all) — the
# program's own -O flag never touches code that lives in libscaly.a.
# The in-process `-c -O2` cannot do this: a library module has no
# External main anchor, every body is linkonce_odr (= discardable), so
# default<O2>'s GlobalDCE deletes the whole module. Emit IR instead,
# promote definitions to weak_odr (kept, same ODR-merge at link), then
# opt -O2 + llc. Falls back to the plain -O0 object when opt is absent.
if [ "${SCALYC_NO_OPT:-0}" != "1" ] && [ -n "$OPT" ]; then
    "$OUT" -S --no-prelude --no-tests -o "$WORK/libscaly.ll" packages/scaly/0.1.0/scaly.scaly
    sed 's/^define linkonce_odr /define weak_odr /' "$WORK/libscaly.ll" > "$WORK/libscaly_weak.ll"
    "$OPT" -O2 "$WORK/libscaly_weak.ll" -o "$WORK/libscaly_opt.bc"
    "$LLC" -relocation-model=pic -O2 -filetype=obj "$WORK/libscaly_opt.bc" -o /tmp/libscaly.o
else
    "$OUT" -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly
fi
cp "$WORK/fcontext.o" /tmp/fcontext.o
cp "$WORK/eio.o" /tmp/eio.o
cp "$WORK/ctime.o" /tmp/ctime.o
rm -f /tmp/libscaly.a; ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o /tmp/ctime.o
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
    ${CLANG:-clang} "${LINKARGS[@]}" "${SCALYLS_OBJS[@]}" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" \
        -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm -o "$LSOUT"
    echo "build-from-seed: OK — $LSOUT (language server)"
fi
