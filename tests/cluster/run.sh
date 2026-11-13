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
. tests/platform.sh || exit 1
STAGE=${1:-$SCALY_STAGE_DEFAULT}
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$(tools/scaly-of.sh "$STAGE")
OUT=${TMPDIR:-/tmp}

# Every program is built with the stage binary's `build` command: the runtime
# comes out of the build cache.
pass=0; fail=0; failures=()

# ---- in-process round-trip: compare stdout to the "; Expected:" lines ----
t=roundtrip
expected=$(sed -n 's/^; Expected: //p' tests/cluster/$t.scaly)
bin="$OUT/cluster_$t$SCALY_EXE"; rm -f "$bin"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$bin" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  "$bin" > "$OUT/cluster_$t.out" 2>/dev/null; rc=$?
  out=$(scaly_lf < "$OUT/cluster_$t.out")
  if [ "$rc" = "0" ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t: rc=$rc '$out'")
  fi
fi

# ---- cross-process round-trip: receiver first, sender retries connect ----
ok=1
for t in xproc_recv xproc_send; do
  rm -f "$OUT/cluster_$t$SCALY_EXE"
  if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); ok=0
  fi
done
if [ "$ok" = "1" ]; then
  "$OUT/cluster_xproc_recv$SCALY_EXE" > "$OUT/cluster_xproc_recv.log" 2>&1 &
  RPID=$!
  "$OUT/cluster_xproc_send$SCALY_EXE" > "$OUT/cluster_xproc_send.log" 2>&1
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

# Wait until a log file contains a marker line. $3 = tick budget, 0.05s each
# (default 200 = ~10s). Pass a larger one for any marker whose arrival time
# scales with MACHINE SPEED rather than with a round trip — see the fault test.
wait_marker() {
  for _ in $(seq 1 "${3:-200}"); do
    grep -q "$2" "$1" 2>/dev/null && return 0
    sleep 0.05
  done
  return 1
}

# ---- 7.3 remote-channel ping-pong: ONE binary, two roles (rule 10:
# one cluster one build — different programs would refuse each other) ----
t=pingpong
rm -f "$OUT/cluster_$t$SCALY_EXE"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  PINGPONG_ROLE=b "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_b.log" 2>&1 &
  BPID=$!
  wait_marker "$OUT/cluster_${t}_b.log" "^B: ready$"
  PINGPONG_ROLE=a "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_a.log" 2>&1
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

# ---- a frame that arrives after the local close is dropped, not a trap ----
# 200 connections: B floods, A takes one message and says goodbye in the
# middle of the stream. Before 2026-10-05 A's reader delivered the frame it
# had in hand into the channel the close had just closed: exit 110, in 6 runs
# of 6 here and in one run of thirty of the trainer below (train parity).
t=lateframe
rm -f "$OUT/cluster_$t$SCALY_EXE"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  LATEFRAME_ROLE=b "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_b.log" 2>&1 &
  BPID=$!
  wait_marker "$OUT/cluster_${t}_b.log" "^B: ready$"
  LATEFRAME_ROLE=a "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_a.log" 2>&1
  ARC=$?
  # B waits in accept for a connection that never comes when A died early
  [ "$ARC" = "0" ] || kill $BPID 2>/dev/null
  wait $BPID
  BRC=$?
  if [ "$ARC" = "0" ] && [ "$BRC" = "0" ] \
     && grep -q "^A: PASS$" "$OUT/cluster_${t}_a.log" \
     && grep -q "^B: PASS$" "$OUT/cluster_${t}_b.log"; then
    pass=$((pass+1))
  else
    fail=$((fail+1))
    failures+=("lateframe: a rc=$ARC b rc=$BRC $(tail -1 "$OUT/cluster_${t}_a.log") / $(tail -1 "$OUT/cluster_${t}_b.log")")
  fi
fi

# ---- 7.3 kill -9 mid-run: survivor's receive nulls + monitor fires ----
t=kill9
rm -f "$OUT/cluster_$t$SCALY_EXE"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  KILL9_ROLE=s "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_s.log" 2>&1 &
  SPID=$!
  wait_marker "$OUT/cluster_${t}_s.log" "^S: ready$"
  KILL9_ROLE=v "$OUT/cluster_$t$SCALY_EXE" > "$OUT/cluster_${t}_v.log" 2>&1 &
  VPID=$!
  if wait_marker "$OUT/cluster_${t}_s.log" "^S: linked$"; then
    kill -9 $VPID 2>/dev/null
  else
    # never linked: the survivor would wait for its peer forever (measured
    # beside a loaded machine) — end both and let the verdict say FAIL
    kill -9 $VPID $SPID 2>/dev/null
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
  rm -f "$OUT/cluster_$t$SCALY_EXE"
  if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile)"); ok=0
  fi
done
if [ "$ok" = "1" ]; then
  "$OUT/cluster_stamp_a$SCALY_EXE" > "$OUT/cluster_stamp_a.log" 2> "$OUT/cluster_stamp_a.err" &
  APID=$!
  wait_marker "$OUT/cluster_stamp_a.log" "^A: ready$"
  "$OUT/cluster_stamp_b$SCALY_EXE" > "$OUT/cluster_stamp_b.log" 2> "$OUT/cluster_stamp_b.err"
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
rm -f "$OUT/cluster_$t$SCALY_EXE"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$OUT/cluster_$t$SCALY_EXE" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  BIN="$OUT/cluster_$t$SCALY_EXE"
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
if [ -x "$OUT/cluster_train$SCALY_EXE" ]; then
  BIN="$OUT/cluster_train$SCALY_EXE"
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
  # ★The BUDGET, however, was not speed-independent, and that is the whole
  # trap: the first progress line is step 200 of 1200, so it arrives at
  # 200/rate seconds — 17 s at the 12 steps/s this x86_64 Linux box manages,
  # comfortably past the 10 s default. wait_marker then returned 1, the kill
  # never fired, all three ranks ran to completion, and the assertion below
  # reported a missing "R: down 2" — which reads exactly like a broken failure
  # DETECTOR while nothing had died at all (2026-08-14, the first time this
  # suite ran on Linux; CI's Linux leg does not run it). 60 s covers a machine
  # roughly six times slower than the dev box and still leaves 1000 steps
  # after the kill. A missed window is now reported instead of being silent.
  if wait_marker "$OUT/train_faultR.log" "^L " 1200; then
    kill -9 $W2 2>/dev/null
  else
    echo "cluster: train fault — no progress line within 60s, kill window missed"
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

# ---- helpers (ROADMAP-cluster.md, C1): one program, started with
# --scaly-helper or normally; a coordinator finds the helpers of its own
# build in the group and connects. Without a key the group is the
# loopback's; with one the helpers must hold it, and a helper of another
# build is passed over. All on this machine, so on the loopback. ----
t=helpers
HB="$OUT/cluster_$t$SCALY_EXE"
rm -f "$HB" "$OUT/cluster_${t}_other$SCALY_EXE"
# another build of the same program: one statement more, another stamp
{ cat tests/cluster/$t.scaly; echo 'print "another build"'; } > "$OUT/cluster_${t}_other.scaly"
if ! "$SCALY" build tests/cluster/$t.scaly -o "$HB" >/dev/null 2>&1 \
   || ! "$SCALY" build "$OUT/cluster_${t}_other.scaly" -o "$OUT/cluster_${t}_other$SCALY_EXE" >/dev/null 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  # helpers <env of the helpers> <env of the coordinator> <expected line> <what>
  helpers_case() {
    local hpids=()
    env $1 "$HB" --scaly-helper > "$OUT/cluster_${t}_h1.log" 2>&1 & hpids+=($!)
    env $1 "$HB" --scaly-helper > "$OUT/cluster_${t}_h2.log" 2>&1 & hpids+=($!)
    wait_marker "$OUT/cluster_${t}_h1.log" "^helper: ready" && wait_marker "$OUT/cluster_${t}_h2.log" "^helper: ready"
    local got
    got=$(env $2 "$HB" 2>&1 | tail -1)
    kill "${hpids[@]}" 2>/dev/null; wait "${hpids[@]}" 2>/dev/null
    if [ "$got" = "$3" ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("helpers, $4: '$got'"); fi
  }
  helpers_case "X=1" "X=1" "found 2, connected 2" "no key"
  helpers_case "SCALY_CLUSTER_KEY=k1" "SCALY_CLUSTER_KEY=k1" "found 2, connected 2" "one key"
  helpers_case "SCALY_CLUSTER_KEY=k1" "SCALY_CLUSTER_KEY=k2" "found 0, connected 0" "another key"
  helpers_case "SCALY_CLUSTER_KEY=k1" "X=1" "found 0, connected 0" "a key on the helpers only"
  # a helper of another build is passed over; one started after the
  # coordinator began to look is found (it announces itself at once)
  "$OUT/cluster_${t}_other$SCALY_EXE" --scaly-helper > "$OUT/cluster_${t}_o.log" 2>&1 & OPID=$!
  wait_marker "$OUT/cluster_${t}_o.log" "^helper: ready"
  "$HB" > "$OUT/cluster_${t}_c.log" 2>&1 & CPID=$!
  sleep 0.3
  "$HB" --scaly-helper > "$OUT/cluster_${t}_late.log" 2>&1 & LPID=$!
  wait $CPID
  got=$(tail -1 "$OUT/cluster_${t}_c.log")
  kill $OPID $LPID 2>/dev/null; wait $OPID $LPID 2>/dev/null
  if [ "$got" = "found 1, connected 1" ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("helpers, another build and a late one: '$got'"); fi
fi

# ---- rounds on other machines (ROADMAP-cluster.md, C2): a program built
# with --cluster offers a fit loop's rounds to the helpers of its build
# (SCALY_CLUSTER=on); each runs its stretches on its cores and sends back
# what they put. The output is the program's own: with two helpers, with
# one of them killed while it computes, and with none. Built by the
# compiler itself (the tool's `build` takes no --cluster yet). ----
t=rounds
RB="$OUT/cluster_$t$SCALY_EXE"
rm -f "$RB"
expected=$(sed -n 's/^; Expected: //p' tests/cluster/$t.scaly)
if ! "$STAGE" --cluster -O2 -o "$RB" tests/cluster/$t.scaly >"$OUT/cluster_${t}_build.log" 2>&1; then
  fail=$((fail+1)); failures+=("$t(compile)")
else
  # rounds_check <what> <out file> <expected trace>
  rounds_check() {
    local out; out=$(scaly_lf < "$2" | grep -v '^cluster: ')
    if [ "$out" = "$expected" ] && grep -q "$3" "$2"; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$t, $1: '$(tr '\n' '|' < "$2")'"); fi
  }
  # the loops that travel, and only those: two descriptors, none without --cluster
  "$STAGE" --cluster -S -o "$OUT/cluster_$t.ll" tests/cluster/$t.scaly >/dev/null 2>&1
  "$STAGE" -S -o "$OUT/cluster_${t}_plain.ll" tests/cluster/$t.scaly >/dev/null 2>&1
  n_with=$(grep -c '^@cluster.loop.* = private constant' "$OUT/cluster_$t.ll")
  n_without=$(grep -c 'cluster' "$OUT/cluster_${t}_plain.ll")
  if [ "$n_with" = "2" ] && [ "$n_without" = "0" ]; then pass=$((pass+1)); else fail=$((fail+1)); failures+=("$t: descriptors $n_with with --cluster, $n_without lines without"); fi
  # on this machine
  "$RB" > "$OUT/cluster_${t}_alone.log" 2>&1
  rounds_check "alone" "$OUT/cluster_${t}_alone.log" "^string"
  # two helpers
  SCALY_CLUSTER_TRACE=1 "$RB" --scaly-helper > "$OUT/cluster_${t}_h1.log" 2>&1 & H1=$!
  SCALY_CLUSTER_TRACE=1 "$RB" --scaly-helper > "$OUT/cluster_${t}_h2.log" 2>&1 & H2=$!
  wait_marker "$OUT/cluster_${t}_h1.log" "^helper: ready" && wait_marker "$OUT/cluster_${t}_h2.log" "^helper: ready"
  SCALY_CLUSTER=on SCALY_CLUSTER_TRACE=1 "$RB" > "$OUT/cluster_${t}_two.log" 2>&1
  rounds_check "two helpers" "$OUT/cluster_${t}_two.log" "rounds in .* stretches on 2 helpers, all there"
  # one killed while it computes: its stretch goes to the other
  : > "$OUT/cluster_${t}_h1.log"
  SCALY_CLUSTER=on SCALY_CLUSTER_TRACE=1 "$RB" > "$OUT/cluster_${t}_kill.log" 2>&1 & CPID=$!
  wait_marker "$OUT/cluster_${t}_h1.log" "^helper: rounds" && kill -9 $H1 2>/dev/null
  wait $CPID
  wait $H1 2>/dev/null
  rounds_check "a helper killed" "$OUT/cluster_${t}_kill.log" "all there"
  kill $H2 2>/dev/null; wait $H2 2>/dev/null
  # none: the loops run here
  SCALY_CLUSTER=on SCALY_CLUSTER_TRACE=1 "$RB" > "$OUT/cluster_${t}_none.log" 2>&1
  rounds_check "no helper" "$OUT/cluster_${t}_none.log" "^string"
fi

echo "cluster: $pass PASS, $fail FAIL ${failures[*]}"
[ $fail -eq 0 ]
