#!/bin/bash
# Bootstrap cycle: rebuild stage-1 (via stage-0) and stage-2 (via stage-1),
# then compare both stages' emission of the scaly package. IDENTICAL means
# the self-hosted lineage emits consistently.
# Binaries land in /tmp/scalyc_stage1_new and /tmp/scalyc_stage2_new —
# promote to /tmp/scalyc_stage{1,2} only after the full bar is green.
set -e
cd "$(dirname "$0")/.."
./scalyc/build/scalyc -o /tmp/scalyc_stage1_new packages/scalyc/0.1.0/main.scaly -L/opt/homebrew/opt/llvm@18/lib -lLLVM-18 2>&1 | grep -v warning | grep -v "^$" || true
bash -c 'ulimit -s 65520; /tmp/scalyc_stage1_new -c -o /tmp/sc1n.o packages/scalyc/0.1.0/scalyc.scaly'
rm -f /tmp/libscalyc1n.a; ar rcs /tmp/libscalyc1n.a /tmp/sc1n.o
bash -c 'ulimit -s 65520; /tmp/scalyc_stage1_new -o /tmp/scalyc_stage2_new packages/scalyc/0.1.0/main.scaly /tmp/libscalyc1n.a -L/opt/homebrew/opt/llvm@18/lib -lLLVM-18' 2>&1 | grep -v "ld: warning" || true
/tmp/scalyc_stage1_new -S --no-prelude -o /tmp/sl_s1.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
/tmp/scalyc_stage2_new -S --no-prelude -o /tmp/sl_s2.ll packages/scaly/0.1.0/scaly.scaly 2>/dev/null
cmp -s /tmp/sl_s1.ll /tmp/sl_s2.ll && echo "RESULT: IDENTICAL" || echo "RESULT: DIVERGES"
