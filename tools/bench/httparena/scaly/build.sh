#!/bin/bash
# build.sh -- the Scaly entry's image, profile-guided. HttpArena's scripts run
# it from the repository's root instead of a plain `docker build`.
#
# The profile is taken in a container that ALLOWS io_uring: the training
# cannot run inside `docker build`, whose default seccomp profile refuses
# io_uring, and a profile taken on epoll would mark the io_uring paths cold.
#   1. the instrumented arena and its client (target pgo-train)
#   2. the training: the arena under the request shapes the profiles send
#      (train.scaly), its counts merged into pgo/arena.profdata
#   3. the arena optimised with that profile (the default target)
# SCALY_PGO=0 skips 1 and 2: the same build with --release alone.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
IMAGE="${IMAGE_NAME:-httparena-scaly}"
# The dataset /json serves, so the training reaches it: the repository's own
# data/dataset.json, or HTTPARENA_DATA
DATASET="${HTTPARENA_DATA:-$PWD/data}/dataset.json"
CTX="$(mktemp -d)"
trap 'rm -rf "$CTX"' EXIT
cp "$HERE/Dockerfile" "$HERE"/*.scaly "$CTX/"
mkdir -p "$CTX/pgo"
if [ "${SCALY_PGO:-1}" != 0 ]; then
  docker build -t "$IMAGE-pgo-train" --target pgo-train "$CTX"
  DATA_MOUNT=()
  if [ -f "$DATASET" ]; then
    DATA_MOUNT=(-v "$DATASET:/data/dataset.json:ro")
  else
    echo "build.sh: no dataset at $DATASET - the training reaches /json only as a 500"
  fi
  docker run --rm --security-opt seccomp=unconfined -v "$CTX/pgo:/pgo" ${DATA_MOUNT[@]+"${DATA_MOUNT[@]}"} "$IMAGE-pgo-train" sh -c '
    SCALY_ARENA_TRAIN=1 LLVM_PROFILE_FILE=/pgo/%p.profraw /arena_instr 18090 0 0 0 &
    p=$!
    sleep 1
    /train 18090 3000
    wait $p
    llvm-profdata-21 merge -o /pgo/arena.profdata /pgo/*.profraw
    rm -f /pgo/*.profraw'
  [ -s "$CTX/pgo/arena.profdata" ] || { echo "build.sh: no profile came out of the training run"; exit 1; }
fi
docker build -t "$IMAGE" "$CTX"
