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
# All three run once per host. Then, on the page's host alone (node's WASI
# has no threads):
#
#   4. THE THREADS PROFILE, wasm32-wasip1-threads: the fiber suite's fixtures
#      of parallel loops, the task pool, threads and fork-join (THREADED
#      below), built for it and run on four threads each -- stdout and exit
#      code as their markers say --, and then the whole regression corpus
#      again. Not among them: parallel_read (a thread of a page has no files:
#      packages/scaly/<version>/web/thread.mjs says why) and
#      parallel_spike_no_fork (its verdict rests on windows of a few
#      microseconds, and a clock read through the host costs about that).
#
#   5. A CLUSTER IN ONE PROCESS: the programs of tests/cluster built for the
#      threads profile, their nodes workers of one node process on one
#      network of the host (web/net.mjs: the sockets WASI does not have),
#      and tests/cluster/wasm.mjs runs tests/cluster/run.sh's scenarios over
#      it -- ping-pong, the late frame, kill -9, the stamp mismatch, the
#      distributed trainer's parity and its fault --, and two of its own: a
#      node that falls silent, which only the heartbeat finds, and the render
#      farm of the cluster showcase (web/cluster/render.scaly, a plain `for`
#      built with --cluster), whose picture is the program's alone bit for
#      bit where helpers -- found on the network's group -- render its
#      passes, whatever helper is lost on the way.
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

# 4. the threads profile, on the page's host
THREADED="forkjoin_after_pool forkjoin_emit forkjoin_runtime parallel_alias parallel_defer parallel_for
  parallel_hotspot parallel_kept_value parallel_local_scratch parallel_reduce parallel_reduce_fp
  parfor_fault_cancels parfor_fault_caught parfor_fault_fatal shared_global taskgroup_dispose taskpool
  taskpool_default thread_alloc thread_churn thread_deep_stack thread_hello thread_reclaim trace_balance"
TT=wasm32-wasip1-threads
RUN="$NODE --no-warnings $PAGE"
pass=0; fail=0; failures=()
for n in $THREADED; do
  src=tests/fiber/$n.scaly
  if ! "$TOOL" build --target $TT "$src" -o "$WORK/t_$n.wasm" > "$WORK/t_$n.build" 2>&1; then
    fail=$((fail+1)); failures+=("$n(build: $(grep -m1 -i 'error' "$WORK/t_$n.build" | cut -c1-120))"); continue
  fi
  want=$(sed -n 's/^; Expected: //p' "$src")
  want_rc=$(sed -n 's/^; ExpectedExit: //p' "$src" | head -1)
  # shellcheck disable=SC2086
  out=$(SCALY_WORKERS=4 $RUN "$WORK/t_$n.wasm" 2>"$WORK/t_$n.err"); rc=$?
  if [ "$rc" = "${want_rc:-0}" ] && [ "$out" = "$want" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$n(rc=$rc: $(printf '%s' "$out" | tail -c 100 | tr '\n' ' ')$(tail -c 120 "$WORK/t_$n.err" | tr '\n' ' '))")
  fi
done
echo "wasm (threads): fixtures $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ] || failed=1
SCALY_WORKERS=4 REGRESS_TARGET=$TT REGRESS_RUN="$RUN" REGRESS_SKIP=tests/wasm/skip REGRESS_NAME="wasm (threads): regress" \
  "$PY" tests/regress/run.py "$STAGE" "$TOOL" "" "$WORK" "$JOBS" || failed=1

# 5. a cluster in one process
mkdir -p "$WORK/cluster"
built=1
for n in roundtrip xproc_recv xproc_send pingpong lateframe kill9 stamp_a stamp_b train; do
  if ! "$TOOL" build --target $TT tests/cluster/$n.scaly -o "$WORK/cluster/$n.wasm" > "$WORK/cluster/$n.build" 2>&1; then
    echo "wasm (cluster): FAIL — $n does not build ($(grep -m1 -i 'error' "$WORK/cluster/$n.build" | cut -c1-120))"
    built=0
  fi
done
if ! "$TOOL" build --target $TT --cluster web/cluster/render.scaly -o "$WORK/cluster/render.wasm" > "$WORK/cluster/render.build" 2>&1; then
  echo "wasm (cluster): FAIL — render does not build ($(grep -m1 -i 'error' "$WORK/cluster/render.build" | cut -c1-120))"
  built=0
fi
if [ $built -eq 1 ]; then
  "$NODE" --no-warnings tests/cluster/wasm.mjs "packages/scaly/$(tools/version.sh scaly)/web" "$WORK/cluster" || failed=1
else
  failed=1
fi

[ $failed -eq 0 ]
