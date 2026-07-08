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
# `tools/seed.sh` (no args) -> bootstraps stage-2 -> emits + verifies the seed
# for THIS host's target triple (the compiler is host-only; one seed per box).
#   The "fabulous four" LP64 targets and how to install deps:
#     arm64-apple-darwin   brew install llvm@18 cmake openjade
#     x86_64-apple-darwin  (Intel Mac)  same brew formulae
#     x86_64-linux-gnu     apt install llvm-18-dev clang-18 cmake openjade \
#                                      zlib1g-dev libzstd-dev
#     aarch64-linux-gnu    same apt packages (arm64 Ubuntu)
# Override LLVM detection with LLVM18=/path; see tools/llvm-env.sh.
#
# .ll artifacts are NOT checked into the repo: ~12MB, target DataLayout + LLVM
# version baked in (union payload sizes computed at emission). One .ll set per
# target triple, regenerated and fixed-point-verified per release.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "SEED: FAIL — LLVM 18 not found"; exit 1; }

CC=${1:-/tmp/scalyc_stage2}
OUT=${2:-dist/seed}
CLANG=${CLANG:-clang}

# On Linux, stock GNU ld (BFD) fails to link libLLVM-18 ("failed to set dynamic
# section sizes: bad value"); lld handles it. Mirror tools/build-from-seed.sh so
# seed.sh's OWN link steps (scalyc_seed, scalyls) work on Linux too. macOS ld64
# links fine, so LINKARGS stays empty there.
LINKARGS=()
if [ "$(uname -s)" = "Linux" ]; then
  for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld ld.lld-18; do
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

echo "seed: emitting .ll with $CC --no-tests"
( ulimit -s 65520
  for f in main scalyc; do
    "$CC" -S --no-tests -o "$OUT/$f.ll" packages/scalyc/0.1.0/$f.scaly || exit 1
  done
  "$CC" -S --no-tests -o "$OUT/scaly.ll" packages/scaly/0.1.0/scaly.scaly || exit 1
) || fail "emission"

echo "seed: llc -> obj (LLVM 18) + link ($CLANG, no dynamic_lookup)"
for f in main scalyc scaly; do
  # -relocation-model=pic: x86-64 Linux PIE rejects llc's default R_X86_64_32
  # abs relocations; PIC is the Mach-O default, so this is a no-op on macOS.
  "$LLC" -relocation-model=pic -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" || fail "llc $f.ll"
done
# NO -Wl,-undefined,dynamic_lookup — a clean link proves zero undefined.
# (LLVM-18 llc is required: some system clangs reject the seed's
# mul/ptrtoint-getelementptr constexprs; llc-18 accepts them. The final link
# is plain object linking, so any clang/cc works.)
# fcontext.o supplies the fiber context-switch primitives that scaly.ll's
# Fiber procedures reference (vendored assembly, host-arch-selected);
# eio.o the evented-I/O backend (kqueue/epoll, cpp-selected C shim).
CLANG="$CLANG" tools/fcontext.sh "$OUT/fcontext.o" || fail "fcontext assembly"
CLANG="$CLANG" tools/eio.sh "$OUT/eio.o" || fail "eio shim compile"
if ! "$CLANG" "${LINKARGS[@]}" "$OUT/main.o" "$OUT/scalyc.o" "$OUT/scaly.o" "$OUT/fcontext.o" "$OUT/eio.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT/scalyc_seed" 2> "$OUT/link.log"; then
  grep -v 'reexported library' "$OUT/link.log" || true
  fail "link (undefined symbols)"
fi
echo "seed: linked clean -> $OUT/scalyc_seed"

echo "seed: hello.scaly"
"$OUT/scalyc_seed" -o "$OUT/hello" tests/aot/hello.scaly || fail "compile hello"
out=$("$OUT/hello")
[ "$out" = "Hello, World!" ] || fail "hello output: '$out'"

echo "seed: AOT corpus"
tools/aot_corpus.sh "$OUT/scalyc_seed" seed | tail -1

echo "seed: fixed-point self-reproduction"
( ulimit -s 65520
  for f in main scalyc; do
    "$OUT/scalyc_seed" -S --no-tests -o "$OUT/r_$f.ll" packages/scalyc/0.1.0/$f.scaly || exit 1
  done
  "$OUT/scalyc_seed" -S --no-tests -o "$OUT/r_scaly.ll" packages/scaly/0.1.0/scaly.scaly || exit 1
) || fail "re-emission"
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
echo "seed: emitting scalyls roots with $OUT/scalyc_seed (self-hosted)"
( ulimit -s 65520
  "$OUT/scalyc_seed" -S --no-tests -o "$OUT/scalyls_main.ll" packages/scalyls/0.1.0/main.scaly || exit 1
  "$OUT/scalyc_seed" -S --no-tests -o "$OUT/scalyls.ll"      packages/scalyls/0.1.0/scalyls.scaly || exit 1
) || fail "scalyls emission"
for f in scalyls_main scalyls; do
  "$LLC" -relocation-model=pic -filetype=obj "$OUT/$f.ll" -o "$OUT/$f.o" || fail "llc $f.ll"
done
if ! "$CLANG" "${LINKARGS[@]}" "$OUT/scalyls_main.o" "$OUT/scalyls.o" "$OUT/scalyc.o" "$OUT/scaly.o" "$OUT/fcontext.o" "$OUT/eio.o" \
     -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$OUT/scalyls" 2> "$OUT/scalyls_link.log"; then
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

echo "SEED: OK — compiler links clean, runs hello + AOT, reproduces itself"
echo "          byte-identical; scalyls language server links + serves"
