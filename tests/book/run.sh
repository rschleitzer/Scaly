#!/bin/bash
# tests/book/run.sh [compiler] -- the programs of the book's part on scaling
# run and print what the book says they print. Every listing of those
# chapters stands with its output (tests/book/listings.py says how a screen
# is read): a plain screen is checked against `scaly run` of the program --
# twice, with the pool and with SCALY_WORKERS=1, because what a program
# prints may not depend on how many cores it ran on --, a transcript is run
# command by command. Times never stand in a screen; they stand in the text.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

CHAPTERS=(docs/scaly/chapters/scaling-*.xml)
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
export SCALY_CACHE="$TMP/cache"
export SCALY_HOME="$ROOT"

python3 "$HERE/listings.py" "$TMP/listings" "${CHAPTERS[@]}"; lrc=$?
pass=0; fail=0
[ "$lrc" = 0 ] || fail=$((fail+1))

# the screen's commands, with this tree's programs for scaly and scalyc; a
# command ending in " &" runs in the background -- the nodes of a cluster
# beside the one in front -- prints nothing in the book, and must have ended
# with rc 0 and no output when the transcript ends (BG_SECONDS at most)
BG_SECONDS=${BG_SECONDS:-60}
run_transcript() {
  local dir=$1 transcript=$2 work=$3 cmd="" want="" rc=0
  local bg_pids=() bg_cmds=() bg_logs=()
  mkdir -p "$work"
  check() {
    [ -n "$cmd" ] || return 0
    local file got bg=0
    file=$(printf '%s\n' "$cmd" | tr ' ' '\n' | grep '\.scaly$' | head -1)
    [ -n "$file" ] && cp "$dir/prog.scaly" "$work/$file"
    # assignments in front (SCALY_WORKERS=1) stay, the program is this tree's
    local env="" run="$cmd"
    if [[ "$run" == *" &" ]]; then bg=1; run=${run% &}; fi
    while [[ "$run" =~ ^([A-Z_]+=[^ ]*)\ (.*)$ ]]; do
      env+="${BASH_REMATCH[1]} "; run=${BASH_REMATCH[2]}
    done
    case "$run" in
      "scalyc "*) run="\"$BIN\" ${run#scalyc }" ;;
      "scaly "*) run="\"$SCALY\" ${run#scaly }" ;;
    esac
    run="$env$run"
    if [ "$bg" = 1 ]; then
      local log="$work/bg-${#bg_pids[@]}.log"
      (cd "$work" && exec bash -c "$run") < /dev/null > "$log" 2>&1 &
      bg_pids+=($!); bg_cmds+=("$cmd"); bg_logs+=("$log")
      if [ -n "$(printf '%s' "$want" | tr -d '[:space:]')" ]; then
        echo "FAIL $(basename "$dir"): \$ $cmd -- a command in the background shows no output"
        rc=1
      fi
      return 0
    fi
    got=$(cd "$work" && bash -c "$run" < /dev/null 2>&1); local crc=$?
    if [ "$crc" != 0 ] || [ "$got" != "$(printf '%s' "$want" | sed '$ { /^$/ d; }')" ]; then
      echo "FAIL $(basename "$dir"): \$ $cmd (rc=$crc)"
      diff <(printf '%s\n' "$want" | sed '$ { /^$/ d; }') <(printf '%s\n' "$got") | head -8 | sed 's/^/    /'
      rc=1
    fi
  }
  while IFS= read -r line || [ -n "$line" ]; do
    if [ "${line#\$ }" != "$line" ]; then
      check
      cmd=${line#\$ }; want=""
    else
      want+="$line"$'\n'
    fi
  done < "$transcript"
  check
  local k t
  for k in "${!bg_pids[@]}"; do
    t=0
    while kill -0 "${bg_pids[$k]}" 2>/dev/null && [ $t -lt $((BG_SECONDS * 10)) ]; do
      sleep 0.1; t=$((t + 1))
    done
    if kill -0 "${bg_pids[$k]}" 2>/dev/null; then
      kill -9 "${bg_pids[$k]}" 2>/dev/null
      echo "FAIL $(basename "$dir"): \$ ${bg_cmds[$k]} -- still running after ${BG_SECONDS} s"
      rc=1
    fi
    wait "${bg_pids[$k]}"; local brc=$?
    if [ "$brc" != 0 ] || [ -s "${bg_logs[$k]}" ]; then
      echo "FAIL $(basename "$dir"): \$ ${bg_cmds[$k]} (rc=$brc)"
      head -8 "${bg_logs[$k]}" | sed 's/^/    /'
      rc=1
    fi
  done
  return $rc
}

for d in "$TMP"/listings/*/; do
  d=${d%/}; name=$(basename "$d")
  ok=1
  for t in "$d"/transcript-*.txt; do
    [ -f "$t" ] || continue
    run_transcript "$d" "$t" "$TMP/work/$name" || ok=0
  done
  if [ ! -f "$d/expected.txt" ]; then
    if [ "$ok" = 1 ]; then pass=$((pass+1)); else fail=$((fail+1)); fi
    continue
  fi
  for workers in pool 1; do
    [ "$ok" = 1 ] || break
    if [ "$workers" = pool ]; then
      got=$(env -u SCALY_WORKERS "$SCALY" run "$d/prog.scaly" < /dev/null 2>&1); rc=$?
    else
      got=$(SCALY_WORKERS=$workers "$SCALY" run "$d/prog.scaly" < /dev/null 2>&1); rc=$?
    fi
    if [ "$rc" != 0 ] || [ "$got" != "$(cat "$d/expected.txt")" ]; then
      echo "FAIL $name (SCALY_WORKERS=$workers, rc=$rc)"
      diff "$d/expected.txt" <(printf '%s\n' "$got") | head -8 | sed 's/^/    /'
      ok=0
      break
    fi
  done
  if [ "$ok" = 1 ]; then pass=$((pass+1)); else fail=$((fail+1)); fi
done
echo "book: $pass PASS, $fail FAIL"
[ "$fail" = 0 ]
