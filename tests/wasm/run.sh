#!/bin/bash
# The wasm profile (W1 slices 4 and 5): programs built for wasm32-wasip1 by the
# tool (`scaly build --target wasm32-wasip1`, the standard library and its
# `_wasm` native files compiled for the target into a scratch cache) and
# run under two WASI hosts: node's own, and the host a web page runs one on
# (packages/scaly/<version>/web/wasi.mjs, its file system in memory, driven from
# the command line by web/run.mjs beside it).
#
#   1. this directory's fixtures: the profile itself -- the program's own entry
#      (WASI's `__main_void` / `__main_argc_argv`, its arguments), allocation,
#      files and clocks over WASI, a parallel `for` run inline on the one core
#   2. the whole regression corpus, by tests/regress/run.py's rules (stdout says
#      PASS; `; expect-rc:` with `; expect-out:`; an `xfail_` must not
#      compile), every catch point and trap among them -- wasm exception
#      handling through compiler/wasm_eh.cpp and wasi-libc's libsetjmp. Skipped,
#      each with its reason: the IR gates of the host (`; expect-ir:`), what
#      links libLLVM (`; link: llvm`), and the fixtures tests/wasm/skip names
#      (a 64-bit host's layout written down as numbers, an untyped function
#      pointer to a function that answers a pointer).
#   3. two programs of tests/gpu built with --gpu: the module asks its host
#      for a graphics card, and without one writes what it writes without the
#      switch (the card itself is tests/gpu/web/run.sh's, in a browser)
#
# All three run once per host.
#
# Needs a wasi-sdk ($WASI_SDK, else ~/opt/wasi-sdk-*) and node; SKIPPED by name
# without either.
#
# Usage: tests/wasm/run.sh [stage-binary]   (default /tmp/scalyc_stage2; the
#        tool beside it, tools/scaly-of.sh)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
TOOL=$(tools/scaly-of.sh "$STAGE")
T=wasm32-wasip1

wasi="${WASI_SDK:-}"
if [ -z "$wasi" ]; then
  for cand in "$HOME"/opt/wasi-sdk-*; do [ -d "$cand" ] && wasi="$cand"; done
fi
if [ -z "$wasi" ] || [ ! -x "$wasi/bin/clang" ]; then
  echo "wasm: SKIP (no wasi-sdk: set WASI_SDK)"; exit 0
fi
# a node that STARTS (tools/node.sh): one that is merely there fails every run
NODE=$(tools/node.sh) || { echo "wasm: SKIP (no node that starts, to run a WASI module)"; exit 0; }
[ -x "$TOOL" ] || { echo "wasm: FAIL — no tool beside $STAGE ($TOOL)"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
# the tool's C compiler: the wasi-sdk's clang over its sysroot
printf '#!/bin/sh\nexec "%s/bin/clang" --sysroot="%s/share/wasi-sysroot" "$@"\n' "$wasi" "$wasi" > "$WORK/cc"
chmod 755 "$WORK/cc"
export SCALY_CC="$WORK/cc" SCALY_CACHE="$WORK/cache"
PAGE="packages/scaly/$(tools/version.sh scaly)/web/run.mjs"
JOBS=${REGRESS_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
PY=$(command -v python3 || command -v python) || { echo "wasm: no python3" >&2; exit 2; }

failed=0
for host in node page; do
  case $host in
    node) RUN="$NODE --no-warnings tests/wasm/run.mjs" ;;
    page) RUN="$NODE --no-warnings $PAGE" ;;
  esac

  # 1. the profile; built first and alone, which also fills the cache with the
  # standard library for the target before the corpus builds in parallel
  pass=0; fail=0; failures=()
  for src in tests/wasm/*.scaly; do
    n=$(basename "$src" .scaly)
    args=$(sed -n 's/^; run-args: *//p' "$src" | head -1)
    if ! "$TOOL" build --target $T "$src" -o "$WORK/$n.wasm" > "$WORK/$n.build" 2>&1; then
      fail=$((fail+1)); failures+=("$n(build: $(grep -m1 -i 'error' "$WORK/$n.build" | cut -c1-120))"); continue
    fi
    # shellcheck disable=SC2086
    out=$($RUN "$WORK/$n.wasm" $args 2>&1); rc=$?
    if [ "$rc" = 0 ] && [ "$(printf '%s\n' "$out" | grep -v '^$' | tail -1)" = PASS ]; then
      pass=$((pass+1))
    else
      fail=$((fail+1)); failures+=("$n(rc=$rc: $(printf '%s' "$out" | tail -c 160 | tr '\n' ' '))")
    fi
  done
  echo "wasm ($host): profile $pass PASS, $fail FAIL ${failures[*]}"
  [ $fail -eq 0 ] || failed=1

  # 2. the regression corpus
  REGRESS_TARGET=$T REGRESS_RUN="$RUN" REGRESS_SKIP=tests/wasm/skip REGRESS_NAME="wasm ($host): regress" \
    "$PY" tests/regress/run.py "$STAGE" "$TOOL" "" "$WORK" "$JOBS" || failed=1

  # 3. a program built with --gpu asks its host for a graphics card (the
  # import module scaly_gpu; one built without asks for nothing), and under
  # a host without a card -- both of these -- it writes what it writes
  # without the switch. The card itself: tests/gpu/web/run.sh, in a browser.
  pass=0; fail=0; failures=()
  for src in tests/gpu/webgpu/integers32.scaly tests/gpu/reductions.scaly; do
    n=$(basename "$src" .scaly)
    if [ ! -f "$WORK/$n.gpu.wasm" ]; then
      "$TOOL" build --target $T "$src" -o "$WORK/$n.plain.wasm" > "$WORK/$n.build" 2>&1 &&
        "$TOOL" build --target $T --gpu "$src" -o "$WORK/$n.gpu.wasm" >> "$WORK/$n.build" 2>&1 ||
        { fail=$((fail+1)); failures+=("$n(build: $(grep -m1 -i 'error' "$WORK/$n.build" | cut -c1-120))"); continue; }
    fi
    asks() { "$NODE" -e 'const m = new WebAssembly.Module(require("fs").readFileSync(process.argv[1])); process.exit(WebAssembly.Module.imports(m).some(i => i.module === "scaly_gpu") ? 0 : 1)' "$1"; }
    # shellcheck disable=SC2086
    plain=$($RUN "$WORK/$n.plain.wasm" 2>&1); prc=$?
    # shellcheck disable=SC2086
    card=$($RUN "$WORK/$n.gpu.wasm" 2>&1); crc=$?
    if [ "$prc" = 0 ] && [ "$crc" = 0 ] && [ -n "$plain" ] && [ "$plain" = "$card" ] && asks "$WORK/$n.gpu.wasm" && ! asks "$WORK/$n.plain.wasm"; then
      pass=$((pass+1))
    else
      fail=$((fail+1)); failures+=("$n(rc=$prc/$crc: $(printf '%s' "$card" | tail -c 120 | tr '\n' ' '))")
    fi
  done
  echo "wasm ($host): --gpu without a card $pass PASS, $fail FAIL ${failures[*]}"
  [ $fail -eq 0 ] || failed=1
done

[ $failed -eq 0 ]
