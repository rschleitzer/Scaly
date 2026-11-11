#!/bin/bash
# tests/gpu/web/run.sh [compiler] — the kernels in a browser.
#
# A kernel a card under WebGPU may run is printed in WGSL (GpuMsl), and the
# native host runs it through wgpu-native. A web page has another WebGPU: the
# browser's, with its own reading of the language. This gate takes every run
# the programs of tests/gpu make through wgpu-native -- written down by the
# host, SCALY_GPU_DUMP: kernel, words, inputs, outputs before and after --
# and runs it again in a page (replay.html, over the page-side host
# packages/scaly/<version>/web/gpu.mjs) in headless Chrome. Whole numbers
# must come out bit for bit as natively, floating point of a `relaxed` loop
# within a tolerance, and a run a round stopped must stop here too.
#
# NOT in the bar: it needs a Chrome with WebGPU, wgpu-native and node, and is
# SKIPPED by name without one of them. Run it after a change to the WGSL the
# compiler prints or to gpu.mjs.
#
# Usage: tests/gpu/web/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
CHROME="${CHROME:-/Applications/Google Chrome.app/Contents/MacOS/Google Chrome}"
[ -x "$CHROME" ] || { echo "gpu-web: SKIP (no Chrome: set CHROME)"; exit 0; }
NODE=$(tools/node.sh) || { echo "gpu-web: SKIP (no node that starts)"; exit 0; }

T=$(mktemp -d "${TMPDIR:-/tmp}/scaly_gpu_web.XXXXXX") || exit 1
trap 'rm -rf "${T:?}"' EXIT
mkdir -p "$T/site" "$T/bin"
built=0
for src in tests/gpu/*.scaly tests/gpu/webgpu/*.scaly; do
  name=$(basename "$src" .scaly)
  "$STAGE" --gpu -o "$T/bin/$name" "$src" >"$T/bin/$name.build" 2>&1 || continue
  mkdir -p "$T/site/$name"
  SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_DUMP="$T/site/$name" "$T/bin/$name" >/dev/null 2>&1
  built=$((built+1))
done
runs=$(cd "$T/site" && ls */*.json 2>/dev/null)
if [ -z "$runs" ]; then
  echo "gpu-web: SKIP (no run through wgpu-native written down: brew install wgpu-native)"; exit 0
fi
count=$(printf '%s\n' "$runs" | wc -l | tr -d ' ')
printf '%s\n' "$runs" | python3 -c 'import json,sys; print(json.dumps([l.strip() for l in sys.stdin if l.strip()]))' > "$T/site/runs.json"
cp tests/gpu/web/replay.html "$T/site/"
cp "$(ls packages/scaly/*/web/gpu.mjs | tail -1)" "$T/site/"

PORT=${PORT:-8377}
"$NODE" tests/gpu/web/serve.mjs "$T/site" "$PORT" "$T/result" 120 &
server=$!
"$CHROME" --headless=new --user-data-dir="$T/profile" --enable-unsafe-webgpu --no-first-run "http://127.0.0.1:$PORT/replay.html" >/dev/null 2>&1 &
chrome=$!
wait $server
kill $chrome 2>/dev/null; wait $chrome 2>/dev/null

result=$(cat "$T/result" 2>/dev/null)
printf '%s\n' "$result" | grep -v '^replay: ' | head -20
summary=$(printf '%s\n' "$result" | grep '^replay: ')
if printf '%s\n' "$result" | grep -q '^SKIP'; then
  echo "gpu-web: SKIP (the browser has no WebGPU)"; exit 0
fi
if [ -n "$summary" ] && printf '%s\n' "$summary" | grep -q "^replay: $count runs as the native card ran them, 0 not"; then
  echo "gpu-web: PASS — $count runs of $built programs: ${summary#replay: }"
  exit 0
fi
echo "gpu-web: FAIL — ${summary:-no answer from the page ($result)}"
exit 1
