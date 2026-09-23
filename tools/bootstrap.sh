#!/bin/bash
# Clean bootstrap from a fresh checkout to a working self-hosted stage-2.
#   tools/bootstrap.sh            -> /tmp/scalyc_stage2 (+ stage1)
#
# Steps: build the bootstrap ROOT, then
#   ROOT   -> stage1 (compiles the scalyc package),
#   stage1 -> stage2 (compiles scalyc.scaly into libscalyc, relinks main).
# stage2 is the canonical self-hosted compiler the seed is emitted from.
#
# ROOT is built from the committed .ll seed (tools/build-from-seed.sh: llc +
# clang, NO C++ stage-0) — the seed compiler handles both the legacy #/$ page
# sigils AND no-sigil page inference, so it survives the de-sigil migration that
# frozen C++ stage-0 cannot parse. Falls back to building C++ stage-0 only when
# no committed seed is present (valid while the source still uses #).
#
# Requires (install per platform — see tools/seed.sh header):
#   LLVM 20 (+dev), clang/clang++, ar — plus cmake + openjade ONLY for the
#   C++ stage-0 fallback (no committed seed).
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "bootstrap: FAIL — LLVM 20 not found"; exit 1; }

# -lm: the stdlib's tensor tape kernels call tanhf/expf/sqrtf/logf/powf, which
# live in a separate libm on Linux (macOS has them in libSystem). Forwarded to
# the compiler's own link step, which emits -l flags last — after the archives
# that reference them, as a left-to-right ELF linker requires.
LINK="-L$LLVM_LIBDIR -l$LLVM_LIBNAME -lm"

# Link a stage: link_stage <compiler> <out> <package-archive>. POSIX lets the
# compiler drive the link, exactly as before. ★The Windows box cannot: the
# driver's link line has no stack reserve, and a stage binary must compile the
# compiler — Windows gives a thread 1 MB where `ulimit -s 65520` gives 64, and
# the planner is bound in nesting depth. So there the main object is emitted
# with -c and linked by hand through tools/win-link.sh, which is CI's rung 9
# line (`-Xlinker -stack:67108864,1048576`, -lLLVM-C, -lws2_32, the archive).
# `ar` is llvm-ar there (tools/win/ar, on PATH from tools/win-env.sh); a
# `.a` archive is read by its magic, not its suffix.
link_stage() {
  if [ "$SCALY_COFF" = 1 ]; then
    "$1" -c -o "$2_main.o" packages/scalyc/0.1.0/main.scaly
    tools/win-link.sh --llvm --runtime "$2$SCALY_EXE" "$2_main.o" "$3"
  else
    "$1" -o "$2" packages/scalyc/0.1.0/main.scaly "$3" $LINK
  fi
}

# Bootstrap ROOT: the committed seed. The C++ stage-0 fallback is retired
# (sources frozen under retired/scalyc0/) — no seed means no bootstrap.
# The ROOT is a function of the seed and of the script that builds it, so it is
# reused while both hash as they did when it was built (41 s of a 60 s
# bootstrap). A reused ROOT still gets a runtime archive of its OWN before it
# links stage1 — the archive in /tmp may come from any other compiler — the way
# tools/cycle.sh does it. The Windows box always rebuilds.
root_key() { cat seed/main.ll seed/scalyc.ll seed/scaly.ll tools/build-from-seed.sh 2>/dev/null | shasum -a 256 | cut -c1-64; }
if [ "$SCALY_COFF" != 1 ] && [ -f seed/scalyc.ll ] && [ -x /tmp/scalyc_seed_root ] \
     && [ "$(cat /tmp/scalyc_seed_root.key 2>/dev/null)" = "$(root_key)" ]; then
  echo "bootstrap: ROOT = seed-built compiler -> /tmp/scalyc_seed_root (reused, seed unchanged)"
  ROOT=/tmp/scalyc_seed_root
  ( ulimit -s 65520; "$ROOT" -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly )
  tools/fcontext.sh /tmp/fcontext.o
  tools/eio.sh /tmp/eio.o
  tools/ctime.sh /tmp/ctime.o
  tools/panic.sh /tmp/panic.o
  rm -f /tmp/libscaly.a; ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o /tmp/ctime.o /tmp/panic.o
elif [ -f seed/scalyc.ll ] && SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh /tmp/scalyc_seed_root >/dev/null 2>&1; then
  echo "bootstrap: ROOT = seed-built compiler -> /tmp/scalyc_seed_root"
  ROOT=/tmp/scalyc_seed_root
  root_key > /tmp/scalyc_seed_root.key
else
  echo "bootstrap: FAIL — no usable seed (seed/scalyc.ll missing or build-from-seed failed)"
  exit 1
fi

echo "bootstrap: ROOT -> stage1"
# The self-hosted ROOT compiling main.scaly emits only main + external refs to
# the compiler package; the package must be compiled to an archive and linked
# (the same two-step as stage1 -> stage2).
( ulimit -s 65520
  "$ROOT" -c -o /tmp/sc0.o packages/scalyc/0.1.0/scalyc.scaly
  rm -f /tmp/libscalyc0.a; ar rcs /tmp/libscalyc0.a /tmp/sc0.o
  link_stage "$ROOT" /tmp/scalyc_stage1 /tmp/libscalyc0.a
) 2>&1 | grep -v 'ld: warning' || true

# Rebuild the scaly-package runtime archive with stage1 so stage2's -o link
# (cli.run appends /tmp/libscaly.a) picks up a fix-consistent runtime. A STALE
# /tmp/libscaly.a — built by an older compiler — silently shadows the correct
# generic instantiations (it links before libscalyc1.a, so its linkonce_odr
# copies win), reintroducing fixed bugs in stage2 while stage1 stays fine.
# --no-tests drops test/test_* (dead from cli.main, reference uninstantiated
# generics that would otherwise be undefined at link).
if [ "$SCALY_COFF" = 1 ]; then
  # The same discipline, the Windows archive: libscaly.lib where cli.scaly's
  # link driver looks, built as CI's rung 3 builds it (tools/win-archive.sh).
  echo "bootstrap: stage1 -> /tmp/libscaly.lib (runtime archive)"
  tools/win-archive.sh /tmp/scalyc_stage1 > /dev/null
else
echo "bootstrap: stage1 -> /tmp/libscaly.a (runtime archive) and the scalyc package, side by side"
# The package object does not need the archive, only stage2's link does.
( ulimit -s 65520; /tmp/scalyc_stage1 -c -o /tmp/sc1.o packages/scalyc/0.1.0/scalyc.scaly ) & sc1_pid=$!
( ulimit -s 65520; /tmp/scalyc_stage1 -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly )
tools/fcontext.sh /tmp/fcontext.o
tools/eio.sh /tmp/eio.o
tools/ctime.sh /tmp/ctime.o
tools/panic.sh /tmp/panic.o
rm -f /tmp/libscaly.a; ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o /tmp/ctime.o /tmp/panic.o
wait $sc1_pid || { echo "bootstrap: FAIL — stage1 could not compile the scalyc package"; exit 1; }
fi

echo "bootstrap: stage1 -> stage2"
# A failed link must not leave the previous stage2 behind to be tested instead.
rm -f /tmp/scalyc_stage2
( ulimit -s 65520
  [ "$SCALY_COFF" = 1 ] && /tmp/scalyc_stage1 -c -o /tmp/sc1.o packages/scalyc/0.1.0/scalyc.scaly
  rm -f /tmp/libscalyc1.a; ar rcs /tmp/libscalyc1.a /tmp/sc1.o
  link_stage /tmp/scalyc_stage1 /tmp/scalyc_stage2 /tmp/libscalyc1.a
) 2>&1 | grep -v 'ld: warning' || true

[ -x /tmp/scalyc_stage2 ] || { echo "bootstrap: FAIL — stage2 not produced"; exit 1; }
echo "bootstrap: OK -> /tmp/scalyc_stage2"
