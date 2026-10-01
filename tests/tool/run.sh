#!/bin/bash
# tests/tool/run.sh [compiler] — the one tool (ROADMAP-public.md, stage A):
# `scaly build` and `scaly run` with no script, archive or -L behind them.
#   build      tests/tool/hello.scaly built against the package objects of a
#              fresh build cache, run with arguments
#   cached     a second build compiles nothing: no cache file newer than the
#              first build
#   run        the same program through `scaly run`, arguments handed on;
#              tests/tool/globals.scaly, whose narrow constants aborted the JIT
#   native     the cache holds the C and assembly files THIS target takes,
#              chosen by name: one fcontext for this architecture, no
#              `_windows` file on a POSIX host (eio_windows.c there instead)
#   transitive a program declaring only `package https` compiles: http, tls and
#              scaly come in through https's own declarations
#   http       every tests/http program built with `scaly build` and run
#              under poison against its "; Expected:" line
#   release    `scaly build --release`: hello and the http server as ONE module
#              (package bitcode from the cache, linked and optimised in
#              process), the binary smaller than the plain build's
#   target     `--target x86_64-apple-darwin`, plain and --release: an x86_64
#              binary that runs under Rosetta 2; SKIPs by name elsewhere
#   pgo        `--pgo-train`, a run, `--pgo <profile>`: the same output
#   repl       `scaly` alone: tests/tool/repl.session piped in under poison
#              gives tests/tool/repl.expected byte for byte -- values kept,
#              a var changed by `set`, a record, an Array grown in a loop, a
#              braceless function, a float, an error with its caret, `:where`
#              for a scalar, a String, an Array grown later and a large one
#              and a record shown field by field, one nested in another;
#              `:time` and `:ir` by the shape of their output
#   repl-pty   the same binary at a TERMINAL (tests/tool/repl_pty.py drives a
#              pty): the line editor, its history file, a second session;
#              SKIPs by name on Windows
#   test       `scaly test`: tests/tool/sums.scaly names its failing test and
#              answers rc 1, a filter selects, the file's own statements do
#              not run; opensp's forty test functions pass
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
  out=$("$BIN" run tests/tool/globals.scaly 2>&1)
  [ "$out" = "18 7 5" ] && ok || bad "run globals: got '$(echo "$out" | tail -1)'"
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

# test
if scaly_jit_available 2>/dev/null; then
  out=$("$BIN" test tests/tool/sums.scaly 2>&1); rc=$?
  if [ "$rc" = 1 ] && echo "$out" | grep -q '^test test_wrong \.\.\. FAIL (answered 2)$' \
     && echo "$out" | grep -q '^1 of 3 failed$' && ! echo "$out" | grep -q 'the program itself'; then
    ok
  else
    bad "test: rc=$rc '$(echo "$out" | tr '\n' '|')'"
  fi
  out=$("$BIN" test tests/tool/sums.scaly sum 2>&1); rc=$?
  [ "$rc" = 0 ] && [ "$out" = "$(printf 'test test_sum ... ok\n1 passed')" ] && ok || bad "test filter: rc=$rc '$out'"
  out=$("$BIN" test packages/opensp/0.1.0/opensp.scaly 2>&1); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q '^40 passed$' && ok || bad "test opensp: rc=$rc $(echo "$out" | tail -1)"
fi

# no-tests method: a method named `test` is not a test
if "$BIN" build tests/tool/testmethod.scaly --no-tests -o "$TMP/testmethod$SCALY_EXE" > "$TMP/testmethod.log" 2>&1; then
  out=$("$TMP/testmethod$SCALY_EXE")
  [ "$out" = "PASS" ] && ok || bad "no-tests method: got '$out'"
else
  bad "no-tests method: $(tail -2 "$TMP/testmethod.log" | tr '\n' ' ')"
fi

# release: the whole program as one module -- the same output, and a binary
# that holds what the program uses instead of the stdlib's whole object; a
# program over a package (the http server) under poison
if "$BIN" build tests/tool/hello.scaly --release -o "$TMP/hello_rel$SCALY_EXE" > "$TMP/rel.log" 2>&1; then
  out=$("$TMP/hello_rel$SCALY_EXE" one two)
  plain=$(wc -c < "$TMP/hello$SCALY_EXE"); rel=$(wc -c < "$TMP/hello_rel$SCALY_EXE")
  [ "$out" = "hello one two" ] && [ "$rel" -lt "$plain" ] && ok || bad "release: got '$out', $rel bytes against $plain"
else
  bad "release: rc=$? $(tail -3 "$TMP/rel.log" | tr '\n' ' ')"
fi
if "$BIN" build tests/http/server.scaly --release -o "$TMP/server_rel$SCALY_EXE" > "$TMP/rel2.log" 2>&1; then
  out=$(SCALY_POISON=1 "$TMP/server_rel$SCALY_EXE" 2>&1)
  [ "$out" = "$(sed -n 's/^; Expected: //p' tests/http/server.scaly)" ] && ok || bad "release http: '$out'"
else
  bad "release http(build): $(tail -1 "$TMP/rel2.log")"
fi

# target: `scaly build --target` links for the target as well -- an x86_64
# macOS program on an arm64 Mac, run under Rosetta 2 (the probe is
# tests/target/rosetta.sh's: only running an x86_64 binary says it can run)
echo 'int main(void){return 42;}' > "$TMP/probe.c"
if [ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] \
   && clang -arch x86_64 "$TMP/probe.c" -o "$TMP/probe" 2> /dev/null && { "$TMP/probe"; [ $? = 42 ]; }; then
  for mode in "" --release; do
    if "$BIN" build tests/tool/hello.scaly $mode --target x86_64-apple-darwin -o "$TMP/hello_x86" > "$TMP/x86.log" 2>&1; then
      out=$("$TMP/hello_x86" one two)
      [ "$out" = "hello one two" ] && file "$TMP/hello_x86" | grep -q 'x86_64' && ok || bad "target $mode: '$out', $(file -b "$TMP/hello_x86")"
    else
      bad "target $mode(build): $(grep -v 'ld: warning' "$TMP/x86.log" | tail -2 | tr '\n' ' ')"
    fi
  done
else
  echo "SKIP target (needs an arm64 Mac with Rosetta 2 and a universal SDK)"
fi

# pgo: an instrumented build, its run writes a profile, the build with that
# profile gives the same output. Needs the LLVM the compiler is linked against
# as tools too (its clang for the profile runtime, llvm-profdata); SKIPs by
# name where they are not installed.
(
  . tools/llvm-env.sh > /dev/null 2>&1
  [ -x "${LLVM_PREFIX:-/nonexistent}/bin/llvm-profdata" ] || command -v "llvm-profdata-${LLVM_MAJOR:-0}" > /dev/null 2>&1
) && have_pgo=1 || have_pgo=0
if [ "$SCALY_COFF" = 1 ] || [ "$have_pgo" = 0 ]; then
  echo "SKIP pgo (needs LLVM's clang and llvm-profdata)"
elif "$BIN" build tests/tool/hello.scaly --pgo-train -o "$TMP/hello_train" > "$TMP/pgo.log" 2>&1 \
     && LLVM_PROFILE_FILE="$TMP/hello.profraw" "$TMP/hello_train" one two > /dev/null 2>&1 \
     && [ -s "$TMP/hello.profraw" ] \
     && "$BIN" build tests/tool/hello.scaly --pgo "$TMP/hello.profraw" -o "$TMP/hello_pgo" >> "$TMP/pgo.log" 2>&1; then
  out=$("$TMP/hello_pgo" one two)
  [ "$out" = "hello one two" ] && ok || bad "pgo: got '$out'"
else
  bad "pgo: $(tail -3 "$TMP/pgo.log" | tr '\n' ' ')"
fi

# repl
if scaly_jit_available 2>/dev/null; then
  SCALY_POISON=1 "$BIN" < tests/tool/repl.session > "$TMP/repl.out" 2>&1
  if cmp -s "$TMP/repl.out" tests/tool/repl.expected; then
    ok
  else
    bad "repl: $(diff tests/tool/repl.expected "$TMP/repl.out" | head -5 | tr '\n' '|')"
  fi
fi

# repl :time and :ir -- their output is not the same twice, so by its shape:
# a time line, and main's definition holding the entry's addition and only the
# one session value the entry names
if scaly_jit_available 2>/dev/null; then
  printf 'let x 40\nlet y 2\nx + 2\n:time\n:ir\n' | SCALY_POISON=1 "$BIN" > "$TMP/repl2.out" 2>&1
  if grep -Eq 'ran in [0-9]+\.[0-9]{3} ms; reading, planning and compiling took [0-9]+\.[0-9]{3} ms' "$TMP/repl2.out" \
     && grep -q 'define i64 @main(' "$TMP/repl2.out" && grep -Eq 'add i64 %[a-z0-9.]+, 2' "$TMP/repl2.out" \
     && [ "$(grep -c 'unwrap.trap:' "$TMP/repl2.out")" = 1 ]; then
    ok
  else
    bad "repl :time/:ir: $(tail -3 "$TMP/repl2.out" | tr '\n' '|')"
  fi
fi

# repl at a terminal: the line editor through a pty (python's, POSIX only)
if scaly_jit_available 2>/dev/null; then
  if [ "$SCALY_COFF" = 1 ] || ! command -v python3 > /dev/null 2>&1; then
    echo "SKIP repl-pty (needs a pty and python3)"
  else
    out=$(python3 tests/tool/repl_pty.py "$BIN" "$TMP" 2>&1); rc=$?
    [ "$rc" = 0 ] && ok || bad "repl-pty: $(echo "$out" | tail -2 | tr '\n' '|')"
  fi
fi

echo "tool: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
