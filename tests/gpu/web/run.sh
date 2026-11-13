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
#    without, the card having run its loops where it is on. Then the
#    showcase's editor (web/fractals, index.html?selftest=build): the page
#    BUILDS the showcase's program for wasm32-wasip1-threads with that
#    compiler (playground.json, tools/wasm/precache.mjs) and runs it on all
#    cores -- its picture must be the prebuilt program's --, and a version
#    that does not compile must show the compiler's message. And the Scaly
#    Playground's own chapters (playground.html?selftest), in English and in
#    German (&lang=de): every step runs, the tour's step on values shows
#    each value beside its line (web/repl/notes.mjs), a task's program as
#    given is "not yet" and its solution is praised, a drawing step shows
#    its drawing, and the step for the cores draws on
#    one core what it draws on all of them -- and what the program draws
#    natively. One of them
#    (canvas.scaly) writes a picture, which the page's worker must hand
#    back: the same but for a hundredth of its points at most -- its loop is
#    `relaxed`.
#
# 4. THE CORES OF THE PAGE. Programs built for wasm32-wasip1-threads run in
#    a page (cores.html) on one core and on all the browser gives: the page
#    is the coordinator of their threads (web/threads.mjs, web/thread.mjs),
#    the program runs in a worker, each thread in one of its own. Each run
#    must write what the native program writes, a run on more than one core
#    must have started threads (but where the card took its loops), and one
#    run has the card beside the cores.
#    The times are printed, not judged.
#
# 5. THE SHOWCASE (web/fractals): built by its build.sh, its page's
#    self-test (index.html?selftest) runs the program -- the Mandelbrot set
#    -- on one core, on all, on the card and as the runtime decides. The
#    one-core picture must be the native program's byte for byte, all
#    cores' the same, the card's other in at most one point in a hundred
#    (`relaxed`). Then every other fractal, small, from its first view, on
#    one core -- the native program's picture again -- and on the card,
#    which must have taken the loop; how many of its points differ is
#    printed (the card's floating point, `relaxed`, and chaos: the Lyapunov
#    fractal is made of places where a last digit decides).
#
# 6. THE CLUSTER SHOWCASE (web/cluster): built by its build.sh, its page's
#    self-test (index.html?selftest) renders a small picture on three nodes,
#    kills one once it has rendered a tile and silences another, then
#    renders it alone. Both pictures must be the native program's, bit for
#    bit, and what happened must say each node gone and its tile handed on.
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
ON="nested reductions shapes integers32 relaxed gate canvas"
# the programs that do not end under wasm as they end natively: fault_soft
# divides by zero, which has no result -- this machine's processor answers
# 0, wasm stops the program. With and without the card it must end alike.
UNLIKE="fault_soft"

T=$(mktemp -d "${TMPDIR:-/tmp}/scaly_gpu_web.XXXXXX") || exit 1
trap 'rm -rf "${T:?}"' EXIT
mkdir -p "$T/site" "$T/bin" "$T/page" "$T/cwd"
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
for src in tests/gpu/*.scaly tests/gpu/webgpu/*.scaly tests/gpu/web/gate.scaly tests/gpu/web/canvas.scaly; do
  name=$(basename "$src" .scaly)
  # a program is known here by its name: two of one name would be compared with each other
  [ ! -e "$T/bin/$name.build" ] || { echo "gpu-web: FAIL — two programs are called $name"; exit 1; }
  "$STAGE" --gpu -o "$T/bin/$name" "$src" >"$T/bin/$name.build" 2>&1 || continue
  mkdir -p "$T/site/$name"
  # in a directory of its own: a program may write a file where it stands
  (cd "$T/cwd" && SCALY_GPU=on SCALY_GPU_BACKEND=webgpu SCALY_GPU_DUMP="$T/site/$name" "$T/bin/$name" >/dev/null 2>&1)
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
  (cd "$T/cwd" && SCALY_GPU=off "$T/bin/$name" > "$T/page/$name.native" 2>/dev/null); native_rc=$?
  if ! SCALY_CC="$T/cc" SCALY_CACHE="$T/cache" "$TOOL" build --target wasm32-wasip1 --gpu "$src" -o "$T/page/$name.wasm" > "$T/page/$name.build" 2>&1; then
    echo "  FAIL $name: no wasm build ($(grep -m1 -i error "$T/page/$name.build" | cut -c1-120))"; failed=1; continue
  fi
  # a host without a card: the command line
  (cd "$T/cwd" && "$NODE" --no-warnings "$OLDPWD/$WEB/run.mjs" "$T/page/$name.wasm" > "$T/page/$name.nocard" 2>/dev/null); nocard_rc=$?
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

# --- 4. the cores of the page ---------------------------------------------
mkdir -p "$T/cores"
TT=wasm32-wasip1-threads
for entry in "gate:tests/gpu/web/gate.scaly:" "canvas:tests/gpu/web/canvas.scaly:" \
             "reduce:tests/fiber/parallel_reduce_fp.scaly:" "gate_gpu:tests/gpu/web/gate.scaly:--gpu"; do
  IFS=: read -r name src flag <<< "$entry"
  # shellcheck disable=SC2086
  if ! SCALY_CC="$T/cc" SCALY_CACHE="$T/cache" "$TOOL" build --target $TT $flag "$src" -o "$T/cores/$name.wasm" > "$T/cores/$name.build" 2>&1; then
    echo "  FAIL $name: no build for $TT ($(grep -m1 -i error "$T/cores/$name.build" | cut -c1-120))"; failed=1
  fi
done
cp "$T/page/gate.native" "$T/cores/gate.native"; cp "$T/page/gate.native" "$T/cores/gate_gpu.native"
cp "$T/page/canvas.native" "$T/cores/canvas.native"
sed -n 's/^; Expected: //p' tests/fiber/parallel_reduce_fp.scaly > "$T/cores/reduce.native"
cat > "$T/cores/cores.json" <<'JSON'
[{"name": "gate", "wasm": "gate.wasm", "workers": 1},
 {"name": "gate", "wasm": "gate.wasm", "workers": "all"},
 {"name": "canvas", "wasm": "canvas.wasm", "workers": "all"},
 {"name": "reduce", "wasm": "reduce.wasm", "workers": "all"},
 {"name": "gate_gpu", "wasm": "gate_gpu.wasm", "workers": "all", "gpu": "auto"}]
JSON
cp tests/gpu/web/cores.html tests/gpu/web/cores-worker.mjs "$WEB/wasi.mjs" "$WEB/threads.mjs" "$WEB/thread.mjs" \
   "$WEB/gpu.mjs" "$WEB/gpu-host.mjs" "$WEB/gpu-worker.mjs" "$T/cores/"
page "$T/cores" cores.html 300
python3 - "$T/result" "$T/cores" <<'PY' || failed=1
import json, os, sys
result, cores = sys.argv[1], sys.argv[2]
lines = open(result).read().splitlines() if os.path.exists(result) else []
fails = [l[:300] for l in lines if l.startswith('FAILED')]
runs = [json.loads(l[4:]) for l in lines if l.startswith('RUN ')]
if len(runs) != 5:
    fails.append('the page answered for %d of 5 runs (%s)' % (len(runs), lines[-1][:200] if lines else 'nothing'))
times = {}
for r in runs:
    native = open(os.path.join(cores, r['name'] + '.native')).read()
    what = '%s on %d of %d cores%s' % (r['name'], r['workers'], r['cpus'], ' and the card' if r.get('gpu') else '')
    if r['status'] != '0' or r['stdout'] != native:
        fails.append('%s: %s, wrote %r (%s%s)' % (what, r['status'], r['stdout'][-120:], r.get('stderr', '')[-150:], r['thread_out'][-150:]))
    if not r.get('threaded'):
        fails.append('%s: the module asks for no threads' % what)
    # with the card beside them the cores may never be asked: the card takes the loop at its gate
    if r['workers'] > 1 and r['cpus'] > 1 and r['started'] < 1 and not r.get('gpu'):
        fails.append('%s: no thread started' % what)
    if r.get('gpu') and r.get('cards', 0) < 1:
        fails.append('%s: the card took no loop' % what)
    times[(r['name'], r['workers'] > 1, bool(r.get('gpu')))] = r.get('ms', 0)
for f in fails:
    print('  FAIL ' + f)
one, many, card = times.get(('gate', False, False)), times.get(('gate', True, False)), times.get(('gate_gpu', True, True))
cpus = runs[0]['cpus'] if runs else 0
print('gpu-web: cores %s — %d runs in a page; gate.scaly %s ms on one core, %s ms on %d%s'
      % ('FAIL' if fails else 'PASS', len(runs), round(one or 0), round(many or 0), cpus,
         ', %s ms with the card beside them' % round(card) if card else ''))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }

# --- 5. the showcase ------------------------------------------------------
if ! web/fractals/build.sh "$T/show" "$STAGE" > "$T/show.build" 2>&1; then
  echo "gpu-web: FAIL — the showcase does not build ($(tail -1 "$T/show.build" | cut -c1-160))"; exit 1
fi
"$STAGE" --gpu -o "$T/bin/showcase" web/fractals/fractals.scaly > "$T/bin/showcase.build" 2>&1 ||
  { echo "gpu-web: FAIL — the showcase's program does not build natively"; exit 1; }
(cd "$T/cwd" && SCALY_GPU=off SCALY_WORKERS=1 "$T/bin/showcase" 600 400 250 -500000000 0 3000000000 1 0 > /dev/null 2>&1)
page "$T/show" "index.html?selftest" 600
python3 - "$T/result" "$T/cwd/fractal.600x400.rgba" "$T/bin/showcase" "$T/cwd" <<'PY' || failed=1
import json, os, subprocess, sys
result, native, program, cwd = sys.argv[1:5]
fails = []
try:
    r = json.load(open(result))
except Exception as e:
    print('  FAIL the page did not answer (%s)' % (open(result).read()[:200] if os.path.exists(result) else 'nothing'))
    sys.exit(1)
def fnv(path):
    h = 2166136261
    for b in open(path, 'rb').read():
        h = ((h ^ b) * 16777619) & 0xffffffff
    return h
h = fnv(native)
if r['args'] != ['600', '400', '250', '-500000000', '0', '3000000000', '1', '0']:
    fails.append('the self-test ran %s, not what was compared natively' % r['args'])
if r['hash'] != h:
    fails.append('one core: the picture is not the native program\'s')
by = {x['label']: x for x in r['runs']}
for x in r['runs']:
    if x['status'] != '0':
        fails.append('%s: ended with %s' % (x['label'], x['status']))
    limit = 0 if x['gpu'] == 'off' else 2400
    if x['differ'] is None or x['differ'] > limit:
        fails.append('%s: %s of 240000 points other than on one core' % (x['label'], x['differ']))
    if x['gpu'] == 'on' and x['cards'] < 1:
        fails.append('%s: the card took no loop' % x['label'])
want = 4 if r['card'] else 2
if len(r['runs']) != want:
    fails.append('%d runs, not %d' % (len(r['runs']), want))
gallery = r.get('gallery') or []
if len(gallery) != 15:
    fails.append('%d other fractals in the self-test, not 15' % len(gallery))
said = []
for g in gallery:
    picture = os.path.join(cwd, 'fractal.%sx%s.rgba' % (g['args'][0], g['args'][1]))
    if os.path.exists(picture):
        os.remove(picture)
    env = dict(os.environ, SCALY_GPU='off', SCALY_WORKERS='1')
    subprocess.run([program] + g['args'], cwd=cwd, env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if g['status'] != '0':
        fails.append('%s: ended with %s' % (g['slug'], g['status']))
    elif not g['args'][2].isdigit() or int(g['args'][2]) < 1:
        fails.append('%s: the page asked for %r turns' % (g['slug'], g['args'][2]))
    elif g.get('colours', 0) < 200:
        fails.append('%s: a picture of %s colours -- of nothing' % (g['slug'], g.get('colours')))
    elif not os.path.exists(picture) or g['hash'] != fnv(picture):
        fails.append('%s, one core: the picture is not the native program\'s' % g['slug'])
    c = g.get('card')
    if r['card']:
        if not c or c['status'] != '0' or c['cards'] < 1:
            fails.append('%s: the card did not take the loop (%s)' % (g['slug'], c))
        else:
            said.append('%s %s' % (g['slug'], c['differ']))
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: showcase %s — %s' % ('FAIL' if fails else 'PASS',
      ', '.join('%s %.0f ms' % (x['label'], x['ms'] or 0) for x in r['runs'])))
if said:
    print('gpu-web: showcase — of 24000 points the card drew otherwise: ' + ', '.join(said))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }

# --- 6. the cluster showcase ---------------------------------------------
if ! web/cluster/build.sh "$T/farm" "$STAGE" > "$T/farm.build" 2>&1; then
  echo "gpu-web: FAIL — the cluster showcase does not build ($(tail -1 "$T/farm.build" | cut -c1-160))"; exit 1
fi
"$STAGE" -o "$T/bin/farm" web/cluster/render.scaly > "$T/bin/farm.build" 2>&1 ||
  { echo "gpu-web: FAIL — the render farm does not build natively"; exit 1; }
native=$(RENDER_ROLE=s RENDER_WIDTH=128 RENDER_HEIGHT=96 RENDER_TILE=16 RENDER_PASSES=6 RENDER_SAMPLES=2 "$T/bin/farm" | sed -n 's/^C: image //p')
page "$T/farm" "index.html?selftest" 300
python3 - "$T/result" "$native" <<'PY' || failed=1
import json, os, sys
result, native = sys.argv[1:3]
try:
    r = json.load(open(result))
except Exception:
    print('  FAIL the cluster page did not answer (%s)' % (open(result).read()[:200] if os.path.exists(result) else 'nothing'))
    sys.exit(1)
fails = []
c, a = r['cluster'], r['alone']
if not native:
    fails.append('the native program said no hash')
if c['status'] != 0 or c['hash'] != native:
    fails.append('the cluster rendered another picture (%s, status %s) than the native program (%s)' % (c['hash'], c['status'], native))
if a['status'] != 0 or a['hash'] != native:
    fails.append('the program alone rendered another picture (%s, status %s)' % (a['hash'], a['status']))
if r['verdict'] != 'same':
    fails.append('the page did not say the pictures are the same (%s)' % r['verdict'])
said = ' / '.join(c['events'])
for want in ('Node 2 killed', 'finds node 2 gone', 'Node 3 falls silent', 'suspects node 3', 'goes back to the queue'):
    if want not in said:
        fails.append('no "%s" among what happened: %s' % (want, said))
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: cluster %s — three nodes, one killed and one silenced, %.0f ms; alone %.0f ms; tiles %s' %
      ('FAIL' if fails else 'PASS', c['ms'] or 0, a['ms'] or 0, c['tiles']))
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
          ('gate', 'tests/gpu/web/gate.scaly', ['off', 'auto']),
          ('canvas', 'tests/gpu/web/canvas.scaly', ['off', 'on'])]
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
    for mode, r in modes.items():
        if not r['began']:
            fails.append('%s: %s, the worker did not say when the program began' % (name, mode))
        if name == 'canvas':
            p = r['pictures']
            if len(p) != 1 or (p[0]['width'], p[0]['height']) != (300, 200):
                fails.append('canvas: %s, the worker handed back %s, not one picture of 300 x 200' % (mode, p))
            elif p[0]['differ'] > 600:
                # relaxed: a card's floating point may put a point on the other side of the edge
                fails.append('canvas: %s, %d of 60000 points other than without the card' % (mode, p[0]['differ']))
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: compiler %s — %d programs compiled in the page and run with --gpu; the card ran %d loops'
      % ('FAIL' if fails else 'PASS', len(by), cards))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }

# the showcase's editor: a build in the page
if ! web/fractals/build.sh "$T/repl/fractals" "$STAGE" > "$T/show2.build" 2>&1; then
  echo "gpu-web: FAIL — the showcase does not build ($(tail -1 "$T/show2.build" | cut -c1-160))"; exit 1
fi
page "$T/repl" "fractals/index.html?selftest=build" 300
python3 - "$T/result" <<'PY' || failed=1
import json, os, sys
result = sys.argv[1]
try:
    b = json.load(open(result))['built']
except Exception:
    print('  FAIL the page did not answer (%s)' % (open(result).read()[:200] if os.path.exists(result) else 'nothing'))
    sys.exit(1)
fails = []
if not b or not b.get('ok'):
    fails.append('the page did not build the program: %s %s' % (b and b.get('buildstatus'), b and b.get('diagnostics', '')[:200]))
elif not b.get('same'):
    fails.append('built in the page, on all cores: not the prebuilt program\'s picture (%s)' % b.get('status'))
if b and b.get('broken_ok') is not False:
    fails.append('a version that does not compile was built')
elif b and 'unknown identifier' not in b.get('broken_said', ''):
    fails.append('a version that does not compile: no message (%r)' % b.get('broken_said', '')[:200])
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: editor %s — %s' % ('FAIL' if fails else 'PASS', b and b.get('buildstatus')))
sys.exit(1 if fails else 0)
PY
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }

# the playground's own chapters; the picture of its step for the cores,
# drawn natively to compare with
python3 tests/tour/listings.py docs/playground/scaling.xml pg-mandelbrot "$T/pgnative" > /dev/null || failed=1
rm -rf "$T/pgcwd"; mkdir -p "$T/pgcwd"
home=$(pwd)
case "$TOOL" in /*) tool_abs=$TOOL ;; *) tool_abs="$home/$TOOL" ;; esac
(cd "$T/pgcwd" && SCALY_HOME="$home" "$tool_abs" run "$(ls "$T"/pgnative/*.scaly | head -1)" > /dev/null 2>&1)
# in English and in German (the tour and the chapters translated)
for lang in en de; do
page "$T/repl" "playground.html?selftest&lang=$lang" 300
python3 - "$T/result" "$T/pgcwd/mandelbrot.600x400.rgba" "$lang" <<'PY' || failed=1
import json, os, sys
result, native, lang = sys.argv[1], sys.argv[2], sys.argv[3]
try:
    r = json.load(open(result))
except Exception:
    print('  FAIL the playground did not answer (%s)' % (open(result).read()[:200] if os.path.exists(result) else 'nothing'))
    sys.exit(1)
fails = []
if r.get('lang') != lang:
    fails.append('asked for %s, the playground is in %s' % (lang, r.get('lang')))
h = 2166136261
for b in (open(native, 'rb').read() if os.path.exists(native) else b''):
    h = ((h ^ b) * 16777619) & 0xffffffff
if not r['report']:
    fails.append('no step of its own chapters ran')
for step in r['report']:
    for run in step['runs']:
        what = step['title'] + (' (%s)' % run['mode'] if run['mode'] else '')
        if not run['ok']:
            fails.append('%s: %s %s' % (what, run['status'], run['output'][:120]))
        if step['chapter'] == 'pg-drawing' and run['drawings'] < 1:
            fails.append('%s: no drawing shown' % what)
    if step['gpu']:
        hashes = {p['hash'] for run in step['runs'] for p in run['pictures']}
        if len(hashes) != 1:
            fails.append('%s: the pictures on one core and on all differ (%s)' % (step['title'], hashes))
        elif hashes != {h}:
            fails.append('%s: the picture is not the one the program draws natively' % step['title'])
# the values beside the code (notes.mjs), of the tour's step on values:
# `let answer 42`, `var count 0`, `count := count + 1`
notes = r.get('notes') or {}
want = {'1': ['answer', '42'], '2': ['count', '0'], '3': ['count', '1']}
if any(notes.get(k) != v for k, v in want.items()):
    fails.append('the values beside the code are %s, not %s' % (notes, want))
# a task (Your Turn, pg-sum): the program as given is not yet what it asks
# for, its solution is -- and the lizard says so
task = r.get('task')
if not task:
    fails.append('no task found (pg-sum)')
else:
    if not task['before']['shown'] or task['before']['ok']:
        fails.append('the task program as given: %s, not "not yet"' % task['before'])
    if not task['after']['shown'] or not task['after']['ok']:
        fails.append('the task solution: %s, not praised' % task['after'])
    if task['hints'] < 1:
        fails.append('the task has no hints')
for f in fails:
    print('  FAIL ' + f)
print('gpu-web: playground (%s) %s — %d steps of its own chapters ran, %d steps in all, %d lines with values' % (lang, 'FAIL' if fails else 'PASS', len(r['report']), r['steps'], len(notes)))
sys.exit(1 if fails else 0)
PY
done
[ $failed -eq 0 ] || { echo "gpu-web: FAIL"; exit 1; }
echo "gpu-web: PASS"
