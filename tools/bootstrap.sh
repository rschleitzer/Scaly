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
#   LLVM 18 (+dev), clang/clang++, ar — plus cmake + openjade ONLY for the
#   C++ stage-0 fallback (no committed seed).
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "bootstrap: FAIL — LLVM 18 not found"; exit 1; }

LINK="-L$LLVM_LIBDIR -l$LLVM_LIBNAME"

# Bootstrap ROOT: the committed seed. The C++ stage-0 fallback is retired
# (sources frozen under retired/scalyc0/) — no seed means no bootstrap.
if [ -f seed/scalyc.ll ] && SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh /tmp/scalyc_seed_root >/dev/null 2>&1; then
  echo "bootstrap: ROOT = seed-built compiler -> /tmp/scalyc_seed_root"
  ROOT=/tmp/scalyc_seed_root
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
  "$ROOT" -o /tmp/scalyc_stage1 packages/scalyc/0.1.0/main.scaly /tmp/libscalyc0.a $LINK
) 2>&1 | grep -v 'ld: warning' || true

# Rebuild the scaly-package runtime archive with stage1 so stage2's -o link
# (cli.run appends /tmp/libscaly.a) picks up a fix-consistent runtime. A STALE
# /tmp/libscaly.a — built by an older compiler — silently shadows the correct
# generic instantiations (it links before libscalyc1.a, so its linkonce_odr
# copies win), reintroducing fixed bugs in stage2 while stage1 stays fine.
# --no-tests drops test/test_* (dead from cli.main, reference uninstantiated
# generics that would otherwise be undefined at link).
echo "bootstrap: stage1 -> /tmp/libscaly.a (runtime archive)"
( ulimit -s 65520; /tmp/scalyc_stage1 -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly )
tools/fcontext.sh /tmp/fcontext.o
rm -f /tmp/libscaly.a; ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o

echo "bootstrap: stage1 -> stage2"
( ulimit -s 65520
  /tmp/scalyc_stage1 -c -o /tmp/sc1.o packages/scalyc/0.1.0/scalyc.scaly
  rm -f /tmp/libscalyc1.a; ar rcs /tmp/libscalyc1.a /tmp/sc1.o
  /tmp/scalyc_stage1 -o /tmp/scalyc_stage2 packages/scalyc/0.1.0/main.scaly /tmp/libscalyc1.a $LINK
) 2>&1 | grep -v 'ld: warning' || true

[ -x /tmp/scalyc_stage2 ] || { echo "bootstrap: FAIL — stage2 not produced"; exit 1; }
echo "bootstrap: OK -> /tmp/scalyc_stage2"
