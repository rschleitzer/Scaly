#!/bin/bash
# build.sh -- the Scaly entry's image, profile-guided. HttpArena's scripts run
# it from the repository's root instead of a plain `docker build`.
#
# The profile is taken in a container that ALLOWS io_uring: the training
# cannot run inside `docker build`, whose default seccomp profile refuses
# io_uring, and a profile taken on epoll would mark the io_uring paths cold.
#   1. the instrumented arena and its client (target pgo-train)
#   2. the training: the arena under the request shapes the profiles send
#      (train.scaly), its counts merged into pgo/arena.profdata -- with a
#      Postgres of its own seeded as the board's sidecar is, so that the
#      database profile (async-db) is trained too
#   3. the arena optimised with that profile (the default target)
# SCALY_PGO=0 skips 1 and 2: the same build with --release alone.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
IMAGE="${IMAGE_NAME:-httparena-scaly}"
# The dataset /json serves, so the training reaches it: the repository's own
# data/dataset.json, or HTTPARENA_DATA
DATASET="${HTTPARENA_DATA:-$PWD/data}/dataset.json"
SEED="${HTTPARENA_DATA:-$PWD/data}/pgdb-seed.sql"
CTX="$(mktemp -d)"
PG="scaly-pgo-pg-$$"
NET="scaly-pgo-$$"
cleanup() {
  docker rm -f -v "$PG" > /dev/null 2>&1 || true
  docker network rm "$NET" > /dev/null 2>&1 || true
  rm -rf "$CTX"
}
trap cleanup EXIT
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
  # the database: on a network of its own, so that it takes no port of the
  # host (the board's sidecar may hold 5432); ready when it answers over TCP,
  # which the entrypoint's init server does not
  DB_ARGS=()
  TRAIN_DB=""
  if [ -f "$SEED" ]; then
    docker network create "$NET" > /dev/null
    docker run -d --rm --name "$PG" --network "$NET" \
      --tmpfs /var/lib/postgresql:rw,size=2g \
      -e POSTGRES_USER=bench -e POSTGRES_PASSWORD=bench -e POSTGRES_DB=benchmark \
      -v "$SEED:/docker-entrypoint-initdb.d/seed.sql:ro" \
      postgres:18 -c max_connections=256 > /dev/null
    for _ in $(seq 1 90); do
      if docker exec "$PG" psql -h 127.0.0.1 -U bench -d benchmark -tAc \
          'SELECT 1 FROM items LIMIT 1' 2> /dev/null | grep -q 1; then
        DB_ARGS=(--network "$NET" -e "DATABASE_URL=postgres://bench:bench@$PG:5432/benchmark")
        TRAIN_DB=db
        break
      fi
      sleep 1
    done
    [ -n "$TRAIN_DB" ] || echo "build.sh: the training database did not come up - async-db stays untrained"
  else
    echo "build.sh: no seed at $SEED - async-db stays untrained"
  fi
  docker run --rm --security-opt seccomp=unconfined -v "$CTX/pgo:/pgo" ${DATA_MOUNT[@]+"${DATA_MOUNT[@]}"} \
    ${DB_ARGS[@]+"${DB_ARGS[@]}"} -e "TRAIN_DB=$TRAIN_DB" "$IMAGE-pgo-train" sh -c '
    SCALY_ARENA_TRAIN=1 LLVM_PROFILE_FILE=/pgo/%p.profraw /arena_instr 18090 0 0 0 &
    p=$!
    sleep 1
    /train 18090 3000 $TRAIN_DB
    wait $p
    llvm-profdata-21 merge -o /pgo/arena.profdata /pgo/*.profraw
    rm -f /pgo/*.profraw'
  [ -s "$CTX/pgo/arena.profdata" ] || { echo "build.sh: no profile came out of the training run"; exit 1; }
  docker rm -f -v "$PG" > /dev/null 2>&1 || true
fi
docker build -t "$IMAGE" "$CTX"
