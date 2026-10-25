#!/bin/bash
# tools/fuzz/linux/run.sh [seconds 900] -- fuzz_http.py on Linux, in Docker,
# against the arena of THIS tree with what it serves on the board: the
# static files and the dataset of an HttpArena checkout, and a Postgres
# seeded with its benchmark database (async-db, fortunes, /api/items).
#   1. the entry's build stage as the image scaly-fuzz-base (once; cached)
#   2. the image scaly-fuzz (Dockerfile here): this tree's packages, the
#      arena --release, the fuzz tools
#   3. a network and a Postgres of their own (scaly-fuzz, scaly-fuzz-pg), no
#      port published on the host
#   4. the fuzzer in a container on that network, the arena as its child,
#      with seccomp unconfined as HttpArena runs it (the default profile
#      refuses io_uring, and the arena would take its other path)
# HA (default ~/scaly-bench-results/ha/HttpArena) is the checkout; OUT
# (default ./fuzz-linux-out) receives the findings and the RSS; SEED and
# JOBS go to fuzz_http.py, CPUS to the container. Network, Postgres and the
# fuzzing container are removed at the end; the two images stay.
set -eu
SECONDS_RUN="${1:-900}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../../.." && pwd)"
HA="${HA:-$HOME/scaly-bench-results/ha/HttpArena}"
OUT="$(mkdir -p "${OUT:-fuzz-linux-out}" && cd "${OUT:-fuzz-linux-out}" && pwd)"
NET=scaly-fuzz
PG=scaly-fuzz-pg
RUN=scaly-fuzz-run

docker build -q --target build -t scaly-fuzz-base "$TREE/tools/bench/httparena/scaly" > /dev/null

CTX="$(mktemp -d)"
cleanup() {
  docker rm -f "$RUN" "$PG" > /dev/null 2>&1 || true
  docker network rm "$NET" > /dev/null 2>&1 || true
  rm -rf "$CTX"
}
trap cleanup EXIT
mkdir -p "$CTX/packages" "$CTX/entry" "$CTX/fuzz"
for p in scaly http json compress tls https h3 pg; do
  cp -R "$TREE/packages/$p/0.2.0" "$CTX/packages/$p"
done
cp "$TREE"/tools/bench/httparena/scaly/*.scaly "$CTX/entry/"
cp "$TREE"/tools/fuzz/*.py "$CTX/fuzz/"
cp "$HERE/Dockerfile" "$CTX/"
docker build -q -t scaly-fuzz "$CTX" > /dev/null
echo "run.sh: image scaly-fuzz built from $(git -C "$TREE" rev-parse --short HEAD)$(git -C "$TREE" diff --quiet || echo '+changes')"

cleanup
docker network create "$NET" > /dev/null
docker run -d --rm --name "$PG" --network "$NET" \
  -e POSTGRES_USER=bench -e POSTGRES_PASSWORD=bench -e POSTGRES_DB=benchmark \
  -v "$HA/data/pgdb-seed.sql:/docker-entrypoint-initdb.d/seed.sql:ro" \
  postgres:18 -c max_connections=256 > /dev/null
for i in $(seq 1 60); do
  docker exec "$PG" psql -U bench -d benchmark -tAc 'SELECT 1 FROM items LIMIT 1' 2> /dev/null | grep -q 1 && break
  sleep 1
done

docker run --rm --name "$RUN" --network "$NET" --cpus="${CPUS:-4}" \
  --security-opt seccomp=unconfined --ulimit nofile=65536:65536 \
  -e DATABASE_URL="postgres://bench:bench@$PG:5432/benchmark" \
  -v "$HA/data:/data:ro" -v "$HA/requests:/requests:ro" -v "$OUT:/out" \
  scaly-fuzz python3 -I /fuzz/fuzz_http.py /arena --seconds "$SECONDS_RUN" \
    --seed "${SEED:-11}" --jobs "${JOBS:-4}" --seeds /requests --dataset /data/dataset.json --out /out
