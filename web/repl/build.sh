#!/bin/bash
# web/repl/build.sh [out-dir] -- the REPL's page: index.html, worker.mjs and
# side.mjs of this directory, and beside them in out-dir (default: this
# directory) what is built or taken from the tree --
#   scaly.wasm    the tool built for WebAssembly (tools/wasm/repl.sh build;
#                 SCALY_WASM_LLVM, default ~/scaly-wasm)
#   wasi.mjs      the page's WASI host, web/wasi.mjs of the standard library
#   stdlib.json   the standard library's files, which the REPL compiles every
#                 entry against: path under /scaly (SCALY_HOME) -> base64
# Then serve it cross-origin isolated:
#
#   web/repl/build.sh && node web/repl/serve.mjs
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../.." && pwd)"
OUT="${1:-$HERE}"
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
cd "$TREE"
STDLIB="packages/scaly/$(tools/version.sh scaly)"
if [ ! -f "$L/scaly.wasm" ]; then
  echo "web/repl/build.sh: no $L/scaly.wasm -- build it with tools/wasm/repl.sh build" >&2
  exit 3
fi
mkdir -p "$OUT"
if [ "$OUT" != "$HERE" ]; then
  cp "$HERE/index.html" "$HERE/worker.mjs" "$HERE/side.mjs" "$HERE/serve.mjs" "$OUT/"
fi
cp "$L/scaly.wasm" "$OUT/scaly.wasm"
cp "$STDLIB/web/wasi.mjs" "$OUT/wasi.mjs"
node "$HERE/stdlib.mjs" "$TREE" "$STDLIB" > "$OUT/stdlib.json"
echo "web/repl/build.sh: $OUT ($(du -sh "$OUT/scaly.wasm" | cut -f1) compiler, $(du -sh "$OUT/stdlib.json" | cut -f1) standard library)"
