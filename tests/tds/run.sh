#!/bin/bash
# tests/tds/run.sh [compiler] — the tds package
# (Microsoft SQL Server's protocol): build every test program here with `scaly
# build` , run it under poison, and compare stdout with its
# "; Expected:" line.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

# A SQL Server to speak to: without one the suite SKIPs by name -- never a
# vacuous pass. For one in Docker:
#   docker run -d --rm --platform linux/amd64 -e ACCEPT_EULA=Y -e MSSQL_SA_PASSWORD='Throwaway!1' \
#     -p 127.0.0.1:51433:1433 mcr.microsoft.com/mssql/server:2022-latest
#   TDSTEST_PORT=51433 TDSTEST_PASSWORD='Throwaway!1' tests/tds/run.sh
if [ -z "${TDSTEST_PORT:-}" ] || [ -z "${TDSTEST_PASSWORD:-}" ]; then
  echo "tds: SKIP (no SQL Server: TDSTEST_PORT and TDSTEST_PASSWORD name one on 127.0.0.1)"
  exit 0
fi

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0; failures=()
for f in tests/tds/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  bin="$TMP/$t$SCALY_EXE"
  if ! "$SCALY" build "$f" -o "$bin" > "$TMP/$t.log" 2>&1; then
    fail=$((fail+1)); failures+=("$t(compile): $(head -1 "$TMP/$t.log")"); continue
  fi
  out=$(SCALY_POISON=1 "$bin" 2>"$TMP/$t.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "$expected" ]; then
    pass=$((pass+1))
  else
    fail=$((fail+1)); failures+=("$t(rc=$rc): '$out' $(head -1 "$TMP/$t.err")")
  fi
done
echo "tds: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
