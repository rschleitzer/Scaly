#!/bin/bash
# The wasm profile (W1 slice 4): programs built for wasm32-wasip1 by the tool
# (`scaly build --target wasm32-wasip1`, the standard library and its `_wasm`
# native files compiled for the target into a scratch cache) and run under
# node's WASI. What it holds: the program's own entry (WASI's `__main_void` /
# `__main_argc_argv`), allocation, files and clocks over WASI, a parallel `for`
# run inline on the one core, every catch point (`try`, a panic arm -- wasm
# exception handling through compiler/wasm_eh.cpp and wasi-libc's libsetjmp),
# and a trap's exit code with its message.
#
# A fixture passes as in tests/regress: stdout's last line says PASS, or with
# `; expect-rc: N` the run ends with N and stdout holds every `; expect-out:`.
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

FIXTURES="
tests/wasm/profile.scaly one
tests/regress/try_catch.scaly
tests/regress/try_arm_catches_panic.scaly
tests/regress/try_arm_catches_null.scaly
tests/regress/try_arm_catches_size.scaly
tests/regress/try_arm_catches_oom.scaly
tests/regress/try_arm_kind_selects.scaly
tests/regress/try_else_catches_panic.scaly
tests/regress/try_value_after_catch.scaly
tests/regress/trap_array_at.scaly
tests/regress/trap_slice_subscript.scaly
"

pass=0; fail=0; failures=()
while read -r src args; do
  [ -n "$src" ] || continue
  n=$(basename "$src" .scaly)
  if ! SCALY_CC="$WORK/cc" SCALY_CACHE="$WORK/cache" "$TOOL" build --target $T "$src" -o "$WORK/$n.wasm" > "$WORK/$n.build" 2>&1; then
    fail=$((fail+1)); failures+=("$n(build: $(grep -m1 -i 'error' "$WORK/$n.build" | cut -c1-120))"); continue
  fi
  # shellcheck disable=SC2086
  node --no-warnings tests/wasm/run.mjs "$WORK/$n.wasm" $args > "$WORK/$n.out" 2> "$WORK/$n.err"
  rc=$?
  want_rc=$(sed -n 's/^; expect-rc: *//p' "$src" | head -1)
  ok=1
  if [ -n "$want_rc" ]; then
    [ "$rc" = "$want_rc" ] || ok=0
    while IFS= read -r want; do
      [ -n "$want" ] || continue
      grep -qF -- "$want" "$WORK/$n.out" "$WORK/$n.err" || ok=0
    done < <(sed -n 's/^; expect-out: *//p' "$src")
  else
    [ "$rc" = 0 ] && grep -v '^$' "$WORK/$n.out" | tail -1 | grep -q PASS || ok=0
  fi
  if [ $ok = 1 ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$n(rc=$rc: $(tail -c 120 "$WORK/$n.out" "$WORK/$n.err" 2>/dev/null | tr '\n' ' ' | cut -c1-160))")
  fi
done <<< "$FIXTURES"

echo "wasm: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
