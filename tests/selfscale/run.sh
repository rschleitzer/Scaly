#!/usr/bin/env bash
# Self-scaling yardstick: small idiomatic programs,
# written without any annotation, measured sequential against self-scaled.
#
#   tests/selfscale/run.sh [compiler] [rounds]      (defaults scalyc/build/scalyc, 3)
#
# Per program: the --task-plan verdicts, then a warm-up run that is thrown away
# (the first start of a fresh binary pays the endpoint scanner), then `rounds`
# ALTERNATING pairs — SCALY_WORKERS=1 (the pool capped at one worker: the
# sequential baseline, same binary) and the full pool. Reported: median wall
# clock and CPU of each side, the speedup, peak RSS, and whether both sides
# printed the same line. A measurement, not a gate: the exit status is 1 only
# when a program fails to build or the two sides disagree.
#
# ★The runtime archive must be the OPTIMIZED one (tools/build-from-seed.sh
# builds /tmp/libscaly.a through opt -O2; tools/bootstrap.sh does not): the
# allocator's bodies run at the archive's level, and under bootstrap's archive
# binary-trees and fannkuch run a third slower on BOTH sides (2026-09-26).
cd "$(dirname "$0")/../.." || exit 1
BIN=${1:-scalyc/build/scalyc}
ROUNDS=${2:-3}
OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
ulimit -s 65520
rc=0
printf '%-13s %8s %8s %8s %8s %8s %7s  %s\n' program seq-wall seq-cpu par-wall par-cpu speedup rss-MB output
for src in tests/selfscale/*.scaly; do
  p=$(basename "$src" .scaly)
  if ! "$BIN" --task-plan -O2 -o "$OUT/$p" "$src" > "$OUT/$p.plan" 2>&1; then
    echo "$p: BUILD FAILED"; sed 's/^/    /' "$OUT/$p.plan" | head -5; rc=1; continue
  fi
  "$OUT/$p" > /dev/null 2>&1
  python3 - "$OUT/$p" "$ROUNDS" "$p" <<'EOF' || rc=1
import os, resource, statistics, subprocess, sys, time
exe, rounds, name = sys.argv[1], int(sys.argv[2]), sys.argv[3]
def run(workers):
    env = dict(os.environ)
    if workers:
        env["SCALY_WORKERS"] = workers
    else:
        env.pop("SCALY_WORKERS", None)
    before = resource.getrusage(resource.RUSAGE_CHILDREN)
    t = time.monotonic()
    out = subprocess.run([exe], env=env, capture_output=True, text=True).stdout.strip()
    wall = time.monotonic() - t
    after = resource.getrusage(resource.RUSAGE_CHILDREN)
    cpu = (after.ru_utime - before.ru_utime) + (after.ru_stime - before.ru_stime)
    return wall, cpu, out, after.ru_maxrss
seq, par, outs, rss = [], [], set(), 0
for _ in range(rounds):
    for side, w in ((seq, "1"), (par, None)):
        wall, cpu, out, maxrss = run(w)
        side.append((wall, cpu)); outs.add(out); rss = max(rss, maxrss)
sw = statistics.median(x[0] for x in seq); sc = statistics.median(x[1] for x in seq)
pw = statistics.median(x[0] for x in par); pc = statistics.median(x[1] for x in par)
rss_mb = rss / (1 << 20) if sys.platform == "darwin" else rss / 1024
same = "same" if len(outs) == 1 else "DIFFERS"
print("%-13s %7.2fs %7.2fs %7.2fs %7.2fs %7.2fx %7.0f  %s: %s" % (name, sw, sc, pw, pc, sw / pw, rss_mb, same, sorted(outs)[0]))
sys.exit(0 if len(outs) == 1 else 1)
EOF
  grep -v '^task-plan: packages/' "$OUT/$p.plan" | grep -v ' for-loops,' | sed 's/^task-plan: tests\/selfscale\//    /'
done
exit $rc
