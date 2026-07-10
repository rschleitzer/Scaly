#!/bin/bash
# Cluster suite (self-scaling stage 7) — the 7.1 region-serialization
# round-trips: in-process (roundtrip.scaly, two rounds proving page
# recycling) and cross-process (xproc_send/xproc_recv over TCP loopback,
# the received region materialized into real pages and released through
# the normal lifecycle). The 7.0 spike under spike/ stays a wire-format
# reference and is NOT run here (it benchmarks; this suite verifies).
#
# Usage: tests/cluster/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}
OUT=${TMPDIR:-/tmp}

# Top up an older archive that predates the fiber/eio objects (both are
# self-contained; a missing archive fails the compile loudly anyway).
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^fcontext\.o$'; then
  tools/fcontext.sh /tmp/fcontext.o && ar rcs /tmp/libscaly.a /tmp/fcontext.o
fi
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^eio\.o$'; then
  tools/eio.sh /tmp/eio.o && ar rcs /tmp/libscaly.a /tmp/eio.o
fi

pass=0; fail=0; failures=()

# ---- in-process round-trip: compare stdout to the "; Expected:" lines ----
t=roundtrip
expected=$(sed -n 's/^; Expected: //p' tests/cluster/$t.scaly)
bin="$OUT/cluster_$t"; rm -f "$bin"
if ! "$STAGE" -o "$bin" tests/cluster/$t.scaly >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  out=$("$bin" 2>/dev/null); rc=$?
  if [ "$rc" = "0" ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: rc=$rc '$out'")
  fi
fi

# ---- cross-process round-trip: receiver first, sender retries connect ----
ok=1
for t in xproc_recv xproc_send; do
  rm -f "$OUT/cluster_$t"
  if ! "$STAGE" -o "$OUT/cluster_$t" tests/cluster/$t.scaly >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); ok=0
  fi
done
if [ "$ok" = "1" ]; then
  "$OUT/cluster_xproc_recv" > "$OUT/cluster_xproc_recv.log" 2>&1 &
  RPID=$!
  "$OUT/cluster_xproc_send" > "$OUT/cluster_xproc_send.log" 2>&1
  SRC=$?
  wait $RPID
  RRC=$?
  if [ "$SRC" = "0" ] && [ "$RRC" = "0" ] \
     && grep -q "^S: PASS$" "$OUT/cluster_xproc_send.log" \
     && grep -q "^R: verify PASS$" "$OUT/cluster_xproc_recv.log"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("xproc: send rc=$SRC recv rc=$RRC $(tail -1 "$OUT/cluster_xproc_send.log") / $(tail -1 "$OUT/cluster_xproc_recv.log")")
  fi
fi

echo "cluster: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
