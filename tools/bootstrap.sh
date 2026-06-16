#!/bin/bash
# Clean bootstrap from a fresh checkout to a working self-hosted stage-2.
#   tools/bootstrap.sh            -> /tmp/scalyc_stage2 (+ stage1)
#
# Steps: build C++ stage-0 (build.sh: openjade codegen + cmake), then
#   stage-0 -> stage1 (compiles main.scaly),
#   stage1  -> stage2 (compiles scalyc.scaly into libscalyc, relinks main).
# stage2 is the canonical self-hosted compiler the seed is emitted from.
#
# Requires (install per platform — see tools/seed.sh header):
#   LLVM 18 (+dev), clang/clang++, cmake, openjade, ar.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh
[ "$llvm_env_ok" = "1" ] || { echo "bootstrap: FAIL — LLVM 18 not found"; exit 1; }

echo "bootstrap: building C++ stage-0 (build.sh)"
./build.sh >/dev/null

LINK="-L$LLVM_LIBDIR -l$LLVM_LIBNAME"
echo "bootstrap: stage-0 -> stage1"
./scalyc/build/scalyc -o /tmp/scalyc_stage1 packages/scalyc/0.1.0/main.scaly $LINK 2>&1 | grep -v 'warning' || true

# Rebuild the scaly-package runtime archive with stage1 so stage2's -o link
# (cli.run appends /tmp/libscaly.a) picks up a fix-consistent runtime. A STALE
# /tmp/libscaly.a — built by an older compiler — silently shadows the correct
# generic instantiations (it links before libscalyc1.a, so its linkonce_odr
# copies win), reintroducing fixed bugs in stage2 while stage1 stays fine.
# --no-tests drops test/test_* (dead from cli.main, reference uninstantiated
# generics that would otherwise be undefined at link).
echo "bootstrap: stage1 -> /tmp/libscaly.a (runtime archive)"
( ulimit -s 65520; /tmp/scalyc_stage1 -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly )
ar rcs /tmp/libscaly.a /tmp/libscaly.o

echo "bootstrap: stage1 -> stage2"
( ulimit -s 65520
  /tmp/scalyc_stage1 -c -o /tmp/sc1.o packages/scalyc/0.1.0/scalyc.scaly
  ar rcs /tmp/libscalyc1.a /tmp/sc1.o
  /tmp/scalyc_stage1 -o /tmp/scalyc_stage2 packages/scalyc/0.1.0/main.scaly /tmp/libscalyc1.a $LINK
) 2>&1 | grep -v 'ld: warning' || true

[ -x /tmp/scalyc_stage2 ] || { echo "bootstrap: FAIL — stage2 not produced"; exit 1; }
echo "bootstrap: OK -> /tmp/scalyc_stage2"
