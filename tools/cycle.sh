#!/bin/bash
# Bootstrap cycle: rebuild stage-1 (via the seed ROOT) and stage-2 (via stage-1),
# then compare both stages' emission of the scaly package. IDENTICAL means
# the self-hosted lineage emits consistently.
# Binaries land in /tmp/scalyc_stage1_new and /tmp/scalyc_stage2_new —
# promote to /tmp/scalyc_stage{1,2} only after the full bar is green.
set -e
cd "$(dirname "$0")/.."
source tools/llvm-env.sh >/dev/null
LINK="-L$LLVM_LIBDIR -l$LLVM_LIBNAME"

# ROOT: build from the committed seed. The C++ stage-0 fallback is retired
# (sources frozen under retired/scalyc0/) — no seed means no bootstrap.
# Cached at /tmp/scalyc_seed_root — rebuilt only when older than the seed.
if [ -f seed/scalyc.ll ] && { [ -x /tmp/scalyc_seed_root ] && [ /tmp/scalyc_seed_root -nt seed/scalyc.ll ] \
     || SCALYC_SEED_NO_SCALYLS=1 tools/build-from-seed.sh /tmp/scalyc_seed_root >/dev/null 2>&1; }; then
  ROOT=/tmp/scalyc_seed_root
else
  echo "cycle: FAIL — no usable seed (seed/scalyc.ll missing or build-from-seed failed)"
  exit 1
fi

# Every stage links against ONE fixed path, /tmp/libscaly.a (cli.run appends
# it), so each stage gets its runtime rebuilt by the compiler that will link
# against it — the same discipline bootstrap.sh follows. Ordinarily this only
# guards against a stale archive shadowing generic instantiations; when a
# change alters the RUNTIME CALLING CONVENTION it is the difference between a
# working cycle and a segfault, because the two lineages then genuinely
# disagree. The caller-page argument became a {page, parent} frame on
# 2026-08-02, and an archive from the other side of that change hands a page
# where a frame is expected — nothing in the mangled names records it.
#
# The ROOT's own archive is rebuilt here rather than left to
# build-from-seed.sh: the ROOT is mtime-cached above, so on a cache hit that
# script never runs and /tmp/libscaly.a keeps whatever the last build left.
rebuild_runtime() {
  bash -c "ulimit -s 65520; '$1' -c --no-prelude --no-tests -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly"
  tools/fcontext.sh /tmp/fcontext.o
  tools/eio.sh /tmp/eio.o
  tools/ctime.sh /tmp/ctime.o
  rm -f /tmp/libscaly.a; ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o /tmp/ctime.o
}

# The self-hosted ROOT needs the archive two-step to build stage1 (it emits
# only main + external compiler-package refs).
rebuild_runtime "$ROOT"
bash -c "ulimit -s 65520; '$ROOT' -c -o /tmp/sc0n.o packages/scalyc/0.1.0/scalyc.scaly"
rm -f /tmp/libscalyc0n.a; ar rcs /tmp/libscalyc0n.a /tmp/sc0n.o
bash -c "ulimit -s 65520; '$ROOT' -o /tmp/scalyc_stage1_new packages/scalyc/0.1.0/main.scaly /tmp/libscalyc0n.a $LINK" 2>&1 | grep -v "ld: warning" || true
rebuild_runtime /tmp/scalyc_stage1_new
bash -c 'ulimit -s 65520; /tmp/scalyc_stage1_new -c -o /tmp/sc1n.o packages/scalyc/0.1.0/scalyc.scaly'
rm -f /tmp/libscalyc1n.a; ar rcs /tmp/libscalyc1n.a /tmp/sc1n.o
bash -c "ulimit -s 65520; /tmp/scalyc_stage1_new -o /tmp/scalyc_stage2_new packages/scalyc/0.1.0/main.scaly /tmp/libscalyc1n.a $LINK" 2>&1 | grep -v "ld: warning" || true
/tmp/scalyc_stage1_new -S --no-prelude -o /tmp/sl_s1.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
/tmp/scalyc_stage2_new -S --no-prelude -o /tmp/sl_s2.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
cmp -s /tmp/sl_s1.ll /tmp/sl_s2.ll && echo "RESULT: IDENTICAL" || echo "RESULT: DIVERGES"
