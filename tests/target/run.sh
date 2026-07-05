#!/bin/bash
# --target cross-emit suite: prove `scalyc -c --target <triple>` emits object
# code for another LP64 "fabulous four" target, not just the host.
#
# The self-hosted compiler is host-only for LINKING (write_object_to_file used
# to hardcode LLVMGetDefaultTargetTriple), but the emitted IR is target-neutral
# (no datalayout line, LP64 sizes baked in) and both the AArch64 and X86
# backends are initialized, so llc/TargetMachine can retarget object emission to
# any of the four LP64 targets. This suite checks the OBJECT's architecture via
# `file` — it does not link/run (that needs each target's sysroot + libscaly.a),
# so it runs on any host in CI without cross-toolchains.
#
# Usage: tests/target/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
SRC=tests/aot/hello.scaly
pass=0; fail=0; failures=()

# triple -> substring that `file` must report for a correct object.
check() {
  local triple=$1 want=$2 obj=/tmp/target_$3.o
  rm -f "$obj"
  "$STAGE" -c --target "$triple" -o "$obj" "$SRC" >/dev/null 2>&1
  local desc; desc=$(file "$obj" 2>/dev/null)
  if printf '%s' "$desc" | grep -qF "$want"; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$triple: '$desc'")
  fi
}

check x86_64-linux-gnu   "x86-64"        x86lin
check aarch64-linux-gnu  "aarch64"       arm64lin
check x86_64-apple-darwin "Mach-O 64-bit object x86_64" x86mac
check arm64-apple-darwin  "Mach-O 64-bit object arm64"  arm64mac

echo "target: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
