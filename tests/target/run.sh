#!/bin/bash
# --target cross-emit suite: prove `scalyc -c --target <triple>` emits object
# code for a target other than the host — the four LP64 "fabulous four", plus
# the two Windows targets (stage 7).
#
# The self-hosted compiler is host-only for LINKING (write_object_to_file used
# to hardcode LLVMGetDefaultTargetTriple), but the emitted IR carries no
# `target triple` (only a datalayout, stamped per invocation from the
# TargetMachine) and both the AArch64 and X86 backends are initialized, so
# llc/TargetMachine can retarget object emission to any of them. This suite
# checks the OBJECT's architecture — it does not link/run (that needs each
# target's sysroot + libscaly.a), so it runs on any host in CI without
# cross-toolchains.
#
# The two Windows arms (2026-08-09) were added when the stage-7 port opened and
# passed on the FIRST try, with no compiler change: LLVM IR has no `long`, only
# iN, so the seed's baked LP64 sizes carry to LLP64 unchanged — pointers are 8
# bytes on Win64 too. That confirms the roadmap's hypothesis that the classic
# LLP64 trap misses the language and leaves only the C-ABI boundary exposed
# (guarded by check 4 of tests/abi/run.sh). Linking is a separate question and
# is gated by the windows job in .github/workflows/seed.yml.
#
# Usage: tests/target/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
SRC=tests/aot/hello.scaly
pass=0; fail=0; failures=()

# triple -> (format token, arch token) that `file` must BOTH report. Checked
# independently because GNU file (Linux) and BSD file (macOS) order the words
# differently ("Mach-O 64-bit x86_64 object" vs "Mach-O 64-bit object x86_64"),
# so a fixed phrase is not portable — but both always contain the format name
# and the arch name somewhere.
check() {
  local triple=$1 fmt=$2 arch=$3 obj=/tmp/target_$4.o
  rm -f "$obj"
  "$STAGE" -c --target "$triple" -o "$obj" "$SRC" >/dev/null 2>&1
  local desc; desc=$(file "$obj" 2>/dev/null)
  if printf '%s' "$desc" | grep -qF "$fmt" && printf '%s' "$desc" | grep -qF "$arch"; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$triple: '$desc'")
  fi
}

# COFF is checked on the MACHINE field instead of through `file`. The four
# targets above are reported by both BSD and GNU `file`; COFF support varies
# between builds, and the field is the ground truth anyway — the first two
# bytes of a COFF object are IMAGE_FILE_MACHINE, little-endian
# (0x8664 AMD64 -> "6486", 0xaa64 ARM64 -> "64aa"). Whitespace is stripped, so
# BSD and GNU `od` spacing does not matter.
check_coff() {
  local triple=$1 machine=$2 obj=/tmp/target_$3.o
  rm -f "$obj"
  "$STAGE" -c --target "$triple" -o "$obj" "$SRC" >/dev/null 2>&1
  local got; got=$(od -A n -t x1 -N 2 "$obj" 2>/dev/null | tr -d ' \n')
  if [ "$got" = "$machine" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$triple: machine '$got', expected '$machine'")
  fi
}

check x86_64-linux-gnu    ELF     x86-64   x86lin
check aarch64-linux-gnu   ELF     aarch64  arm64lin
check x86_64-apple-darwin Mach-O  x86_64   x86mac
check arm64-apple-darwin  Mach-O  arm64    arm64mac
check_coff x86_64-pc-windows-msvc  6486  x86win
check_coff aarch64-pc-windows-msvc 64aa  arm64win

# -mcpu reaches codegen (ROADMAP-simd.md phase 3): the same f64x4 function for
# x86-64 is SSE (xmm only) for the generic CPU and AVX (ymm) for haswell. The
# check reads the disassembly, so it needs llvm-objdump; without one (the
# Windows box) it says SKIP by name.
. tools/llvm-env.sh >/dev/null 2>&1 || true
OBJDUMP=
for cand in "$(dirname "${LLC:-/nonexistent/llc}")/llvm-objdump" "${LLVM_PREFIX:-/nonexistent}/bin/llvm-objdump" llvm-objdump-20; do
  if command -v "$cand" >/dev/null 2>&1; then OBJDUMP=$cand; break; fi
done
check_cpu() {
  local cpu_flag=$1 want_ymm=$2 obj=/tmp/target_cpu_$$.o
  rm -f "$obj"
  "$STAGE" -c --no-prelude --target x86_64-linux-gnu $cpu_flag -o "$obj" tests/target/simd_cpu.scaly >/dev/null 2>&1
  local ymm; ymm=$("$OBJDUMP" -d "$obj" 2>/dev/null | grep -c ymm)
  local xmm; xmm=$("$OBJDUMP" -d "$obj" 2>/dev/null | grep -c xmm)
  rm -f "$obj"
  if { [ "$want_ymm" = 1 ] && [ "$ymm" -gt 0 ]; } || { [ "$want_ymm" = 0 ] && [ "$ymm" -eq 0 ] && [ "$xmm" -gt 0 ]; }; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("-mcpu '${cpu_flag:-generic}': ymm $ymm, xmm $xmm")
  fi
}
if [ -n "$OBJDUMP" ]; then
  check_cpu "" 0
  check_cpu "-mcpu=haswell" 1
else
  echo "target: SKIP -mcpu check (no llvm-objdump)"
fi

echo "target: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
