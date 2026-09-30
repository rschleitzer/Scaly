#!/usr/bin/env bash
# tools/link-lto.sh — whole-program link of a Scaly program from emitted IR.
#
#   tools/link-lto.sh <out-binary> <ir-file> [<ir-file> ...]
#
# Merges the program IR, its package IRs and the runtime IR into one module,
# runs `opt -O2` across the whole thing and links the single object. This is
# the same pipeline tools/build-from-seed.sh uses for the compiler, with two
# differences that only make sense for a leaf program:
#
#   * linkonce_odr bodies are NOT promoted to weak_odr first. build-from-seed
#     promotes them so the in-process ORC JIT can still resolve every stdlib
#     symbol out of the compiler image; a program has no JIT to serve, so
#     letting llvm-link and GlobalDCE drop what nothing references is what
#     halves the binary.
#   * definitions become `hidden` after the merge. weak/linkonce symbols are
#     interposable, so on Mach-O even binary-internal calls to them route
#     through the stub table — measured 2026-08-01 at 17.6 % of all samples in
#     the dazzle engine, because the hot RBMM prologue/epilogue calls go that
#     way. Hidden lets the linker bind them directly (see PERFORMANCE.md).
#
# Measured on the dazzle CLI: −19 % CPU on the DocBook stylesheets, −13 % on a
# 13.7 MB parse, binary 5.17 MB → 2.57 MB, output byte-identical.
#
# Exit 3 = llvm-link or opt unavailable; the caller falls back to its
# per-archive path (this is not a failure — LLVM's opt is optional in
# tools/llvm-env.sh).

set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"

if [ "$#" -lt 2 ]; then
  echo "usage: tools/link-lto.sh <out-binary> <ir-file> [<ir-file> ...]" >&2
  exit 2
fi
OUT="$1"; shift

# shellcheck disable=SC1091
source "$ROOT/tools/llvm-env.sh" >/dev/null 2>&1
[ -n "${LLVM_LINK:-}" ] && [ -n "${OPT:-}" ] && [ -n "${LLC:-}" ] || exit 3

WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

"$LLVM_LINK" -S "$@" -o "$WORK/whole.ll" || { echo "link-lto: FAIL (llvm-link)" >&2; exit 1; }
sed 's/^define linkonce_odr /define linkonce_odr hidden /' "$WORK/whole.ll" > "$WORK/whole_hidden.ll"
# LTO_OPT_FLAGS: extra opt flags, e.g. profile-guided optimization:
#   --pgo-kind=pgo-instr-gen-pipeline   (and LINK_EXTRA=<llvm>/lib/clang/20/lib/darwin/libclang_rt.profile_osx.a)
#   --pgo-kind=pgo-instr-use-pipeline --profile-file=<merged .profdata>
"$OPT" -O2 ${LTO_OPT_FLAGS:-} "$WORK/whole_hidden.ll" -o "$WORK/whole.bc" || { echo "link-lto: FAIL (opt)" >&2; exit 1; }
# -relocation-model=pic: x86-64 Linux links executables as PIE, which rejects
# llc's default (static) absolute relocations. No-op on Mach-O.
# LTO_LLC_FLAGS: extra llc flags for a profiling build (e.g. -frame-pointer=all,
# so that a stack-logging allocator can walk the stacks); empty by default
# shellcheck disable=SC2086
# LTO_SPLIT=<n|auto>: codegen in n parallel parts (tools/llc-split.sh). OFF by
# default: the parts cost 1-2 % run time (tscaly bench), and the binaries this
# script builds are measured -- the tsgo comparison above all.
"$ROOT/tools/llc-split.sh" "${LTO_SPLIT:-1}" "$WORK/whole.bc" "$WORK/whole" \
  ${LTO_LLC_FLAGS:-} -relocation-model=pic -O2 -filetype=obj > "$WORK/objs.txt" \
  || { echo "link-lto: FAIL (llc)" >&2; exit 1; }
OBJS=()
while IFS= read -r o; do OBJS+=("$o"); done < "$WORK/objs.txt"

# The fiber context-switch primitives, the evented-I/O shim and the civil-time
# shim — built here rather than reused from /tmp so the binary never depends on
# whatever a previous build left lying around.
"$ROOT/tools/fcontext.sh" "$WORK/fcontext.o" >/dev/null 2>&1
"$ROOT/tools/eio.sh"      "$WORK/eio.o"      >/dev/null 2>&1
"$ROOT/tools/ctime.sh"    "$WORK/ctime.o"    >/dev/null 2>&1
"$ROOT/tools/panic.sh"    "$WORK/panic.o"    >/dev/null 2>&1

# Empty-array expansion under `set -u` needs the +-guard below (bash 3.2 on
# macOS treats "${arr[@]}" of an empty array as unbound).
LINKARGS=()
if [ "$(uname -s)" = "Linux" ]; then
  for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld "ld.lld-$LLVM_MAJOR"; do
    p=$(command -v "$c" 2>/dev/null || true)
    [ -n "$p" ] && { LINKARGS+=("-fuse-ld=$p"); break; }
  done
fi

mkdir -p "$(dirname "$OUT")"
# -lm last: the stdlib's tensor tape kernels call tanhf/expf/sqrtf/logf/powf,
# a separate libm on Linux (libSystem on macOS), and it must follow the objects
# that reference it.
# LINK_EXTRA: additional libraries the program needs, appended before -lm.
# dazzle's Stage-6b JIT calls LLVM-C/ORC, so its build passes
# -L<llvm-libdir> -lLLVM-$LLVM_MAJOR here; nothing else uses it.
EXTRA=()
if [ -n "${LINK_EXTRA:-}" ]; then
  # shellcheck disable=SC2206
  EXTRA=(${LINK_EXTRA})
fi
${CLANG:-clang} ${LINKARGS[@]+"${LINKARGS[@]}"} "${OBJS[@]}" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" "$WORK/panic.o" ${EXTRA[@]+"${EXTRA[@]}"} -lm -o "$OUT" \
  || { echo "link-lto: FAIL (link)" >&2; exit 1; }
