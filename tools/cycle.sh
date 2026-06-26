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

# ROOT: build from the committed seed (C++-free); fall back to C++ stage-0.
# Cached at /tmp/scalyc_seed_root — rebuilt only when older than the seed.
if [ -f seed/scalyc.ll ] && { [ -x /tmp/scalyc_seed_root ] && [ /tmp/scalyc_seed_root -nt seed/scalyc.ll ] \
     || tools/build-from-seed.sh /tmp/scalyc_seed_root >/dev/null 2>&1; }; then
  ROOT=/tmp/scalyc_seed_root; ROOT_SELFHOSTED=1
else
  ROOT=./scalyc/build/scalyc; ROOT_SELFHOSTED=0
fi

# A self-hosted ROOT needs the archive two-step to build stage1 (it emits only
# main + external compiler-package refs); C++ stage-0 emits the whole package
# from main.scaly alone.
if [ "$ROOT_SELFHOSTED" = "1" ]; then
  bash -c "ulimit -s 65520; '$ROOT' -c -o /tmp/sc0n.o packages/scalyc/0.1.0/scalyc.scaly"
  rm -f /tmp/libscalyc0n.a; ar rcs /tmp/libscalyc0n.a /tmp/sc0n.o
  bash -c "ulimit -s 65520; '$ROOT' -o /tmp/scalyc_stage1_new packages/scalyc/0.1.0/main.scaly /tmp/libscalyc0n.a $LINK" 2>&1 | grep -v "ld: warning" || true
else
  "$ROOT" -o /tmp/scalyc_stage1_new packages/scalyc/0.1.0/main.scaly $LINK 2>&1 | grep -v warning | grep -v "^$" || true
fi
bash -c 'ulimit -s 65520; /tmp/scalyc_stage1_new -c -o /tmp/sc1n.o packages/scalyc/0.1.0/scalyc.scaly'
rm -f /tmp/libscalyc1n.a; ar rcs /tmp/libscalyc1n.a /tmp/sc1n.o
bash -c "ulimit -s 65520; /tmp/scalyc_stage1_new -o /tmp/scalyc_stage2_new packages/scalyc/0.1.0/main.scaly /tmp/libscalyc1n.a $LINK" 2>&1 | grep -v "ld: warning" || true
/tmp/scalyc_stage1_new -S --no-prelude -o /tmp/sl_s1.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
/tmp/scalyc_stage2_new -S --no-prelude -o /tmp/sl_s2.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
cmp -s /tmp/sl_s1.ll /tmp/sl_s2.ll && echo "RESULT: IDENTICAL" || echo "RESULT: DIVERGES"
