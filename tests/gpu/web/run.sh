#!/bin/bash
# tests/gpu/web/run.sh [compiler] — the kernels in a browser.
#
# 1. THE KERNELS. A kernel a card under WebGPU may run is printed in WGSL
#    (GpuMsl), and the native host runs it through wgpu-native. A web page has
#    another WebGPU: the browser's, with its own reading of the language. This
#    part takes every run the programs of tests/gpu make through wgpu-native
#    -- written down by the host, SCALY_GPU_DUMP: kernel, words, inputs,
#    outputs before and after -- and runs it again in a page (replay.html,
#    over the page-side host packages/scaly/<version>/web/gpu.mjs) in headless
#    Chrome. Whole numbers must come out bit for bit as natively, floating
#    point of a `relaxed` loop within a tolerance, and a run a round stopped
#    must stop here too.
#
# 2. THE PROGRAMS. The same programs, and gate.scaly here, built for
#    wasm32-wasip1 with --gpu and run in a page (program.html): the program
#    in a worker, the card in a second one, the call between them
#    synchronous (web/gpu-host.mjs, web/gpu-worker.mjs). Each runs three
#    times: with the card off, tried at every loop's head, and left to the
#    gate. Without a card a program must write what it writes natively
#    (but for the ones named in UNLIKE below); with
#    one, what it writes without -- and the card must have run a loop where
#    it is forced on the programs named in ON below, and at least twice by
#    the gate's own decision in gate.scaly. Before the page, each module runs
#    once on the command line under a host without a card.
#
# 3. THE COMPILER IN THE PAGE. The compiler built for WebAssembly
#    (tools/wasm/repl.sh build, web/repl/build.sh) compiles three of the
#    programs IN the page (compiler.html), as the tour runs a step --
#    `scaly run --gpu main.scaly`, the program a side module of the compiler
#    -- and each must write what it writes natively, with the card and
#    without, the card having run its loops where it is on.
#
# NOT in the bar: it needs a Chrome with WebGPU, wgpu-native and node (part 2
# a wasi-sdk too, part 3 libLLVM built for wasm, tools/wasm/build-llvm.sh),
# and is SKIPPED by name without one of them. Run it after a change to the
# WGSL the compiler prints, to the page's hosts or to the wasm side of the
# runtime's host (scaly/gpu/gpu_wasm.c).
#
# Usage: tests/gpu/web/run.sh [stage-binary]   (default /tmp/scalyc_stage2; the
#        tool beside it, tools/scaly-of.sh)
cd "$(dirname "$0")/../../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
CHROME="${CHROME:-/Applications/Google Chrome.app/Contents/MacOS/Google Chrome}"
[ -x "$CHROME" ] || { echo "gpu-web: SKIP (no Chrome: set CHROME)"; exit 0; }
NODE=$(tools/node.sh) || { echo "gpu-web: SKIP (no node that starts)"; exit 0; }

# the programs whose loops the card must run where it is forced
ON="nested reductions shapes integers32 relaxed gate"
# the programs that do not end under wasm as they end natively: fault_soft
# divides by zero, which has no result -- this machine's processor answers
# 0, wasm stops the program. With and without the card it must end alike.
UNLIKE="fault_soft"

T=$(mktemp -d "${TMPDIR:-/tmp}/scaly_gpu_web.XXXXXX") || exit 1
trap 'rm -rf "${T:?}"' EXIT
mkdir -p "$T/site" "$T/bin" "$T/page"
WEB=$(ls -d packages/scaly/*/web | tail -1)
PORT=${PORT:-8377}

# One page in headless Chrome over a directory; what it posts is in $T/result.
page() {  # page <directory> <file> <seconds>
  rm -f "$T/result"
  "$NODE" tests/gpu/web/serve.mjs "$1" "$PORT" "$T/result" "$3" &
  local server=$!
  "$CHROME" --headless=new --user-data-dir="$T/profile" --enable-unsafe-webgpu --no-first-run "http://127.0.0.1:$PORT/$2" >/dev/null 2>&1 &
  local chrome=$!
  wait $server
  kill $chrome 2>/dev/null; wait $chrome 2>/dev/null
}

# --- 1. the kernels -----------------------------------------------------
built=0; names=()
for src in tests/gpu/*.scaly tests/gpu/webgpu/*.scaly tests/gpu/web/gate.scaly; do
  name=$(basename "$src" .scaly)
  "$STAGE" --gpu -o "$T/bin/$name" "$src" >"$T/bin/$name.build" 2>&1 || continue
  mkdir -p "$T/site/$name"
  SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_DUMP="$T/site/$name" "$T/bin/$name" >/dev/null 2>&1
  built=$((built+1)); names+=("$name:$src")
done
runs=$(cd "$T/site" && ls */*.json 2>/dev/null)
if [ -z "$runs" ]; then
  echo "gpu-web: SKIP (no run through wgpu-native written down: brew install wgpu-native)"; exit 0
fi
count=$(printf '%s\n' "$runs" | wc -l | tr -d ' ')
printf '%s\n' "$runs" | python3 -c 'import json,sys; print(json.dumps([l.strip() for l in sys.stdin if l.strip()]))' > "$T/site/runs.json"
cp tests/gpu/web/replay.html "$WEB/gpu.mjs" "$T/site/"
page "$T/site" replay.html 120

result=$(cat "$T/result" 2>/dev/null)
printf '%s\n' "$result" | grep -v '^replay: ' | head -20
summary=$(printf '%s\n' "$result" | grep '^replay: ')
if printf '%s\n' "$result" | grep -q '^SKIP'; then
  echo "gpu-web: SKIP (the browser has no WebGPU)"; exit 0
fi
if [ -z "$summary" ] || ! printf '%s\n' "$summary" | grep -q "^replay: $count runs as the native card ran them, 0 not"; then
  echo "gpu-web: FAIL — ${summary:-no answer from the page ($result)}"
  exit 1
fi
echo "gpu-web: kernels PASS — $count runs of $built programs: ${summary#replay: }"

# --- 2. the programs ----------------------------------------------------
wasi="${WASI_SDK:-}"
if [ -z "$wasi" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && wasi="$cand"; done
fi
if [ -z "$wasi" ] || [ ! -x "$wasi/bin/clang" ]; then
  echo "gpu-web: programs SKIP (no wasi-sdk: set WASI_SDK)"; exit 0
fi
TOOL=$(tools/scaly-of.sh "$STAGE")
[ -x "$TOOL" ] || { echo "gpu-web: FAIL — no tool beside $STAGE ($TOOL)"; exit 1; }
printf '#!/bin/sh\nexec "%s/bin/clang" --sysroot="%s/share/wasi-sysroot" "$@"\n' "$wasi" "$wasi" > "$T/cc"
chmod 755 "$T/cc"

failed=0; programs=()
for entry in "${names[@]}"; do
  name=${entry%%:*}; src=${entry#*:}
  SCALY_GPU=off "$T/bin/$name" > "$T/page/$name.native" 2>/dev/null; native_rc=$?
  if ! SCALY_CC="$T/cc" SCALY_CACHE="$T/cache" "$TOOL" build --target wasm32-wasip1 --gpu "$src" -o "$T/page/$name.wasm" > "$T/page/$name.build" 2>&1; then
    echo "  FAIL $name: no wasm build ($(grep -m1 -i error "$T/page/$name.build" | cut -c1-120))"; failed=1; continue
  fi
  # a host without a card: the command line
  "$NODE" --no-warnings "$WEB/run.mjs" "$T/page/$name.wasm" > "$T/page/$name.nocard" 2>/dev/null; nocard_rc=$?
  case " $UNLIKE " in *" $name "*) native_rc=unlike ;; esac
  echo "$native_rc" > "$T/page/$name.rc"
  if [ "$native_rc" = unlike ]; then
    :
  elif [ "$native_rc" = 0 ]; then
    if [ "$nocard_rc" != 0 ] || ! cmp -s "$T/page/$name.native" "$T/page/$name.nocard"; then
      echo "  FAIL $name: without a card the wasm program does not write what the native one writes (rc $nocard_rc)"; failed=1
    fi
  elif [ "$nocard_rc" = 0 ]; then
    echo "  FAIL $name: natively rc $native_rc, as wasm rc 0"; failed=1
  fi
  programs+=("$name")
done
printf '%s\n' "${programs[@]}" | python3 -c 'import json,sys; print(json.dumps([l.strip() for l in sys.stdin if l.strip()]))' > "$T/page/programs.json"
cp tests/gpu/web/program.html tests/gpu/web/program-worker.mjs "$WEB/wasi.mjs" "$WEB/gpu.mjs" "$WEB/gpu-host.mjs" "$WEB/gpu-worker.mjs" "$T/page/"
page "$T/page" program.html 300

if grep -q '^SKIP' "$T/result" 2>/dev/null; then
  echo "gpu-web: programs SKIP (no card for a worker of the page)"; exit 0
fi
ON="$ON" python3 - "$T/result" "$T/page" "${#programs[@]}" <<'PY' || failed=1
import json, os, sys
result, page, expected = sys.argv[1], sys.argv[2], int(sys.argv[3])
forced = os.environ['ON'].split()
lines = open(result).read().splitlines() if os.path.exists(result) else []
bad = [l for l in lines if l.startswith('FAILED')]
runs = [json.loads(l[4:]) for l in lines if l.startswith('RUN ')]
by = {}
for r in runs:
    by.setdefault(r['name'], {})[r['mode']] = r
fails = [b[:300] for b in bad]
if not any(l == 'programs: %d' % expected for l in lines):
    fails.append('the page did not answer for %d programs (%s)' % (expected, lines[-1][:200] if lines else 'nothing'))
cards = {'on': 0, 'auto': 0}
for name, modes in sorted(by.items()):
    off = modes.get('off')
    if off is None or len(modes) != 3:
        fails.append(name + ': not all three runs'); continue
    native_rc = open(os.path.join(page, name + '.rc')).read().strip()
    native = open(os.path.join(page, name + '.native')).read()
    if off['cards'] != 0:
        fails.append(name + ': the card ran %d loops while off' % off['cards'])
    if native_rc == '0' and (off['status'] != '0' or off['stdout'] != native):
        fails.append(name + ': off, the page writes other than the native program (%s)' % off['status'])
    for mode in ('on', 'auto'):
        r = modes[mode]
        cards[mode] += r['cards']
        if r['stdout'] != off['stdout'] or r['status'] != off['status']:
            fails.append('%s: %s, the card changed what the program writes (%s; %s)' % (name, mode, r['status'], '; '.join(r['said'])[:200]))
    if name in forced and modes['on']['cards'] == 0:
        fails.append(name + ': forced, and no loop on the card (%s)' % '; '.join(modes['on']['said'])[:200])
    if name == 'gate' and modes['auto']['cards'] < 2:
        fails.append('gate: the gate gave the card %d loops, not two or more (%s)' % (modes['auto']['cards'], '; '.join(modes['auto']['said'])[:300]))
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: programs %s — %d programs in a page, three runs each; the card ran %d loops forced, %d by the gate'
      % ('FAIL' if fails else 'PASS', len(by), cards['on'], cards['auto']))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }

# --- 3. the compiler in the page ------------------------------------------
L="${SCALY_WASM_LLVM:-$HOME/scaly-wasm}"
if [ ! -d "$L/wasm-build/lib" ]; then
  echo "gpu-web: compiler SKIP (no libLLVM for wasm at $L: tools/wasm/build-llvm.sh)"
  echo "gpu-web: PASS"; exit 0
fi
# the compiler for the page, from the sources as they are (the tree's own
# tool builds it; its cache makes a second build short), then the page
if ! echo ':quit' | tools/wasm/repl.sh build > "$T/compiler.build" 2>&1 || ! web/repl/build.sh "$T/repl" > "$T/repl.build" 2>&1; then
  echo "gpu-web: FAIL — the compiler for the page does not build ($(tail -1 "$T/compiler.build" | cut -c1-120); $(tail -1 "$T/repl.build" 2>/dev/null | cut -c1-120))"; exit 1
fi
python3 - "$T/repl/sources.json" <<'PY'
import json, sys
wanted = [('integers32', 'tests/gpu/webgpu/integers32.scaly', ['off', 'on']),
          ('reductions', 'tests/gpu/reductions.scaly', ['off', 'on']),
          ('gate', 'tests/gpu/web/gate.scaly', ['off', 'auto'])]
json.dump([{'name': n, 'text': open(p).read(), 'modes': m} for n, p, m in wanted], open(sys.argv[1], 'w'))
PY
cp tests/gpu/web/compiler.html "$T/repl/"
page "$T/repl" compiler.html 300

if grep -q '^SKIP' "$T/result" 2>/dev/null; then
  echo "gpu-web: compiler SKIP (no card for a worker of the page)"
  echo "gpu-web: PASS"; exit 0
fi
python3 - "$T/result" "$T/page" <<'PY' || failed=1
import json, os, sys
result, page = sys.argv[1], sys.argv[2]
lines = open(result).read().splitlines() if os.path.exists(result) else []
fails = [l[:300] for l in lines if l.startswith('FAILED')]
if not any(l.startswith('compiled: ') for l in lines):
    fails.append('the page did not answer (%s)' % (lines[-1][:200] if lines else 'nothing'))
by = {}
for l in lines:
    if l.startswith('RUN '):
        r = json.loads(l[4:])
        by.setdefault(r['name'], {})[r['mode']] = r
cards = 0
for name, modes in sorted(by.items()):
    off = modes['off']
    native = open(os.path.join(page, name + '.native')).read()
    if off['status'] != '0' or off['stdout'] != native:
        fails.append('%s: compiled in the page it writes other than natively (%s; %s)' % (name, off['status'], off['stderr'][-200:]))
    if off['cards'] != 0:
        fails.append('%s: the card ran %d loops while off' % (name, off['cards']))
    for mode, r in modes.items():
        if mode == 'off':
            continue
        cards += r['cards']
        if r['stdout'] != off['stdout'] or r['status'] != off['status']:
            fails.append('%s: %s, the card changed what the program writes (%s; %s)' % (name, mode, r['status'], r['stderr'][-200:]))
        if r['cards'] < (2 if mode == 'auto' else 1):
            fails.append('%s: %s, the card ran %d loops' % (name, mode, r['cards']))
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: compiler %s — %d programs compiled in the page and run with --gpu; the card ran %d loops'
      % ('FAIL' if fails else 'PASS', len(by), cards))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }
echo "gpu-web: PASS"
