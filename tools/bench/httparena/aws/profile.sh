#!/bin/bash
# profile.sh <framework> <profile> -- runs ON the host after setup.sh: the
# profile's benchmark.sh run for one entry, and while its load generator
# runs, 6 s of perf (cpu-clock, call graphs) over the server's CPUs. The
# report -- the share per dso, then the top symbols -- and the data land in
# ~/results/perf-<framework>-<profile>.{txt,data}; the run's log beside them.
set -u
FW="$1"; PROF="$2"
HA="$HOME/HttpArena"
OUT="$HOME/results"
mkdir -p "$OUT"
# shellcheck disable=SC1091
. "$HOME/bench.env"
cd "$HA"
sudo -E bash scripts/benchmark.sh "$FW" "$PROF" > "$OUT/profile-$FW-$PROF.log" 2>&1 &
bench=$!
# the first load generator container of the run
until sudo docker ps --format '{{.Image}}' | grep -q gcannon; do
  kill -0 $bench 2> /dev/null || { echo "profile.sh: the run ended before any load"; exit 1; }
  sleep 1
done
sleep 3
D="$OUT/perf-$FW-$PROF"
sudo perf record -q -a -g -e cpu-clock -C "$SERVER_CPUS" -o "$D.data" -- sleep 6 > /dev/null 2>&1
wait $bench
{
  sudo perf report -f -g none -i "$D.data" --no-children --sort dso --stdio 2> /dev/null | grep -E '^ +[0-9]' | head -8
  echo ---
  sudo perf report -f -g none -i "$D.data" --no-children --sort symbol --stdio 2> /dev/null | grep -E '^ +[0-9]+\.[0-9]+%' | head -60 | cut -c1-120
} > "$D.txt"
sudo chown "$USER" "$D.data"
grep -E "Best:" "$OUT/profile-$FW-$PROF.log"
echo "profile.sh: $D.txt"
