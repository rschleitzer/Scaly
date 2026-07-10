#!/bin/bash
# Stage-7 milestone 7.0 wire spike: two OS processes on TCP loopback —
# the sender ships a message region's pages VERBATIM, frees the region,
# and the receiver hand-swizzles + byte-verifies its own copy, then
# both run throughput/latency phases. NOT wired into CI (the spike
# precedes the tests/cluster suite, which arrives with 7.1).
# Needs /tmp/libscaly.a (see CLAUDE.md runtime-archive recipe).
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$HERE/../../.." && pwd)"
OUT=${TMPDIR:-/tmp}
cd "$REPO"
./scalyc/build/scalyc -o "$OUT/spike7_recv" "$HERE/recv7.scaly"
./scalyc/build/scalyc -o "$OUT/spike7_send" "$HERE/send7.scaly"
"$OUT/spike7_recv" > "$OUT/spike7_recv.log" 2>&1 &
RPID=$!
sleep 0.5
"$OUT/spike7_send" > "$OUT/spike7_send.log" 2>&1 || echo "SENDER FAILED"
wait $RPID || echo "RECEIVER FAILED"
echo "=== recv ==="
cat "$OUT/spike7_recv.log"
echo "=== send ==="
cat "$OUT/spike7_send.log"
grep -q "verify PASS" "$OUT/spike7_recv.log" && grep -q "region PASS" "$OUT/spike7_send.log" && echo "SPIKE PASS"
