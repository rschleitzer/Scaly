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

# Wait until a log file contains a marker line (max ~10s).
wait_marker() {
  for _ in $(seq 1 200); do
    grep -q "$2" "$1" 2>/dev/null && return 0
    sleep 0.05
  done
  return 1
}

# ---- 7.3 remote-channel ping-pong: ONE binary, two roles (rule 10:
# one cluster one build — different programs would refuse each other) ----
t=pingpong
rm -f "$OUT/cluster_$t"
if ! "$STAGE" -o "$OUT/cluster_$t" tests/cluster/$t.scaly >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  PINGPONG_ROLE=b "$OUT/cluster_$t" > "$OUT/cluster_${t}_b.log" 2>&1 &
  BPID=$!
  wait_marker "$OUT/cluster_${t}_b.log" "^B: ready$"
  PINGPONG_ROLE=a "$OUT/cluster_$t" > "$OUT/cluster_${t}_a.log" 2>&1
  ARC=$?
  wait $BPID
  BRC=$?
  if [ "$ARC" = "0" ] && [ "$BRC" = "0" ] \
     && grep -q "^A: PASS$" "$OUT/cluster_${t}_a.log" \
     && grep -q "^B: PASS$" "$OUT/cluster_${t}_b.log"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("pingpong: a rc=$ARC b rc=$BRC $(tail -1 "$OUT/cluster_${t}_a.log") / $(tail -1 "$OUT/cluster_${t}_b.log")")
  fi
fi

# ---- 7.3 kill -9 mid-run: survivor's receive nulls + monitor fires ----
t=kill9
rm -f "$OUT/cluster_$t"
if ! "$STAGE" -o "$OUT/cluster_$t" tests/cluster/$t.scaly >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  KILL9_ROLE=s "$OUT/cluster_$t" > "$OUT/cluster_${t}_s.log" 2>&1 &
  SPID=$!
  wait_marker "$OUT/cluster_${t}_s.log" "^S: ready$"
  KILL9_ROLE=v "$OUT/cluster_$t" > "$OUT/cluster_${t}_v.log" 2>&1 &
  VPID=$!
  if wait_marker "$OUT/cluster_${t}_s.log" "^S: linked$"; then
    kill -9 $VPID 2>/dev/null
  fi
  wait $VPID 2>/dev/null
  wait $SPID
  SRC=$?
  if [ "$SRC" = "0" ] && grep -q "^S: PASS$" "$OUT/cluster_${t}_s.log" \
     && grep -q "^S: down suspect$" "$OUT/cluster_${t}_s.log"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("kill9: survivor rc=$SRC $(tail -1 "$OUT/cluster_${t}_s.log")")
  fi
fi

# ---- 7.3 rule-10 stamp mismatch: two builds refuse, both stamps shown ----
ok=1
for t in stamp_a stamp_b; do
  rm -f "$OUT/cluster_$t"
  if ! "$STAGE" -o "$OUT/cluster_$t" tests/cluster/$t.scaly >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); ok=0
  fi
done
if [ "$ok" = "1" ]; then
  "$OUT/cluster_stamp_a" > "$OUT/cluster_stamp_a.log" 2> "$OUT/cluster_stamp_a.err" &
  APID=$!
  wait_marker "$OUT/cluster_stamp_a.log" "^A: ready$"
  "$OUT/cluster_stamp_b" > "$OUT/cluster_stamp_b.log" 2> "$OUT/cluster_stamp_b.err"
  BRC=$?
  wait $APID
  ARC=$?
  if [ "$ARC" = "0" ] && [ "$BRC" = "0" ] \
     && grep -q "^A: refused$" "$OUT/cluster_stamp_a.log" \
     && grep -q "^B: refused$" "$OUT/cluster_stamp_b.log" \
     && grep -q "build stamp mismatch.*local .* peer " "$OUT/cluster_stamp_a.err" \
     && grep -q "build stamp mismatch.*local .* peer " "$OUT/cluster_stamp_b.err"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("stamp: a rc=$ARC b rc=$BRC $(tail -1 "$OUT/cluster_stamp_a.err") / $(tail -1 "$OUT/cluster_stamp_b.err")")
  fi
fi

# ---- 7.4 distributed trainer: bit-exact loss parity vs single-node ----
# ONE binary; solo reference sums a global batch of N windows itself, the
# distributed run splits it one-window-per-rank and all-reduces the
# gradients in rank order — identical arithmetic, so identical loss lines.
t=train
rm -f "$OUT/cluster_$t"
if ! "$STAGE" -o "$OUT/cluster_$t" tests/cluster/$t.scaly >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  BIN="$OUT/cluster_$t"
  ok=1
  for NR in 2 4; do
    TRAIN_N=$NR "$BIN" 2>/dev/null | grep '^L ' > "$OUT/train_solo$NR.L"
    port=$((47720 + NR))
    TRAIN_N=$NR TRAIN_RANK=0 TRAIN_PORT=$port "$BIN" > "$OUT/train_red$NR.log" 2>&1 &
    RPID=$!
    wait_marker "$OUT/train_red$NR.log" "^R: ready"
    wpids=()
    r=1
    while [ $r -lt $NR ]; do
      TRAIN_N=$NR TRAIN_RANK=$r TRAIN_PORT=$port "$BIN" > "$OUT/train_w${NR}_$r.log" 2>&1 &
      wpids+=($!)
      r=$((r + 1))
    done
    wait $RPID; RRC=$?
    for wp in "${wpids[@]}"; do wait "$wp" 2>/dev/null; done
    grep '^L ' "$OUT/train_red$NR.log" > "$OUT/train_red$NR.L"
    if [ "$RRC" != "0" ] || ! diff -q "$OUT/train_solo$NR.L" "$OUT/train_red$NR.L" >/dev/null 2>&1; then
      ok=0; failures+=("train parity N=$NR: rrc=$RRC")
    fi
  done
  if [ "$ok" = "1" ]; then pass=$((pass+1)); else fail=$((fail+1)); fi
fi

# ---- 7.4 fault tolerance: kill a worker mid-run, run completes on the
# survivors (reducer detects the dead rank's channel wake null) ----
if [ -x "$OUT/cluster_train" ]; then
  BIN="$OUT/cluster_train"
  P=47730
  TRAIN_N=3 TRAIN_STEPS=1200 TRAIN_RANK=0 TRAIN_PORT=$P "$BIN" > "$OUT/train_faultR.log" 2>&1 &
  RPID=$!
  wait_marker "$OUT/train_faultR.log" "^R: ready"
  TRAIN_N=3 TRAIN_STEPS=1200 TRAIN_RANK=1 TRAIN_PORT=$P "$BIN" > "$OUT/train_faultW1.log" 2>&1 &
  W1=$!
  TRAIN_N=3 TRAIN_STEPS=1200 TRAIN_RANK=2 TRAIN_PORT=$P "$BIN" > "$OUT/train_faultW2.log" 2>&1 &
  W2=$!
  # kill worker 2 once training is underway (first progress line): the
  # run is provably mid-flight and has steps left, at any machine speed.
  if wait_marker "$OUT/train_faultR.log" "^L "; then
    kill -9 $W2 2>/dev/null
  fi
  wait $RPID; RRC=$?
  wait $W1 2>/dev/null; W1RC=$?
  wait $W2 2>/dev/null
  if [ "$RRC" = "0" ] && [ "$W1RC" = "0" ] \
     && grep -q "^R: down 2$" "$OUT/train_faultR.log" \
     && grep -q "^R: PASS" "$OUT/train_faultR.log"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("train fault: rrc=$RRC w1=$W1RC $(grep -E 'down|PASS' "$OUT/train_faultR.log" | tr '\n' ' ')")
  fi
fi

echo "cluster: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
