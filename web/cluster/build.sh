#!/bin/bash
# web/cluster/build.sh [out-dir] [scalyc-binary] -- the cluster showcase, a
# render farm: index.html and node.mjs of this directory, the program
# (render.scaly, shown on the page too) built for wasm32-wasip1-threads with
# --gpu as render.wasm, and beside them the standard library's page hosts
# (web/ of its version: the WASI host, the threads', the network's, the
# card's). Into out-dir, default this directory's site/ (not in the
# repository). The tool is the one beside the compiler (tools/scaly-of.sh;
# default scalyc/build/scalyc).
# Needs a wasi-sdk ($WASI_SDK, else ~/opt/wasi-sdk-*); answers 3 without
# one. Then serve it cross-origin isolated -- its nodes share memory with
# their threads:
#
#   web/cluster/build.sh && node web/repl/serve.mjs web/cluster/site 8767
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../.." && pwd)"
OUT="${1:-$HERE/site}"
COMPILER="${2:-$TREE/scalyc/build/scalyc}"
cd "$TREE"
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
if [ -z "$W" ] || [ ! -x "$W/bin/clang" ]; then
  echo "web/cluster/build.sh: no wasi-sdk (set WASI_SDK)" >&2
  exit 3
fi
TOOL=$(tools/scaly-of.sh "$COMPILER")
WEB="packages/scaly/$(tools/version.sh scaly)/web"
mkdir -p "$OUT"
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
printf '#!/bin/sh\nexec "%s/bin/clang" --sysroot="%s/share/wasi-sysroot" "$@"\n' "$W" "$W" > "$WORK/cc"
chmod 755 "$WORK/cc"
SCALY_CC="$WORK/cc" SCALY_CACHE="${SCALY_CACHE:-$WORK/cache}" \
  "$TOOL" build --release --target wasm32-wasip1-threads --gpu "$HERE/render.scaly" -o "$OUT/render.wasm"
cp "$HERE/index.html" "$HERE/node.mjs" "$HERE/render.scaly" "$OUT/"
cp "$WEB/wasi.mjs" "$WEB/threads.mjs" "$WEB/thread.mjs" "$WEB/net.mjs" "$WEB/gpu.mjs" "$WEB/gpu-host.mjs" "$WEB/gpu-worker.mjs" "$OUT/"
echo "web/cluster/build.sh: $OUT ($(du -h "$OUT/render.wasm" | cut -f1) the program)"
