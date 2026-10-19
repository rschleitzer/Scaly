#!/bin/bash
# The wasm profile (W1 slices 4 and 5): programs built for wasm32-wasip1 by the
# tool (`scaly build --target wasm32-wasip1`, the standard library and its
# `_wasm` native files compiled for the target into a scratch cache) and run
# under node's WASI.
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
if ! command -v node >/dev/null 2>&1; then
  echo "wasm: SKIP (no node to run a WASI module)"; exit 0
fi
[ -x "$TOOL" ] || { echo "wasm: FAIL — no tool beside $STAGE ($TOOL)"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
# the tool's C compiler: the wasi-sdk's clang over its sysroot
printf '#!/bin/sh\nexec "%s/bin/clang" --sysroot="%s/share/wasi-sysroot" "$@"\n' "$wasi" "$wasi" > "$WORK/cc"
chmod 755 "$WORK/cc"
export SCALY_CC="$WORK/cc" SCALY_CACHE="$WORK/cache"
RUN="node --no-warnings tests/wasm/run.mjs"

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
echo "wasm: profile $pass PASS, $fail FAIL ${failures[*]}"

# 2. the regression corpus
JOBS=${REGRESS_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)}
PY=$(command -v python3 || command -v python) || { echo "wasm: no python3" >&2; exit 2; }
REGRESS_TARGET=$T REGRESS_RUN="$RUN" REGRESS_SKIP=tests/wasm/skip REGRESS_NAME="wasm: regress" \
  "$PY" tests/regress/run.py "$STAGE" "$TOOL" "" "$WORK" "$JOBS"
corpus=$?

[ $fail -eq 0 ] && [ $corpus -eq 0 ]
