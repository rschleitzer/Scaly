#!/bin/bash
# Build the .ll seed (Step D distribution) and verify the fixed point.
#
# The seed is the self-hosted compiler shipped as its own emitted LLVM IR:
# main.ll + scalyc.ll + scaly.ll, emitted with --no-tests (test/test_*
# functions are dead from cli.main and reference uninstantiated generics, so
# omitting them lets the seed link with zero undefined symbols). The seed is
# the FIRST execution of stage-emitted stdlib code — the bootstrap fixed point
# never RUNS it (stage binaries link C++-built libscaly.a), so this script
# both BUILDS and VERIFIES the seed end-to-end.
#
# Verifies:
#   1. seed links with NO undefined symbols (no -Wl,-undefined,dynamic_lookup)
#   2. seed compiles+runs hello.scaly  -> "Hello, World!"
#   3. seed compiles+runs the AOT corpus (tests/aot)
#   4. FIXED POINT: seed re-emits the 3 .ll byte-identical to the reference
#
# Usage: tools/seed.sh [self-hosted-compiler] [out-dir]
#   self-hosted-compiler  default /tmp/scalyc_stage2 (canonical promoted stage);
#                         if it does not exist, tools/bootstrap.sh builds it.
#   out-dir               default dist/seed (gitignored; per-target artifact)
#
# TURNKEY per platform: on a fresh checkout with the deps below installed, run
# `tools/seed.sh` (no args) -> bootstraps stage-2 -> emits + verifies the seed.
# ★The emission target is NAMED (`SEED_TARGET`, default `arm64-apple-darwin`),
# not the host's — see the block above it for why, and do not make it the host
# again. The BOOTSTRAP is still host-only; only the emitted .ll is portable.
#   The "fabulous four" LP64 targets and how to install deps:
#     (cmake went with the retired C++ stage-0; openjade became optional with
#      stage 8 — ./mkp uses our own DSSSL engine and skips when none exists.)
#     arm64-apple-darwin   brew install llvm@20
#     x86_64-apple-darwin  (Intel Mac)  same brew formula
#     x86_64-linux-gnu     apt install llvm-20-dev clang-20 \
#                                      zlib1g-dev libzstd-dev
#     aarch64-linux-gnu    same apt packages (arm64 Ubuntu)
# Override LLVM detection with LLVM20=/path; see tools/llvm-env.sh.
#
# .ll artifacts are NOT checked into the repo: ~12MB, target DataLayout + LLVM
# version baked in (union payload sizes computed at emission). One .ll set per
# target triple, regenerated and fixed-point-verified per release.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "SEED: FAIL — LLVM 20 not found"; exit 1; }

CC=${1:-/tmp/scalyc_stage2}
OUT=${2:-dist/seed}
CLANG=${CLANG:-clang}

# On Linux, stock GNU ld (BFD) fails to link libLLVM-20 ("failed to set dynamic
# section sizes: bad value"); lld handles it. Mirror tools/build-from-seed.sh so
# seed.sh's OWN link steps (scalyc_seed, scalyls) work on Linux too. macOS ld64
# links fine, so LINKARGS stays empty there.
LINKARGS=()
if [ "$(uname -s)" = "Linux" ]; then
  for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld ld.lld-20; do
    p=$(command -v "$c" 2>/dev/null || true)
    [ -n "$p" ] && { LINKARGS+=("-fuse-ld=$p"); break; }
  done
fi

mkdir -p "$OUT"
fail() { echo "SEED: FAIL — $1"; exit 1; }

if [ ! -x "$CC" ]; then
  echo "seed: $CC not found — bootstrapping"
  tools/bootstrap.sh || fail "bootstrap"
  CC=/tmp/scalyc_stage2          # bootstrap.sh's output
  [ -x "$CC" ] || fail "bootstrap produced no $CC"
fi

# ★★★THE SEED'S TARGET IS NAMED, NOT INHERITED FROM THE HOST (2026-09-20).
# Every emission below and the fixed-point re-emission further down must pass
# the SAME --target, or they compare different things.
#
# Why it cannot be the host: `Emitter.set_linkonce_odr` gives a body-carrying
# `linkonce_odr` function a COMDAT on COFF, and `llc` REFUSES a comdat on
# Mach-O ("MachO doesn't support COMDATs"). One text serves every target, so a
# seed emitted on a Windows box would carry 6787 of them and be unusable on
# both Apple targets — silently, because the emitting host's own build works.
# Until development moved to a Windows box this could not happen and the flag
# was not needed; the `-m:o … -n32:64-S128-Fn32` datalayout of the committed
# seed is what says the previous box was an arm64 Mac.
#
# ★Naming it also makes the seed REPRODUCIBLE: measured 2026-09-20 on Windows
# with a stage-2 built there, `main.ll`, `scaly.ll`, `scalyls.ll` and
# `scalyls_main.ll` came out BYTE-IDENTICAL to the committed files, and
# `scalyc.ll` differed only in the functions that had changed. The emission is
# a property of the TARGET and the LLVM version, not of who ran it.
SEED_TARGET=${SEED_TARGET:-arm64-apple-darwin}
# ★--portable-simd (2026-09-29): a SIMD operation with a target form (NEON's
# tbl and addp, ROADMAP-simd.md phase 6) would otherwise take it for the named
# arm64 target, and an x86_64 build from this seed would meet AArch64
# intrinsics. The seed carries the target-neutral forms; a program compiled
# for its machine gets the target's.
# The roots are independent, so every step over them runs them side by side;
# `waitall` fails if any of them did.
waitall() { local rc=0 p; for p in "$@"; do wait "$p" || rc=1; done; return $rc; }
# emit_roots <compiler> <name>=<root>...: each root's IR as $OUT/<name>.ll.
emit_roots() {
  local cc=$1 a pids=(); shift
  for a in "$@"; do
    ( ulimit -s 65520; "$cc" -S --no-tests --portable-simd --target "$SEED_TARGET" -o "$OUT/${a%%=*}.ll" "${a#*=}" ) & pids+=($!)
  done
  waitall "${pids[@]}"
}
# llc_objs <name>...: $OUT/<name>.ll -> $OUT/<name>.o.
# ★-O0 (2026-09-26): these objects only build the CHECK binaries -- the seed
# compiler that must link clean, run hello and the AOT corpus and reproduce
# itself, and the scalyls smoke. Default codegen spent 63 s on one core for
# scalyc.ll alone, -O0 3 s; the -O0 compiler re-emits in 17 s instead of 8 and
# reproduces the seed byte for byte. The compiler the suites run is built by
# tools/build-from-seed.sh (opt -O2 + llc), untouched.
llc_objs() {
  local f pids=()
  for f in "$@"; do
    # -relocation-model=pic: x86-64 Linux PIE rejects llc's default R_X86_64_32
    # abs relocations; PIC is the Mach-O default, so this is a no-op on macOS.
    "$LLC" -O0 -relocation-model=pic -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" & pids+=($!)
  done
  waitall "${pids[@]}"
}
C=packages/scalyc/0.1.0 L=packages/scalyls/0.1.0
echo "seed: emitting .ll with $CC --no-tests --portable-simd --target $SEED_TARGET"
emit_roots "$CC" main=$C/main.scaly scalyc=$C/scalyc.scaly scaly=packages/scaly/0.1.0/scaly.scaly || fail "emission"

if [ "$SCALY_COFF" = 1 ]; then
# ★The Windows box: the seed text is Mach-O-targeted and carries no COMDATs,
# so it cannot be compiled per object for COFF; CI's rung 12 route instead —
# clang -flto=full per root, lld-link with -O2 on the link, dead declares
# stripped first (tools/win-lto.sh has every reason). The same link proves
# zero undefined symbols: lld-link has no dynamic_lookup to forget.
echo "seed: clang -flto=full + lld-link (tools/win-lto.sh, no dynamic_lookup)"
if ! tools/win-lto.sh --llvm "$OUT/scalyc_seed$SCALY_EXE" "$OUT/main.ll" "$OUT/scalyc.ll" "$OUT/scaly.ll" > "$OUT/link.log" 2>&1; then
  tail -40 "$OUT/link.log"
  fail "link (undefined symbols)"
fi
else
echo "seed: llc -> obj (LLVM 20) + link ($CLANG, no dynamic_lookup)"
llc_objs main scalyc scaly || fail "llc"
# NO -Wl,-undefined,dynamic_lookup — a clean link proves zero undefined.
# (The final link is plain object linking, so any clang/cc works. What is NOT
# free is -lLLVM-20 below: libLLVM prints the IR, so its major must match the
# one the seed was minted with. llc's own version is looser — see llvm-env.sh.)
# fcontext.o supplies the fiber context-switch primitives that scaly.ll's
# Fiber procedures reference (vendored assembly, host-arch-selected);
# eio.o the evented-I/O backend (kqueue/epoll, cpp-selected C shim); ctime.o
# the civil-time shim, which the compiler never calls but must EXPORT so a
# dazzle stylesheet under --jit can resolve the DSSSL time primitives.
CLANG="$CLANG" tools/fcontext.sh "$OUT/fcontext.o" || fail "fcontext assembly"
CLANG="$CLANG" tools/eio.sh "$OUT/eio.o" || fail "eio shim compile"
CLANG="$CLANG" tools/ctime.sh "$OUT/ctime.o" || fail "ctime shim compile"
CLANG="$CLANG" tools/panic.sh "$OUT/panic.o" || fail "panic shim compile"
if ! "$CLANG" "${LINKARGS[@]}" "$OUT/main.o" "$OUT/scalyc.o" "$OUT/scaly.o" "$OUT/fcontext.o" "$OUT/eio.o" "$OUT/ctime.o" "$OUT/panic.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm -o "$OUT/scalyc_seed" 2> "$OUT/link.log"; then
  grep -v 'reexported library' "$OUT/link.log" || true
  fail "link (undefined symbols)"
fi
fi
echo "seed: linked clean -> $OUT/scalyc_seed"

# The seed compiler's four jobs are independent of each other and run side by
# side: hello + the AOT corpus, and the re-emission of the three compiler roots.
# The scalyls roots (below) are emitted in the same round.
echo "seed: hello.scaly, AOT corpus, fixed-point re-emission, scalyls roots"
( "$OUT/scalyc_seed" -o "$OUT/hello" tests/aot/hello.scaly || { echo "compile hello"; exit 1; }
  out=$("$OUT/hello")
  [ "$out" = "Hello, World!" ] || { echo "hello output: '$out'"; exit 1; }
  # `| tail -1` keeps the summary line only — so check the SCRIPT's status, not
  # the pipeline's (which is tail's, always 0).
  tools/aot_corpus.sh "$OUT/scalyc_seed" seed | tail -1
  [ "${PIPESTATUS[0]}" = "0" ] || { echo "AOT corpus"; exit 1; }
) > "$OUT/aot.log" 2>&1 & aot_pid=$!
emit_roots "$OUT/scalyc_seed" r_main=$C/main.scaly r_scalyc=$C/scalyc.scaly r_scaly=packages/scaly/0.1.0/scaly.scaly & reemit_pid=$!
emit_roots "$OUT/scalyc_seed" scalyls_main=$L/main.scaly scalyls=$L/scalyls.scaly json=packages/json/0.1.0/json.scaly & ls_pid=$!
wait $aot_pid; aot_rc=$?
cat "$OUT/aot.log"; rm -f "$OUT/aot.log"
[ $aot_rc = 0 ] || fail "hello / AOT corpus"
wait $reemit_pid || fail "re-emission"
wait $ls_pid || fail "scalyls emission"
echo "seed: fixed-point self-reproduction"
for f in main scalyc scaly; do
  cmp -s "$OUT/r_$f.ll" "$OUT/$f.ll" || fail "fixed point: $f.ll differs"
done

# ---------------------------------------------------------------------------
# scalyls language server seed (separate program; NOT part of the compiler
# fixed point above).
#
# scalyls depends on the scalyc compiler PACKAGE and is now built SELF-HOSTED
# (by the seed compiler), per-root like the compiler itself. The former
# cross-package type-layout gap — a dependency type referenced as a member-
# access base or choose scrutinee (ProgramSyntax/FileSyntax structs,
# DeclarationSyntax union) was registered as a Concept but never laid out, so
# map_type fell back to the s114 { ptr } placeholder and document_symbols#
# returned [] — is closed by Planner.scaly's ensure_member_layout# /
# ensure_union_layout# (they plan the dependency type's LAYOUT on demand, no
# initializers/methods, so the body still resolves cross-unit at link time).
# documentSymbol/definition/references/completion/signatureHelp/semanticTokens/
# folding/hover (incl. semantic hover on a variable USE) / diagnostics are at
# parity with the C++-stage-0 build — the cross-package by-value LAYOUT fix
# (Planner.scaly ensure_layout_by_name# recursion + the call-boundary +
# instantiate_generic# ensure-hooks) lets the worker construct + run the full
# planner (Modeler.build_program# -> plan_program over a cross-package Planner)
# without the s114 { ptr }-placeholder ABI mismatch that crashed it.
#
# KNOWN GAP (narrow, graceful): a mid-session SAME-LENGTH on-disk edit of a
# sibling file is not picked up by the chain-segment inlayHint content-hash
# cache (the re-read file String's hash() returns its pre-edit value in the
# long-running worker — a probe-sensitive heap Heisenbug). Editors send a
# didChange (the in-memory store path, which works) rather than editing on disk
# silently, and length-changing edits invalidate fine, so this is rarely hit.
#
# Build: reuse the compiler seed's scalyc.o + scaly.o (the same packages
# scalyls depends on, already emitted above) and add scalyls' OWN main + root.
# NO fixed-point requirement (scalyls does not self-compile): link it clean
# (zero undefined) and smoke-test documentSymbol against a known input.
# (The two scalyls roots were emitted by $OUT/scalyc_seed above, beside the
# fixed-point re-emission.)
if [ "$SCALY_COFF" = 1 ]; then
# ★The Windows box: the scalyls ROOTS are emitted above (for the seed's own
# target — they are seed files and reproduce here like the other three), but
# the server is neither linked nor smoke-tested, and it is said by name: the
# LTO route over these four roots stops at popen, pclose, fork, waitpid and
# kill — scalyls' worker process model, which the Windows substrate has no
# counterpart for (measured 2026-09-20, tools/build-from-seed.sh has the same
# note). Porting the worker is a scalyls port, tests/win32/WINDOWS-BOX.md §4a.
echo "seed: SKIP scalyls link + LSP smoke on the Windows box (worker.scaly: fork/popen/waitpid/kill)"
SCALYLS_VERDICT="scalyls roots emitted, link + smoke SKIPPED (Windows box)"
else
llc_objs scalyls_main scalyls json || fail "llc scalyls"
if ! "$CLANG" "${LINKARGS[@]}" "$OUT/scalyls_main.o" "$OUT/scalyls.o" "$OUT/json.o" "$OUT/scalyc.o" "$OUT/scaly.o" "$OUT/fcontext.o" "$OUT/eio.o" "$OUT/ctime.o" "$OUT/panic.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm -o "$OUT/scalyls" 2> "$OUT/scalyls_link.log"; then
  grep -v 'reexported library' "$OUT/scalyls_link.log" || true
  fail "scalyls link (undefined symbols)"
fi
echo "seed: scalyls linked clean -> $OUT/scalyls"
echo "seed: scalyls LSP smoke (initialize + documentSymbol)"
python3 - "$OUT/scalyls" <<'PY' || fail "scalyls smoke"
import sys, json, subprocess, tempfile, os
def frame(o):
    b = json.dumps(o).encode()
    return ("Content-Length: %d\r\n\r\n" % len(b)).encode() + b
src = "function answer() returns int\n{\n    return 42\n}\n"
fd, path = tempfile.mkstemp(suffix=".scaly"); os.write(fd, src.encode()); os.close(fd)
uri = "file://" + path
inp  = frame({"jsonrpc":"2.0","id":1,"method":"initialize","params":{"rootUri":"file://"+os.path.dirname(path)}})
inp += frame({"jsonrpc":"2.0","method":"initialized","params":{}})
inp += frame({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":uri,"languageId":"scaly","version":1,"text":src}}})
inp += frame({"jsonrpc":"2.0","id":2,"method":"textDocument/documentSymbol","params":{"textDocument":{"uri":uri}}})
inp += frame({"jsonrpc":"2.0","id":3,"method":"shutdown"})
inp += frame({"jsonrpc":"2.0","method":"exit"})
out = subprocess.run([sys.argv[1]], input=inp, stdout=subprocess.PIPE, timeout=30).stdout
os.unlink(path)
got_init = got_sym = False
i = 0
while True:
    j = out.find(b"\r\n\r\n", i)
    if j < 0: break
    n = int(out[i:j].decode().split(":")[1].strip())
    try:
        o = json.loads(out[j+4:j+4+n])
    except Exception:
        o = {}
    if o.get("id") == 1 and "capabilities" in o.get("result", {}): got_init = True
    if o.get("id") == 2:
        r = o.get("result") or []
        if len(r) == 1 and r[0].get("name") == "answer": got_sym = True
    i = j+4+n
if not got_init: sys.exit("bad initialize response")
if not got_sym:  sys.exit("documentSymbol did not return the expected outline")
PY
SCALYLS_VERDICT="scalyls language server links + serves"
fi

echo "SEED: OK — compiler links clean, runs hello + AOT, reproduces itself"
echo "          byte-identical; $SCALYLS_VERDICT"
