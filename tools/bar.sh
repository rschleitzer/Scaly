#!/usr/bin/env bash
# The full bar in one command, with every independent part running side by side.
#
#   tools/bar.sh            the whole bar, tscaly's stage-2 and case yardsticks included
#   tools/bar.sh --quick    ... tscaly at stage 1 only (the ~9 minute lane becomes ~1)
#
# Phase 1 is the compiler and is serial by nature, each step feeding the next:
#   ./mkp -> tools/bootstrap.sh -> tools/seed.sh -> tools/build-from-seed.sh
# seed.sh emits into a scratch directory; when its five roots differ from seed/
# they are copied in and SHA256SUMS is rewritten (the seed refresh an emission
# change owes), and build-from-seed then builds the compiler every suite uses
# from THAT seed. tools/cycle.sh is not run: seed.sh's fixed point is the
# stronger gate and its bootstrap is the same one.
#
# Phase 2 runs in lanes, all at once; inside a lane the steps are in order:
#   compiler  regress, selfhosted, target, fiber, escape, pointer-report,
#             write-report, abi, debuginfo, then tools/interfaces.sh
#             --check — last, so its 6.5 GB tscaly compilation does not overlap
#             the one the tscaly lane starts with
#   lsp       tests/lsp/run.sh
#   ports     one dazzle CLI build shared by all 14 dazzle suites (DAZZLE_PREBUILT),
#             which run side by side; then onsgmls, the SGML corpus, opensp, http, json, compress, tls, pg, redis, tool, cmscratch
#   tscaly    tests/run.sh at stage 2 (its corpus contains stage 1's), then the
#             two case yardsticks side by side — the longest lane
#   vscode    only with BAR_VSCODE_SCENARIO and BAR_VSCODE_BASELINE set: the bench
#             build, then one --bench-batch run (in BAR_VSCODE_DIR, default the
#             scenario's directory) whose first BAR_VSCODE_LINES lines must equal
#             the baseline's
# tests/cluster runs last and alone: it is bound by TIMING (see below).
# The port and tscaly suites run under SCALY_POISON=1, as the acid rounds did.
#
# Every step's output is in its own log under $BAR_LOG (default: a fresh
# /tmp/bar.XXXXXX); the summary prints one line per step and the lanes' wall
# times. Exit status 0 only if every step passed.
#
# Shared /tmp state: bootstrap/seed/build write /tmp/scalyc_stage*, /tmp/sc*.o and
# /tmp/libscaly.a, so NOTHING else may bootstrap while phase 1 runs; phase 2 only
# reads them.
set -u
cd "$(dirname "$0")/.."
ROOT=$PWD

QUICK=0
[ "${1:-}" = "--quick" ] && QUICK=1

LOG=${BAR_LOG:-$(mktemp -d /tmp/bar.XXXXXX)}
# the build cache of the one tool (`scaly build`/`test`, ROADMAP-public.md):
# one per run, beside the logs -- every suite of the run shares it, and the
# user's ~/.scaly/cache does not collect an entry per compiler the bar builds
export SCALY_CACHE="$LOG/cache"
mkdir -p "$LOG"
BIN=$ROOT/scalyc/build/scalyc
T0=$(date +%s)

# glibc's malloc may ask for transparent huge pages (MADV_HUGEPAGE), and the
# compiler's sparse page use then costs twice the memory: tscaly's interface
# compilation measured 12.1 GB against 6.76 GB with the tunable off, on glibc
# 2.43 / aarch64 (2026-10-03), same bytes out, no slower. Beside the other
# lanes that was the difference between a bar and an OOM kill on 18 GB.
# Appended, so a caller's own tunables stay.
if [ "$(uname -s)" = Linux ]; then
  export GLIBC_TUNABLES="${GLIBC_TUNABLES:+$GLIBC_TUNABLES:}glibc.malloc.hugetlb=0"
fi

# step <name> <command...>: run into $LOG/<name>.log, print one verdict line.
step() {
  local name=$1 st rc; shift
  st=$(date +%s)
  "$@" > "$LOG/$name.log" 2>&1; rc=$?
  printf '%-16s %s %4ss  %s\n' "$name" "$([ $rc = 0 ] && echo ok || echo "FAIL($rc)")" \
    "$(( $(date +%s) - st ))" "$(grep -v '^\s*$' "$LOG/$name.log" | tail -1 | cut -c1-90)"
  return $rc
}

# ---- phase 1: the compiler ---------------------------------------------------

seed_refresh() {
  local out=$LOG/seed f changed=""
  tools/seed.sh /tmp/scalyc_stage2 "$out" || return 1
  for f in main.ll scaly_main.ll scalyc.ll scaly.ll scalyls.ll scalyls_main.ll json.ll; do
    cmp -s "$out/$f" "seed/$f" || { cp "$out/$f" "seed/$f"; changed="$changed $f"; }
  done
  if [ -n "$changed" ]; then
    # The sed: Perl's shasum marks the read mode before the name, a space for
    # text and a `*` for binary -- and in Git Bash it reads binary, so a refresh
    # on Windows rewrote every line of an unchanged manifest. Same digests,
    # one spelling everywhere (2026-10-03).
    ( cd seed && shasum -a 256 main.ll scaly_main.ll scalyc.ll scaly.ll scalyls.ll scalyls_main.ll json.ll | sed 's/ \*/  /' > SHA256SUMS )
    echo "seed REFRESHED:$changed"
  else
    echo "seed unchanged"
  fi
}

phase1() {
  step mkp ./mkp || return 1
  step bootstrap tools/bootstrap.sh || return 1
  grep -q '^bootstrap: OK' "$LOG/bootstrap.log" || { echo "bootstrap: no OK line"; return 1; }
  if grep -q 'Emitter:' "$LOG/bootstrap.log"; then echo "bootstrap: Emitter abort in the log"; return 1; fi
  step seed seed_refresh || return 1
  step build tools/build-from-seed.sh scalyc/build/scalyc || return 1
}

echo "bar: logs in $LOG"
phase1 | tee "$LOG/phase1.txt"
[ "${PIPESTATUS[0]}" = 0 ] || { echo "bar: phase 1 FAILED — no suite was run"; exit 1; }
T1=$(date +%s)

# ---- phase 2: the lanes ------------------------------------------------------

lane_compiler() {
  local rc=0
  step regress tests/regress/run.sh "$BIN" || rc=1
  step selfhosted tests/selfhosted/run.sh "$BIN" || rc=1
  step target tests/target/run.sh "$BIN" || rc=1
  step fiber tests/fiber/run.sh "$BIN" || rc=1
  step escape tests/escape/run.sh "$BIN" || rc=1
  step pointer-report tests/pointer-report/run.sh "$BIN" || rc=1
  step write-report tests/write-report/run.sh "$BIN" || rc=1
  step abi tests/abi/run.sh || rc=1
  step debuginfo tests/debuginfo/run.sh "$BIN" || rc=1
  step interfaces tools/interfaces.sh --check "$BIN" || rc=1
  return $rc
}

lane_lsp() {
  # A request's 60 s budget is sized for an idle machine; beside the other lanes
  # `definition` over the whole tree exceeded it and answered empty (a red that
  # is the machine, not the server). The budget tests set or remove the variable
  # themselves, so the lane only moves the ceiling.
  # SCALYLS_PREBUILT: the server phase 1's build step made from the same fresh
  # seed (opt -O2); the suite then skips its own -O2 build of the four roots.
  step lsp env SCALYLS_BUDGET_MS=300000 SCALYLS_PREBUILT="$ROOT/scalyc/build/scalyls" tests/lsp/run.sh "$BIN"
}

dazzle_all() {
  local d rc=0 pids=() names=()
  for d in cli codegen coding engine flowobj fot framemark grove html mif prims rtf specarena tex; do
    ( DAZZLE_PREBUILT=$LOG/dazzle SCALY_POISON=1 tests/dazzle/$d/run.sh > "$LOG/dz_$d.log" 2>&1 ) &
    pids+=($!); names+=($d)
  done
  for i in "${!pids[@]}"; do
    if wait "${pids[$i]}"; then echo "dazzle-${names[$i]}: $(tail -1 "$LOG/dz_${names[$i]}.log" | cut -c1-60)"
    else echo "dazzle-${names[$i]}: FAIL — $LOG/dz_${names[$i]}.log"; rc=1; fi
  done
  [ $rc = 0 ] && echo "dazzle: all 14 suites PASS"
  return $rc
}

lane_ports() {
  ulimit -s 65520
  local rc=0
  if step dazzle-build env DAZZLE_PREBUILT= tests/dazzle/build-cli.sh "$LOG/dazzle" "$BIN"; then
    step dazzle dazzle_all || rc=1
  else
    rc=1
  fi
  if step onsgmls-build tests/sgml/build-onsgmls.sh; then
    # On Windows /tmp/scaly-onsgmls is a bash front-end over
    # tests/win32/lf-wrapper.sh; the driver starts the native binary itself
    # (no CR to strip since the streams are binary): 45 s against 1.7 s.
    local sgml_bin=/tmp/scaly-onsgmls
    [ -f /tmp/scaly-onsgmls-native.exe ] && sgml_bin=/tmp/scaly-onsgmls-native.exe
    step sgml env SCALY_POISON=1 tests/sgml/run.sh "$sgml_bin" || rc=1
  else
    rc=1
  fi
  step opensp tests/opensp/run.sh || rc=1
  step http tests/http/run.sh "$BIN" || rc=1
  step json tests/json/run.sh "$BIN" || rc=1
  step compress tests/compress/run.sh "$BIN" || rc=1
  step tls tests/tls/run.sh "$BIN" || rc=1
  step h3 tests/h3/run.sh "$BIN" || rc=1
  step pg tests/pg/run.sh "$BIN" || rc=1
  step redis tests/redis/run.sh "$BIN" || rc=1
  step tool tests/tool/run.sh "$BIN" || rc=1
  step cmscratch tests/sgml/cmscratch/run.sh || rc=1
  return $rc
}

tscaly_stage() {
  if [ "$QUICK" = 1 ]; then
    SCALY_POISON=1 packages/tscaly/tests/run.sh
  else
    SCALY_POISON=1 TSCALY_STAGE=2 packages/tscaly/tests/run.sh
  fi
}
tscaly_case() {  # tscaly_case caseerrors|casejs: rc AND a zero CRASH count
  SCALY_POISON=1 python3 packages/tscaly/tests/$1.py --jobs 8 || return 1
}
tscaly_cases() {
  local rc=0 p1 p2 c
  tscaly_case caseerrors > "$LOG/caseerrors.log" 2>&1 & p1=$!
  tscaly_case casejs > "$LOG/casejs.log" 2>&1 & p2=$!
  wait $p1 || rc=1
  wait $p2 || rc=1
  for c in caseerrors casejs; do
    echo "$c: $(grep -E '^  (MATCH|FAIL|CRASH) ' "$LOG/$c.log" | tr -s ' ' | tr '\n' ' ')"
    grep -qE '^  CRASH +[1-9]' "$LOG/$c.log" && rc=1
  done
  return $rc
}

lane_tscaly() {
  ulimit -s 65520
  step tscaly tscaly_stage || return 1
  if grep -qE 'UNEXPLAINED +[1-9]' "$LOG/tscaly.log"; then echo "tscaly: UNEXPLAINED units"; return 1; fi
  [ "$QUICK" = 1 ] && return 0
  step tscaly-cases tscaly_cases
}

vscode_run() {
  ( cd "${BAR_VSCODE_DIR:-$(dirname "$BAR_VSCODE_SCENARIO")}" && TSCALY_CHECKERS=1 SCALY_POISON=1 \
      "$LOG/tscaly_bench" --bench-batch "$BAR_VSCODE_SCENARIO" ) > "$LOG/vscode.out" 2> "$LOG/vscode.err" || return 1
  local n=${BAR_VSCODE_LINES:-$(wc -l < "$BAR_VSCODE_BASELINE")}
  if diff -q <(head -"$n" "$BAR_VSCODE_BASELINE") <(head -"$n" "$LOG/vscode.out") > /dev/null; then
    echo "vscode: first $n lines identical to the baseline"
  else
    echo "vscode: DIFFERS from the baseline"; diff <(head -"$n" "$BAR_VSCODE_BASELINE") <(head -"$n" "$LOG/vscode.out") | head -6
    return 1
  fi
}
lane_vscode() {
  ulimit -s 65520
  step vscode-build packages/tscaly/tools/bench/build.sh "$LOG/tscaly_bench" "$BIN" || return 1
  step vscode vscode_run
}

# Memory beside the run: the tscaly compilations take 6.5 GB each and a machine
# that swaps measures nothing. One line every 10 s: time, swap used, free pages
# (macOS) or available memory (Linux, /proc/meminfo — sysctl vm.swapusage and
# vm_stat exist only on macOS and left the file empty there).
mem_line() {
  if [ -r /proc/meminfo ]; then
    awk '/^SwapTotal:/ {t=$2} /^SwapFree:/ {f=$2} /^MemAvailable:/ {a=$2}
         END {printf "swap_used=%.2fM avail=%.0fM", (t-f)/1024, a/1024}' /proc/meminfo
  else
    echo "$(sysctl -n vm.swapusage 2>/dev/null | awk '{print "swap_used=" $6}') $(vm_stat 2>/dev/null | awk '/Pages free/ {print "free_pages=" $3}')"
  fi
}
( while :; do
    echo "$(( $(date +%s) - T0 ))s $(mem_line)"
    sleep 10
  done ) > "$LOG/memory.txt" 2>&1 & MEM_PID=$!

declare -a LANES=(compiler lsp ports tscaly)
if [ "$QUICK" = 0 ] && [ -n "${BAR_VSCODE_SCENARIO:-}" ] && [ -n "${BAR_VSCODE_BASELINE:-}" ]; then
  LANES+=(vscode)
fi
declare -a PIDS=()
for l in "${LANES[@]}"; do
  ( st=$(date +%s); lane_$l > "$LOG/lane_$l.txt" 2>&1; rc=$?
    echo "== lane $l: $([ $rc = 0 ] && echo ok || echo FAILED) in $(( $(date +%s) - st ))s" >> "$LOG/lane_$l.txt"
    exit $rc ) &
  PIDS+=($!)
done
RC=0
for i in "${!PIDS[@]}"; do wait "${PIDS[$i]}" || RC=1; done

# The TIMING-bound suite runs after the lanes, alone: tests/cluster waits at
# most ~10 s for a peer's marker ("S: linked"), and beside the lanes a peer
# missed it — a parity rc 110 in one run, a survivor waiting forever in the
# next — where it passes in 2-5 s on its own.
step cluster tests/cluster/run.sh "$BIN" > "$LOG/lane_tail.txt" || RC=1
kill $MEM_PID 2>/dev/null

for l in "${LANES[@]}"; do cat "$LOG/lane_$l.txt"; done
cat "$LOG/lane_tail.txt"
echo "bar: phase 1 $(( T1 - T0 ))s, phase 2 $(( $(date +%s) - T1 ))s, total $(( $(date +%s) - T0 ))s — $([ $RC = 0 ] && echo PASS || echo FAIL)"
echo "bar: peak swap $(awk '{print $2}' "$LOG/memory.txt" | sort -t '=' -k2 -n | tail -1)  (logs: $LOG)"
exit $RC
