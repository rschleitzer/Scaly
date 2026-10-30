#!/bin/bash
# --target cross-emit suite: prove `scalyc -c --target <triple>` emits object
# code for a target other than the host — the four LP64 "fabulous four", the
# two Windows targets (stage 7) and WebAssembly (wasm32, 2026-10-19).
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

# WebAssembly (2026-10-19, the browser target, 32-bit pointers): the object
# begins with the magic `\0asm`, version 1, and the module's datalayout says
# 32-bit pointers -- a module without one would fold every size as LP64.
check_wasm() {
  local triple=$1 obj=/tmp/target_$2.o ll=/tmp/target_$2.ll
  rm -f "$obj" "$ll"
  "$STAGE" -c --target "$triple" -o "$obj" "$SRC" >/dev/null 2>&1
  local got; got=$(od -A n -t x1 -N 8 "$obj" 2>/dev/null | tr -d ' \n')
  "$STAGE" -S --target "$triple" -o "$ll" "$SRC" >/dev/null 2>&1
  if [ "$got" = "0061736d01000000" ] && grep -q '^target datalayout = ".*-p:32:32' "$ll"; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$triple: magic '$got', datalayout '$(grep '^target datalayout' "$ll" 2>/dev/null)'")
  fi
  rm -f "$obj" "$ll"
}
check_wasm wasm32-wasip1 wasm32wasi

# A triple no backend answers is refused, rc 2, with its name -- before
# 2026-10-19 the module silently kept no datalayout
out=$("$STAGE" -c --target no-such-triple -o /tmp/target_none_$$.o "$SRC" 2>&1); rc=$?
rm -f /tmp/target_none_$$.o
if [ $rc -eq 2 ] && printf '%s' "$out" | grep -qF "no target for the triple 'no-such-triple'"; then
  pass=$((pass+1))
else
  fail=$((fail+1)); failures+=("unknown triple: rc $rc, '$out'")
fi

# -mcpu reaches codegen: the same f64x4 function for
# x86-64 is SSE (xmm only) for the generic CPU and AVX (ymm) for haswell. The
# check reads the disassembly, so it needs llvm-objdump; without one (the
# Windows box) it says SKIP by name.
. tools/llvm-env.sh >/dev/null 2>&1 || true
OBJDUMP=
for cand in "$(dirname "${LLC:-/nonexistent/llc}")/llvm-objdump" "${LLVM_PREFIX:-/nonexistent}/bin/llvm-objdump" "llvm-objdump-${LLVM_MAJOR:-21}"; do
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

# -g for a Windows target writes CodeView, which link.exe puts into a .pdb for
# the Visual Studio debugger, not DWARF (a PE keeps no DWARF link.exe wrote);
# a generic type is named in template syntax there, Vector<int>, which is
# what the natvis views of tools/natvis/scaly.natvis match.
check_codeview() {
  local triple=$1 obj=/tmp/target_cv_$2.o
  rm -f "$obj"
  "$STAGE" -g -c --target "$triple" -o "$obj" tests/target/codeview.scaly >/dev/null 2>&1
  local sections; sections=$("$OBJDUMP" -h "$obj" 2>/dev/null)
  local names; names=$(LC_ALL=C grep -a -c 'Vector<int>' "$obj" 2>/dev/null)
  rm -f "$obj"
  if printf '%s' "$sections" | grep -qF '.debug$S' && ! printf '%s' "$sections" | grep -qF '.debug_info' && [ "${names:-0}" -gt 0 ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$triple -g: CodeView sections or the name Vector<int> missing")
  fi
}
if [ -n "$OBJDUMP" ]; then
  check_codeview x86_64-pc-windows-msvc  x86win
  check_codeview aarch64-pc-windows-msvc arm64win
else
  echo "target: SKIP CodeView check (no llvm-objdump)"
fi

echo "target: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
