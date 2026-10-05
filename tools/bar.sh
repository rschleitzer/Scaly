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
#             --check — last, and only once the tscaly lane's first step is
#             over, so its 6.5 GB tscaly compilation does not overlap the one
#             that lane starts with
#   lsp       tests/lsp/run.sh — once the tscaly lane has compiled its package
#   ports     likewise; dazzle and opensp from THEIR repository (../dazzle or
#             $DAZZLE_REPO, SKIPPED by name without it): its tests/run.sh with
#             this tree's compiler, then the codegen gate here; then http,
#             json, compress, tls, h3, pg, redis, tool
#   tscaly    the TypeScript port, a repository of its own since 2026-10-05
#             (github.com/rschleitzer/tscaly), expected beside this one or at
#             $TSCALY_REPO and SKIPPED by name when it is not there: its
#             tests/run.sh at stage 2 (the corpus contains stage 1's) with THIS
#             tree's compiler, its interface check, then the two case
#             yardsticks side by side — the longest lane. tools/tscaly.pin
#             names the commit of it this tree was last green with
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
# the build cache of the one tool (`scaly build`/`test`):
# one per run, beside the logs -- every suite of the run shares it, and the
# user's ~/.scaly/cache does not collect an entry per compiler the bar builds
export SCALY_CACHE="$LOG/cache"
mkdir -p "$LOG"
BIN=$ROOT/scalyc/build/scalyc
T0=$(date +%s)

# tscaly: the other repository, tested with this tree's compiler. SCALY_HOME is
# this tree, which does not hold the package tscaly, so the compiler takes it
# from the project it runs in (Modeler.package_directory#).
TSCALY=${TSCALY_REPO:-$ROOT/../tscaly}
TSCALY_LIB=/tmp/libscaly.a
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) TSCALY_LIB=/tmp/libscaly.lib ;; esac
tscaly_env() {  # tscaly_env <command of the tscaly repository> [args]
  ( cd "$TSCALY" && env SCALY_POISON=1 SCALYC="$BIN" LIBSCALY="$TSCALY_LIB" SCALY_HOME="$ROOT" \
      TSCALY_WIN_ENV="$ROOT/tools/win-env.sh" "$@" )
}
have_tscaly() { [ -x "$TSCALY/packages/tscaly/tests/run.sh" ]; }

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

# The tscaly lane opens with the largest compilation of the bar (the package,
# 6.5 GB), and the lsp and ports lanes open with their own largest: three
# whole-tree scalyls at ~2 GB each and the dazzle LTO build at 3 GB. All at
# once that is past 16 GB (measured 2026-10-03 under a 16 GB cap: the package
# compile, the dazzle build and three scalyls killed in the same second). So
# those two lanes wait for the package compile: the suite redirects its first
# PROGRAM build into tscaly_tokens-build.log the moment the package object is
# done, and the `tscaly` step's own marker covers a suite that stopped before
# it built anything. They are a tenth of the tscaly lane, so the wait is free.
await_tscaly_build() {
  while [ ! "$TSCALY/packages/tscaly/tests/out/tscaly_tokens-build.log" -nt "$LOG/phase1.txt" ] \
        && [ ! -e "$LOG/tscaly.stepped" ]; do sleep 2; done
}

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
  # ★"Last" was not enough: on a ten-core box this lane is here after 36 s,
  # while the tscaly lane is still compiling — two 6.5 GB compilations beside
  # the dazzle LTO build and the LSP suite, and 18 GB did not hold them
  # (2026-10-03). The marker is written when that lane's `tscaly` step returns,
  # passed or not; what runs beside this step then is the case yardsticks, and
  # the tscaly lane stays the longest, so the wait costs the bar nothing.
  while [ ! -e "$LOG/tscaly.stepped" ]; do sleep 2; done
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
  await_tscaly_build
  step lsp env SCALYLS_BUDGET_MS=300000 SCALYLS_PREBUILT="$ROOT/scalyc/build/scalyls" tests/lsp/run.sh "$BIN"
}

# dazzle and opensp: the other repository (github.com/rschleitzer/dazzle,
# expected beside this one or at $DAZZLE_REPO), tested with this tree's
# compiler. Its own entry script builds both programs once and runs every
# suite of the engine, the SGML corpus and the parser's unit tests.
DAZZLE_REPO=${DAZZLE_REPO:-$ROOT/../dazzle}
have_dazzle() { [ -x "$DAZZLE_REPO/tests/run.sh" ]; }
dazzle_env() {  # dazzle_env <command of the dazzle repository> [args]
  ( cd "$DAZZLE_REPO" && env SCALY_POISON=1 SCALYC="$BIN" LIBSCALY="$TSCALY_LIB" SCALY_HOME="$ROOT" "$@" )
}

lane_ports() {
  ulimit -s 65520
  local rc=0
  await_tscaly_build
  if have_dazzle; then
    echo "dazzle: $DAZZLE_REPO at $(git -C "$DAZZLE_REPO" rev-parse --short=9 HEAD 2>/dev/null)"
    step dazzle dazzle_env tests/run.sh || rc=1
    # the gate that stays here: the engine reproduces THIS tree's generated files
    step dazzle-codegen env DAZZLE_REPO="$DAZZLE_REPO" tests/dazzle/codegen/run.sh "$BIN" || rc=1
  else
    echo "SKIP dazzle, dazzle-codegen (no checkout at $DAZZLE_REPO — git clone https://github.com/rschleitzer/dazzle there, or set DAZZLE_REPO)"
  fi
  step http tests/http/run.sh "$BIN" || rc=1
  step json tests/json/run.sh "$BIN" || rc=1
  step compress tests/compress/run.sh "$BIN" || rc=1
  step tls tests/tls/run.sh "$BIN" || rc=1
  step h3 tests/h3/run.sh "$BIN" || rc=1
  step pg tests/pg/run.sh "$BIN" || rc=1
  step redis tests/redis/run.sh "$BIN" || rc=1
  step tool tests/tool/run.sh "$BIN" || rc=1
  return $rc
}

tscaly_stage() {
  if [ "$QUICK" = 1 ]; then
    tscaly_env packages/tscaly/tests/run.sh
  else
    tscaly_env env TSCALY_STAGE=2 packages/tscaly/tests/run.sh
  fi
}
tscaly_case() {  # tscaly_case caseerrors|casejs: rc AND a zero CRASH count
  tscaly_env python3 packages/tscaly/tests/$1.py --jobs 8 || return 1
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
  local rc=0
  if ! have_tscaly; then
    : > "$LOG/tscaly.stepped"
    echo "SKIP tscaly (no checkout at $TSCALY — git clone https://github.com/rschleitzer/tscaly there, or set TSCALY_REPO)"
    return 0
  fi
  local at pin
  at=$(git -C "$TSCALY" rev-parse HEAD 2>/dev/null); pin=$(cat tools/tscaly.pin 2>/dev/null)
  echo "tscaly: $TSCALY at ${at:0:9}$([ "$at" = "$pin" ] || echo " (tools/tscaly.pin names ${pin:0:9})")"
  step tscaly tscaly_stage || rc=1
  # its generated interface, with this tree's compiler — before the marker: it
  # is a 6.5 GB compilation like the one the compiler lane waits to start
  step tscaly-interface tscaly_env packages/tscaly/tools/interface.sh --check || rc=1
  : > "$LOG/tscaly.stepped"   # lane_compiler's interfaces step waits for this
  [ $rc = 0 ] || return 1
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
  have_tscaly || { echo "SKIP vscode (no tscaly checkout at $TSCALY)"; return 0; }
  step vscode-build tscaly_env packages/tscaly/tools/bench/build.sh "$LOG/tscaly_bench" "$BIN" || return 1
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
# missed it — a survivor waiting forever — where it passes in 2-5 s on its
# own. (The `train parity … rrc=110` that came and went here, alone too, was
# no timing of the test: a frame delivered after the local close, fixed in
# the runtime on 2026-10-05 — Cluster.dispatch#, gate tests/cluster/lateframe.)
step cluster tests/cluster/run.sh "$BIN" > "$LOG/lane_tail.txt" || RC=1
kill $MEM_PID 2>/dev/null

for l in "${LANES[@]}"; do cat "$LOG/lane_$l.txt"; done
cat "$LOG/lane_tail.txt"
echo "bar: phase 1 $(( T1 - T0 ))s, phase 2 $(( $(date +%s) - T1 ))s, total $(( $(date +%s) - T0 ))s — $([ $RC = 0 ] && echo PASS || echo FAIL)"
echo "bar: peak swap $(awk '{print $2}' "$LOG/memory.txt" | sort -t '=' -k2 -n | tail -1)  (logs: $LOG)"
exit $RC
