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

# the screen's commands, with this tree's programs for scaly and scalyc
run_transcript() {
  local dir=$1 transcript=$2 work=$3 cmd="" want="" rc=0
  mkdir -p "$work"
  check() {
    [ -n "$cmd" ] || return 0
    local file got
    file=$(printf '%s\n' "$cmd" | tr ' ' '\n' | grep '\.scaly$' | head -1)
    [ -n "$file" ] && cp "$dir/prog.scaly" "$work/$file"
    # assignments in front (SCALY_WORKERS=1) stay, the program is this tree's
    local env="" run="$cmd"
    while [[ "$run" =~ ^([A-Z_]+=[^ ]*)\ (.*)$ ]]; do
      env+="${BASH_REMATCH[1]} "; run=${BASH_REMATCH[2]}
    done
    case "$run" in
      "scalyc "*) run="\"$BIN\" ${run#scalyc }" ;;
      "scaly "*) run="\"$SCALY\" ${run#scaly }" ;;
    esac
    run="$env$run"
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
