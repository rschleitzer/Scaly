#!/bin/bash
# Stage-6 bridge demo: a bigger BPE-tokenized transformer on the FULL
# public-domain Thomas Mann corpus, pure Scaly, zero Python. Measures
# how far the CPU path carries a real-token model in a few-minutes
# budget — the baseline stage 6's GPU lowering must beat.
#
# Pipeline (each step is cached; delete demo/build to redo):
#   1. demo/fetch_corpus.sh   -> demo/build/mann_full.txt  (~5 MB)
#   2. demo/bpe.scaly         -> demo/build/bpe_vocab.txt + bpe_tokens.txt
#   3. demo/mann_bpe.scaly    -> train + generate
#
# Pins the cwd to the repo root (corpus + artifacts load cwd-relative).
# Usage: demo/run_bpe.sh [compiler]   (default scalyc/build/scalyc)
set -e
cd "$(dirname "$0")/.."

SCALYC=${1:-scalyc/build/scalyc}
if [ ! -x "$SCALYC" ]; then
  echo "error: $SCALYC not found - run ./build.sh first" >&2
  exit 1
fi

# -O2 runtime archive: the tape kernels run at the ARCHIVE's opt level.
if [ ! -f /tmp/libscaly.a ] || find packages/scaly -newer /tmp/libscaly.a | grep -q .; then
  echo "building -O2 runtime archive..."
  source tools/llvm-env.sh
  "$SCALYC" -S --no-prelude --no-tests -o /tmp/libscaly.ll packages/scaly/0.1.0/scaly.scaly
  sed 's/^define linkonce_odr /define weak_odr /' /tmp/libscaly.ll > /tmp/libscaly_weak.ll
  opt -O2 /tmp/libscaly_weak.ll -o /tmp/libscaly_opt.bc
  llc -relocation-model=pic -O2 -filetype=obj /tmp/libscaly_opt.bc -o /tmp/libscaly.o
  tools/fcontext.sh /tmp/fcontext.o
  tools/eio.sh /tmp/eio.o
  ar rcs /tmp/libscaly.a /tmp/libscaly.o /tmp/fcontext.o /tmp/eio.o
fi

# 1. corpus
if [ ! -s demo/build/mann_full.txt ]; then
  demo/fetch_corpus.sh
fi

# 2. BPE vocab + token stream
if [ ! -s demo/build/bpe_tokens.txt ] || [ demo/bpe.scaly -nt demo/build/bpe_tokens.txt ]; then
  echo "training BPE tokenizer..."
  BPE=/tmp/scaly_bpe; rm -f "$BPE"
  "$SCALYC" -O2 -o "$BPE" demo/bpe.scaly
  "$BPE"
fi

# 3. train + generate
BIN=/tmp/scaly_mann_bpe
rm -f "$BIN"
"$SCALYC" -O2 -o "$BIN" demo/mann_bpe.scaly
exec "$BIN"
