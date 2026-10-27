#!/bin/bash
# web/repl/build.sh [out-dir] -- the REPL's page: index.html, worker.mjs and
# side.mjs of this directory, and beside them in out-dir (default: this
# directory) what is built or taken from the tree --
#   scaly.wasm    the tool built for WebAssembly (tools/wasm/repl.sh build;
#                 SCALY_WASM_LLVM, default ~/scaly-wasm) without its names
#                 and the C library's DWARF: 54 MB become 31 (llvm-strip)
#   wasi.mjs      the page's WASI host, web/wasi.mjs of the standard library
#   stdlib.json   the standard library's files, which the REPL compiles every
#                 entry against: path under /scaly (SCALY_HOME) -> base64
# and both of these compressed beside them (compress.mjs; scaly.wasm.br is
# 6.4 MB), which serve.mjs sends where the browser accepts it. Needs a
# wasi-sdk ($WASI_SDK, else ~/opt/wasi-sdk-*) for its llvm-strip.
# Then serve it cross-origin isolated:
#
#   web/repl/build.sh && node web/repl/serve.mjs
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../.." && pwd)"
OUT="${1:-$HERE}"
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
cd "$TREE"
STDLIB="packages/scaly/$(tools/version.sh scaly)"
if [ ! -f "$L/scaly.wasm" ]; then
  echo "web/repl/build.sh: no $L/scaly.wasm -- build it with tools/wasm/repl.sh build" >&2
  exit 3
fi
mkdir -p "$OUT"
if [ "$OUT" != "$HERE" ]; then
  cp "$HERE/index.html" "$HERE/worker.mjs" "$HERE/side.mjs" "$HERE/serve.mjs" "$HERE/compress.mjs" "$OUT/"
fi
"$W/bin/llvm-strip" -o "$OUT/scaly.wasm" "$L/scaly.wasm"
cp "$STDLIB/web/wasi.mjs" "$OUT/wasi.mjs"
node "$HERE/stdlib.mjs" "$TREE" "$STDLIB" > "$OUT/stdlib.json"
node "$HERE/compress.mjs" "$OUT/scaly.wasm" "$OUT/stdlib.json"
size() { du -sh "$1" | cut -f1; }
echo "web/repl/build.sh: $OUT -- the compiler $(size "$OUT/scaly.wasm") ($(size "$OUT/scaly.wasm.br") brotli), the standard library $(size "$OUT/stdlib.json") ($(size "$OUT/stdlib.json.br"))"
