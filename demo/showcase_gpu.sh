#!/bin/bash
# Showcase the GPU-trained Thomas-Mann model (stage 6.1). The FIRST
# run trains on the GPU (~3 min, or MGPU_BUDGET=seconds) and caches
# the weights as a checkpoint under demo/build/; every run after that
# loads the checkpoint and generates within a couple of seconds.
#
#   demo/showcase_gpu.sh                          corpus prompt
#   demo/showcase_gpu.sh Hans Castorp sagte       your own prompt
#   MGPU_GEN=400 demo/showcase_gpu.sh ...         longer sample
#   MGPU_TEMP=95 demo/showcase_gpu.sh ...         hotter sampling (%)
#   MGPU_BIG=1 demo/showcase_gpu.sh ...           the 15M-param model
#                                                 (own checkpoint)
#   MGPU_BUDGET=600 demo/showcase_gpu.sh          train 10 min instead
#
# Delete demo/build/mann_gpu*.ckpt to retrain. SKIPs off macOS.
set -e
cd "$(dirname "$0")/.."

if [ "$(uname -s)" != "Darwin" ]; then
  echo "SKIP: Metal requires macOS"
  exit 0
fi

CKPT=demo/build/mann_gpu.ckpt
[ -n "$MGPU_BIG" ] && CKPT=demo/build/mann_gpu_big.ckpt
export MGPU_CKPT="$CKPT"
export MGPU_GEN="${MGPU_GEN:-200}"
if [ $# -gt 0 ]; then
  export MGPU_PROMPT="$*"
fi
exec demo/run_bpe_gpu.sh
