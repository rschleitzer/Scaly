#!/bin/bash
# tests/tool/run.sh [compiler] — the one tool (ROADMAP-public.md, stage A):
# `scaly build` and `scaly run` with no script, archive or -L behind them.
#   build      tests/tool/hello.scaly built against the package objects of a
#              fresh build cache, run with arguments
#   cached     a second build compiles nothing: no cache file newer than the
#              first build
#   run        the same program through `scaly run`, arguments handed on
#   native     the cache holds the C and assembly files THIS target takes,
#              chosen by name: one fcontext for this architecture, no
#              `_windows` file on a POSIX host (eio_windows.c there instead)
#   transitive a program declaring only `package https` compiles: http, tls and
#              scaly come in through https's own declarations
#   http       every tests/http program built with `scaly build` and run
#              under poison against its "; Expected:" line
# The cache lives in a scratch directory (SCALY_CACHE); nothing is written
# into the tree.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
cd "$ROOT"

. tests/platform.sh || exit 1
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
export SCALY_CACHE="$TMP/cache"

pass=0; fail=0; failures=()
ok() { pass=$((pass+1)); }
bad() { fail=$((fail+1)); failures+=("$1"); }

# build
if "$BIN" build tests/tool/hello.scaly -o "$TMP/hello$SCALY_EXE" > "$TMP/build.log" 2>&1; then
  out=$("$TMP/hello$SCALY_EXE" one two)
  [ "$out" = "hello one two" ] && ok || bad "build: got '$out'"
else
  bad "build: rc=$? $(tail -3 "$TMP/build.log" | tr '\n' ' ')"
fi

# cached
touch "$TMP/marker"
sleep 1
if "$BIN" build tests/tool/hello.scaly -o "$TMP/hello2$SCALY_EXE" > "$TMP/build2.log" 2>&1; then
  newer=$(find "$SCALY_CACHE" -type f -newer "$TMP/marker" | wc -l | tr -d ' ')
  [ "$newer" = 0 ] && ok || bad "cached: $newer cache files rewritten"
else
  bad "cached: rc=$?"
fi

# run
if scaly_jit_available 2>/dev/null; then
  out=$("$BIN" run tests/tool/hello.scaly three 2>&1)
  [ "$out" = "hello three" ] && ok || bad "run: got '$out'"
fi

# native
fc=$(ls "$SCALY_CACHE" | grep -c 'fcontext')
win=$(ls "$SCALY_CACHE" | grep -c '_windows')
arch=$(uname -m)
case "$arch" in aarch64) arch=arm64 ;; amd64) arch=x86_64 ;; esac
if [ "$SCALY_COFF" = 1 ]; then
  [ "$fc" = 1 ] && ls "$SCALY_CACHE" | grep -q 'eio_windows' && ! ls "$SCALY_CACHE" | grep -q 'eio\.c' && ok \
    || bad "native: $(ls "$SCALY_CACHE" | tr '\n' ' ')"
else
  [ "$fc" = 1 ] && [ "$win" = 0 ] && ls "$SCALY_CACHE" | grep -q "fcontext_$arch" && ok \
    || bad "native: $(ls "$SCALY_CACHE" | tr '\n' ' ')"
fi

# transitive
if "$BIN" -S -o "$TMP/transitive.ll" tests/tool/transitive.scaly > "$TMP/transitive.log" 2>&1; then
  ok
else
  bad "transitive: $(head -2 "$TMP/transitive.log" | tr '\n' ' ')"
fi

# http
for f in tests/http/*.scaly; do
  t=$(basename "$f" .scaly)
  expected=$(sed -n 's/^; Expected: //p' "$f")
  if ! "$BIN" build "$f" -o "$TMP/$t$SCALY_EXE" > "$TMP/$t.log" 2>&1; then
    bad "http $t(build): $(tail -1 "$TMP/$t.log")"; continue
  fi
  out=$(cd "$TMP" && SCALY_POISON=1 "./$t$SCALY_EXE" 2>/dev/null)
  [ "$out" = "$expected" ] && ok || bad "http $t: '$out'"
done

echo "tool: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
