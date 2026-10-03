#!/bin/bash
# The READY-MADE objects of the packages' native files, for what we hand out
# (ROADMAP-public.md, "Windows: Rust's way"): `scaly build` links them as they
# are and asks no C compiler (tool.scaly, native_objects).
#
#   tools/native-objects.sh <compiler> <home>
#
# <compiler> is a scalyc with its scaly beside it (tools/scaly-of.sh); <home>
# is the tree that will be handed out -- it holds `packages/` -- and gets
# `packages/<p>/<v>/_native/<os>-<arch>/<file>.o` for every native file a
# program of the standard packages links on THIS system and architecture.
# NEVER the source tree: an object there would be linked instead of a changed
# source, without a word (the script refuses a <home> that is a git checkout).
#
# How: one `scaly build` into a fresh cache with the C compiler this machine
# has; the cache names each native object `<p>-<v>-<key>-<file>.o`, and the
# part after the key is the name the tool looks for. So which files a target
# takes is decided in ONE place, the tool (select_native).
# Only packages without a foreign C library: an object compiled against a
# library's headers belongs to the machine that has the library.
set -eu
cd "$(dirname "$0")/.."
. tools/win-env.sh || exit 1
COMPILER=${1:?usage: tools/native-objects.sh <compiler> <home>}
DEST=${2:?usage: tools/native-objects.sh <compiler> <home>}
[ -d "$DEST/packages" ] || { echo "native-objects: no $DEST/packages" >&2; exit 1; }
[ -e "$DEST/.git" ] && { echo "native-objects: $DEST is a checkout; the objects belong in what is handed out" >&2; exit 1; }
SCALY=$(tools/scaly-of.sh "$COMPILER")

case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) os=windows; machine=${SCALY_WIN_TRIPLE%%-*} ;;
  Darwin) os=darwin; machine=$(uname -m) ;;
  *) os=linux; machine=$(uname -m) ;;
esac
case "$machine" in
  aarch64|arm64) arch=arm64 ;;
  x86_64|amd64) arch=x86_64 ;;
  *) echo "native-objects: unknown architecture $machine" >&2; exit 1 ;;
esac

W=$(mktemp -d)
trap 'rm -rf "$W"' EXIT
printf 'print("ready")\n' > "$W/probe.scaly"
SCALY_HOME="$DEST" SCALY_CACHE="$W/cache" "$SCALY" build "$W/probe.scaly" -o "$W/probe${SCALY_EXE:-}" > "$W/build.log" 2>&1 \
  || { echo "native-objects: the probe program did not build:" >&2; tail -5 "$W/build.log" >&2; exit 1; }

n=0
for o in "$W"/cache/*-*.o; do
  name=$(basename "$o")
  # <package>-<version>-<16 hex>-<file>.o; the package's own object has no <file>
  rest=$(echo "$name" | sed -n 's/^\([a-z0-9_]*\)-\([0-9.]*\)-[0-9a-f]\{16\}-\(.*\)$/\1 \2 \3/p')
  [ -n "$rest" ] || continue
  set -- $rest
  dir="$DEST/packages/$1/$2/_native/$os-$arch"
  mkdir -p "$dir"
  cp "$o" "$dir/$3"
  n=$((n+1))
done
[ "$n" -gt 0 ] || { echo "native-objects: the cache holds no native object" >&2; exit 1; }
echo "native-objects: $n objects for $os-$arch under $DEST/packages"
