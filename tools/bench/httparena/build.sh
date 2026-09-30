#!/bin/bash
# tools/bench/httparena/build.sh [image] — build the Scaly entry for HttpArena
# (default image name httparena-scaly, the name HttpArena's scripts expect for
# a framework directory called scaly).
#
# The Docker build context is staged from the WORKING TREE, not the whole
# repository (the test corpora alone are gigabytes): the seed, the tools the
# seed build calls, the scaly/scalyc/http/json/compress/tls/https/h3/pg/redis packages and
# the arena and trainer programs. To run it under HttpArena's own scripts, put a frameworks/scaly/
# directory in an HttpArena checkout holding meta.json and a build.sh that
# calls this one:
#
#   frameworks/scaly/build.sh:   exec /path/to/Scaly/tools/bench/httparena/build.sh
#   scripts/validate.sh scaly
#
# The gateway profiles are framework directories of their own beside this
# file, copied to frameworks/ as they are: scaly_nginx (gateway-64 and
# production-stack behind nginx) and scaly_caddy (gateway-h3 behind Caddy).
# Their server service is the image this script builds, so it runs first.
#
# Profile-guided (the author's decision, 2026-09-28): an instrumented arena is
# built first (Dockerfile target pgo-train) and trained by
# tools/bench/http/train.scaly in a container that ALLOWS io_uring — the
# training cannot run inside `docker build`, whose default seccomp profile
# refuses io_uring, and a profile taken on epoll would mark the io_uring
# paths cold. The profile then goes into the final build. SCALY_PGO=0 builds
# without it (the same whole-program build, for comparison).
set -eu
# The dataset the /json profile serves: HttpArena's own data/dataset.json,
# found from the directory validate.sh calls this from (the HttpArena root),
# or named by HTTPARENA_DATA. The training run needs it to reach /json.
DATASET="${HTTPARENA_DATA:-$PWD/data}/dataset.json"
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../../.." && pwd)"
IMAGE=${1:-httparena-scaly}
CTX="$(mktemp -d)"
trap 'rm -rf "$CTX"' EXIT
cd "$ROOT"
mkdir -p "$CTX/packages" "$CTX/tools/bench/http" "$CTX/pgo"
cp -R seed "$CTX/"
# the tools directory without the benchmark programs and their build output
( cd tools && find . -maxdepth 1 -type f -exec cp {} "$CTX/tools/" \; )
cp -R packages/scaly packages/scalyc packages/http packages/json packages/compress packages/tls packages/https packages/h3 packages/pg packages/redis "$CTX/packages/"
cp tools/bench/http/arena.scaly tools/bench/http/train.scaly "$CTX/tools/bench/http/"
rm -f "$CTX"/seed/r_*.ll
if [ "${SCALY_PGO:-1}" != 0 ]; then
  docker build -t "$IMAGE-pgo-train" --target pgo-train -f "$HERE/Dockerfile" "$CTX"
  DATA_MOUNT=()
  if [ -f "$DATASET" ]; then
    DATA_MOUNT=(-v "$DATASET:/data/dataset.json:ro")
  else
    echo "build.sh: no dataset at $DATASET - the training reaches /json only as a 500"
  fi
  docker run --rm --security-opt seccomp=unconfined -v "$CTX/pgo:/pgo" ${DATA_MOUNT[@]+"${DATA_MOUNT[@]}"} "$IMAGE-pgo-train" sh -c '
    SCALY_ARENA_TRAIN=1 LLVM_PROFILE_FILE=/pgo/%p.profraw /arena_instr 18090 &
    p=$!
    sleep 1
    /train 18090 3000
    wait $p
    llvm-profdata-21 merge -o /pgo/arena.profdata /pgo/*.profraw
    rm -f /pgo/*.profraw'
  [ -s "$CTX/pgo/arena.profdata" ] || { echo "build.sh: no profile came out of the training run"; exit 1; }
fi
docker build -t "$IMAGE" -f "$HERE/Dockerfile" "$CTX"
