#!/bin/bash
# Stage-5 milestone 5.0: pure-Scaly matmul throughput at demo-relevant
# GEMM sizes — sequential baseline vs the stage-4 adaptive-parallel
# path (cold run + promoted run). The numbers feed the
# Accelerate-or-pure decision for the Thomas-Mann demo (ROADMAP
# stage 5: "Measure, don't assume").
#
# Usage: tests/tensor/bench/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
set -e
cd "$(dirname "$0")/../../.."

STAGE=${1:-/tmp/scalyc_stage2}
BIN=/tmp/tensor_matmul_bench
rm -f "$BIN"
"$STAGE" -o "$BIN" tests/tensor/bench/matmul.scaly
"$BIN"

# 5.1 op kernels at -O2 (the level the demo uses).
OPS=/tmp/tensor_ops_bench
rm -f "$OPS"
"$STAGE" -O2 -o "$OPS" tests/tensor/bench/ops.scaly
"$OPS"
