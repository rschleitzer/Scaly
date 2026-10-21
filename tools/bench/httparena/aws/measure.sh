#!/bin/bash
# measure.sh [frameworks...] -- runs ON the host, under nohup (prepare.sh
# starts it): HttpArena's validate.sh for the Scaly entry, then its full
# benchmark.sh -- every profile each entry subscribes to, three runs, the best
# kept -- for Scaly and the framework entries the default board shows on top
# (standard mode, flagship or emerging; chosen 2026-10-21 from the board's
# data). Results land in ~/results, the board's JSON per framework among them.
set -u
FRAMEWORKS="${*:-scaly genhttp-ioxide araara dusty tako bun ntex fulmine}"
HA="$HOME/HttpArena"
OUT="$HOME/results"
mkdir -p "$OUT"
# shellcheck disable=SC1091
. "$HOME/bench.env"
cd "$HA"
echo "measure: start $(date -u +%FT%TZ), HttpArena $(git rev-parse --short HEAD)" | tee "$OUT/progress.txt"
sudo -E bash scripts/validate.sh scaly > "$OUT/validate-scaly.log" 2>&1
echo "validate scaly: rc $? $(grep -cE '^\s*PASS' "$OUT/validate-scaly.log") pass, $(grep -cE 'FAIL' "$OUT/validate-scaly.log") fail" | tee -a "$OUT/progress.txt"
for fw in $FRAMEWORKS; do
  start=$(date +%s)
  sudo -E timeout -k 60 90m bash scripts/benchmark.sh "$fw" --save > "$OUT/bench-$fw.log" 2>&1
  echo "bench $fw: rc $? in $(( ($(date +%s) - start) / 60 )) min" | tee -a "$OUT/progress.txt"
  sudo docker ps -q | xargs -r sudo docker rm -f > /dev/null 2>&1 || true
done
cp site/data/results/*.json "$OUT/" 2> /dev/null || true
echo "measure: done $(date -u +%FT%TZ)" | tee -a "$OUT/progress.txt"
