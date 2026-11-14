#!/bin/bash
# tests/card/run.sh [compiler] -- CardVectors on a page's graphics card
# (ROADMAP-card.md, scaly/card.scaly, scaly/card/card_wasm.c,
# web/card-host.mjs, web/card-worker.mjs).
#
# Each program here is built twice: natively, where its CardVectors live in
# memory and its loops run on the cores, and for wasm32-wasip1 with --gpu,
# run in a page in headless Chrome whose card is their home -- every loop
# over them one run handed to the card, every read one way there and back.
# The page must print what the native program prints and end as it ends,
# and the card must have run the loops (the host counts runs and flushes).
#
# NOT in the bar: it needs a Chrome with WebGPU (CHROME; on Linux a Chromium
# such as Brave, whose WebGPU wants --enable-unsafe-webgpu and, for the real
# card, --enable-features=Vulkan --use-angle=vulkan -- CHROME_FLAGS: without
# them a headless one draws on the processor, SwiftShader, and passes all
# the same), a wasi-sdk (WASI_SDK, else ~/opt/wasi-sdk-*) and node, and is
# SKIPPED by name without one of them. It says which card it ran on.
#
# Usage: tests/card/run.sh [compiler]   (default scalyc/build/scalyc; the tool
#        beside it, tools/scaly-of.sh)
cd "$(dirname "$0")/../.." || exit 1
COMPILER="${1:-scalyc/build/scalyc}"
CHROME="${CHROME:-/Applications/Google Chrome.app/Contents/MacOS/Google Chrome}"
[ -x "$CHROME" ] || { echo "card: SKIP (no Chrome: set CHROME)"; exit 0; }
CHROME_FLAGS="${CHROME_FLAGS:---enable-unsafe-webgpu}"
NODE=$(tools/node.sh) || { echo "card: SKIP (no node that starts)"; exit 0; }
W="${WASI_SDK:-}"
if [ -z "$W" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && W="$cand"; done
fi
[ -n "$W" ] && [ -x "$W/bin/clang" ] || { echo "card: SKIP (no wasi-sdk: set WASI_SDK)"; exit 0; }
TOOL=$(tools/scaly-of.sh "$COMPILER")
WEB=$(ls -d packages/scaly/*/web | tail -1)
PORT=${PORT:-8379}

T=$(mktemp -d "${TMPDIR:-/tmp}/scaly_card.XXXXXX") || exit 1
trap 'rm -rf "${T:?}"' EXIT
mkdir -p "$T/site" "$T/native" "$T/cache"
printf '#!/bin/sh\nexec "%s/bin/clang" --sysroot="%s/share/wasi-sysroot" "$@"\n' "$W" "$W" > "$T/cc"
chmod 755 "$T/cc"
pass=0; fail=0; failures=()
check() { if eval "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$1"); fi; }

names=()
for src in tests/card/*.scaly; do
  name=$(basename "$src" .scaly)
  names+=("$name")
  SCALY_CACHE="$T/cache" "$TOOL" build "$src" -o "$T/native/$name" > "$T/native/$name.build" 2>&1
  check "$name builds natively" '[ -x "$T/native/$name" ]'
  SCALY_CC="$T/cc" SCALY_CACHE="$T/cache" "$TOOL" build --target wasm32-wasip1 --gpu "$src" -o "$T/site/$name.wasm" > "$T/site/$name.build" 2>&1
  check "$name builds for wasm32 with --gpu" '[ -f "$T/site/$name.wasm" ]' || tail -3 "$T/site/$name.build"
done
printf '%s\n' "${names[@]}" | python3 -c 'import json,sys; print(json.dumps([l.strip() for l in sys.stdin if l.strip()]))' > "$T/site/programs.json"
cp tests/card/page.html tests/card/worker.mjs "$WEB/wasi.mjs" "$WEB/card-host.mjs" "$WEB/card-worker.mjs" "$T/site/"

rm -f "$T/result"
"$NODE" tests/gpu/web/serve.mjs "$T/site" "$PORT" "$T/result" 120 &
server=$!
# shellcheck disable=SC2086
"$CHROME" --headless=new --user-data-dir="$T/profile" $CHROME_FLAGS --no-first-run "http://127.0.0.1:$PORT/page.html" >/dev/null 2>&1 &
chrome=$!
wait $server
kill $chrome 2>/dev/null; wait $chrome 2>/dev/null
result=$(cat "$T/result" 2>/dev/null)
if printf '%s\n' "$result" | grep -q '^SKIP'; then
  echo "card: SKIP (the page has no card: $(printf '%s\n' "$result" | grep '^SKIP' | head -1))"
  exit 0
fi

field() {  # field <name> <key>: a field of the page's line for that program
  printf '%s\n' "$result" | grep "^RUN " | sed 's/^RUN //' | python3 -c "
import json, sys
for line in sys.stdin:
    r = json.loads(line)
    if r.get('name') == '$1':
        v = r.get('$2', '')
        print(v if isinstance(v, str) else json.dumps(v), end='')"
}

adapter=$(field card adapter)
echo "card: on $adapter"
if printf '%s' "$adapter" | grep -q '"fallback": true'; then
  echo "card: NOTE -- a fallback adapter: the processor drew for the card (see CHROME_FLAGS)"
fi

for name in "${names[@]}"; do
  native_out=$("$T/native/$name" 2>/dev/null); native_rc=$?
  page_out=$(field "$name" stdout)
  page_status=$(field "$name" status)
  check "$name: the page ends as the native program ($native_rc, page $page_status)" '[ "$page_status" = "$native_rc" ]'
  check "$name: and prints what it prints" '[ "$page_out" = "$native_out" ]'
  if [ "$page_status" != "$native_rc" ] || [ "$page_out" != "$native_out" ]; then
    echo "  $name natively: $native_out" | head -5
    echo "  $name in the page: $page_out $(field "$name" stderr) $(field "$name" failed)" | head -5
  fi
  runs=$(field "$name" runs)
  check "$name: the card ran its loops (runs $runs)" '[ "${runs:-0}" -gt 0 ]'
  if [ "$name" = card_fault ]; then
    check "card_fault: the card names the loop" 'field card_fault stderr | grep -q "card_fault.scaly:.*met an index beyond its vector"'
  fi
done

echo "card: $pass PASS, $fail FAIL"
for f in "${failures[@]}"; do echo "  FAIL: $f"; done
[ $fail -eq 0 ]
