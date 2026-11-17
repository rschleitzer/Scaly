#!/bin/bash
# web/model/build.sh [out-dir] [scalyc-binary] -- the model: index.html and
# runner.mjs of this directory, the program (model.scaly) built for
# wasm32-wasip1-threads as model.wasm, and beside them the standard
# library's page hosts (web/ of its version: the WASI host and the
# threads'). Into out-dir, default this directory's site/ (not in the
# repository). The tool is the one beside the compiler (tools/scaly-of.sh;
# default scalyc/build/scalyc). Needs a wasi-sdk ($WASI_SDK, else
# ~/opt/wasi-sdk-*); answers 3 without one. Then serve it cross-origin
# isolated -- its threads share memory:
#
#   web/model/build.sh && node web/repl/serve.mjs "$PWD/web/model/site" 8769
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
  echo "web/model/build.sh: no wasi-sdk (set WASI_SDK)" >&2
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
  "$TOOL" build --release --target wasm32-wasip1-threads "$HERE/model.scaly" -o "$OUT/model.wasm"
cp "$HERE/index.html" "$HERE/runner.mjs" "$HERE/model.scaly" "$OUT/"
cp "$WEB/wasi.mjs" "$WEB/threads.mjs" "$WEB/thread.mjs" "$OUT/"
echo "web/model/build.sh: $OUT ($(du -h "$OUT/model.wasm" | cut -f1) the program)"
