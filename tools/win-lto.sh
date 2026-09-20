#!/bin/bash
# Build an executable from emitted LLVM IR on the Windows box — rung 12 of
# CI's `windows` job (.github/workflows/seed.yml), line for line, so that the
# committed seed and a freshly emitted one build here without llc, opt or
# llvm-link. `clang -flto=full` writes bitcode objects and lld-link does the
# merge, the optimisation and the codegen: that IS llvm-link + opt + llc under
# the two names the LLVM installer ships (CLAUDE-tooling.md).
#
#   tools/win-lto.sh [--llvm] <out.exe> <root.ll> [<root.ll> ...]
#
# ★LTO is not a shortcut here, it is the only route for seed-shaped IR: the
# committed seed carries no COMDATs and cannot (one text serves every target,
# Mach-O refuses them), so on COFF every `linkonce_odr` lowers to a bare
# `.weak` plus a default symbol named after its object's anchor, and two
# objects then define 257 symbols twice (LNK1227). One module at link time
# and the question never arises.
# ★NO -O2 on the per-module compile: a DCE that runs per MODULE cannot know
# about a reference from another module — `cli::main` is linkonce_odr in
# scalyc.ll and reachable only from main.ll, and per-module -O2 deleted it.
# The -O2 sits on the link (tools/win-link.sh --lto), where the whole program
# is visible.
# ★A `declare` nothing calls is nothing in an object file and an UNDEFINED
# SYMBOL in a bitcode module's symbol table; `opt -O2` drops them on POSIX
# before llc ever sees them, so the seed carried 58 and nobody knew. Dropping
# a declaration whose name occurs exactly once in its own module is safe by
# construction: nothing in that module can refer to it.
# ★No weak_odr promotion: it exists to keep stdlib bodies visible to the
# in-process ORC JIT, which does not work on Windows (an .exe exports nothing
# for GetProcAddress). Discarding what an executable cannot reach is correct.
# ★The C shims go in as OBJECTS, never through libscaly.lib: that archive
# holds a Scaly runtime too, and pulling it in would resolve the runtime twice
# and measure nothing.
set -eu
cd "$(dirname "$0")/.."
. tools/win-env.sh || exit 1
T=x86_64-pc-windows-msvc
LLVM=()
while [ $# -gt 0 ]; do
  case "$1" in
    --llvm) LLVM=(--llvm) ;;
    -*) echo "win-lto: unknown flag $1" >&2; exit 2 ;;
    *) break ;;
  esac
  shift
done
OUT=${1:?usage: tools/win-lto.sh [--llvm] <out.exe> <root.ll>...}
shift
[ $# -gt 0 ] || { echo "win-lto: no IR given" >&2; exit 2; }

W=$(mktemp -d)
trap 'rm -rf "$W"' EXIT

objs=()
i=0
for ll in "$@"; do
  [ -f "$ll" ] || { echo "win-lto: no such file: $ll" >&2; exit 1; }
  n=$(basename "$ll" .ll)
  grep -o '@[A-Za-z0-9_.$]*' "$ll" | sort | uniq -c | awk '$1==1{print $2}' > "$W/dead_$i.txt"
  awk 'NR==FNR{dead[$0]=1;next}
       /^declare/ { if (match($0,/@[A-Za-z0-9_.$]+/) \
                    && (substr($0,RSTART,RLENGTH) in dead)) next }
       {print}' "$W/dead_$i.txt" "$ll" > "$W/live_$i.ll"
  echo "win-lto: $n.ll: $(grep -c '^declare' "$ll") declares -> $(grep -c '^declare' "$W/live_$i.ll") live"
  clang --target=$T -flto=full -c -Wno-override-module "$W/live_$i.ll" -o "$W/$n.obj"
  objs+=("$W/$n.obj")
  i=$((i+1))
done

tools/fcontext.sh    "$W/fcontext.o"
tools/eio.sh         "$W/eio.o"
tools/ctime.sh       "$W/ctime.o"
tools/panic.sh       "$W/panic.o"
tools/posixcompat.sh "$W/posixcompat.o"

tools/win-link.sh --lto ${LLVM[@]+"${LLVM[@]}"} "$OUT" "${objs[@]}" \
  "$W/fcontext.o" "$W/eio.o" "$W/ctime.o" "$W/panic.o" "$W/posixcompat.o"
