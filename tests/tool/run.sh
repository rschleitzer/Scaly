#!/bin/bash
# tests/tool/run.sh [compiler] — the one tool:
# `scaly build` and `scaly run` with no script, archive or -L behind them.
#   build      tests/tool/hello.scaly built against the package objects of a
#              fresh build cache, run with arguments
#   spaced     the same build with a space in the output path
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
#   export     `--release --export`: the stdlib's definitions stay in the
#              binary as symbols its own process finds -- what a program that
#              runs others in process (the compiler, the tool) is built with;
#              without the flag the same build keeps none of them. SKIPs by
#              name on Windows (the export there is the link script's)
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
#              not run
#   project    a package the installation does not have is found in the
#              project's own ./packages (Modeler.package_directory#), one of
#              an installed name there does not shadow the installed one, and
#              a package in neither place is reported with both named
#   new        `scaly new`: the program it writes runs and passes its test,
#              the package it writes (--lib) passes its test and is found by
#              a program beside its packages directory; an existing name, a
#              name no package can have and a missing name are refused
#   collide    two packages that each define a function of one name (and a
#              record with a method of one name) are refused by `scaly run`,
#              `scaly build` and `--release` alike, both symbols named
#              (tool.check_definitions#) -- before, the three gave three
#              different answers at rc 0; with the names apart the three agree
#   source     `package name version "source"`: a package neither the
#              installation nor the project has is taken from where a fetch
#              lays it, $SCALY_PACKAGES/<host>/<path>/<name>/<version>, on
#              all three routes and under every spelling of one source; a
#              missing one is reported with source and place, a source that
#              is no string is refused, and a local ./packages wins
#   fetch      the tool fetches a declared source with git (local
#              repositories, no network): a package whose own root declares
#              a second source brings that one too; the files lie read-only
#              with commit and tree remembered beside them; a version whose
#              directory changed afterwards is refused when fetched again;
#              a source without the package, one that is no address, and a
#              repository that is not there are each said; a package with C
#              files is announced; scalyc alone fetches nothing. SKIPs by
#              name without git
#   bare       Windows the Rust way: a handed-out tree --
#              the stdlib with its READY-MADE native objects
#              (tools/native-objects.sh) -- builds a program, plain and
#              --release, with NO clang on the PATH and neither LIB nor
#              INCLUDE set: the Build Tools' link.exe is found and called by
#              the compiler itself. SKIPs by name off Windows
# The cache lives in a scratch directory (SCALY_CACHE); nothing is written
# into the tree.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BIN="${1:-$ROOT/scalyc/build/scalyc}"
# the tool beside the compiler: scaly for REPL/run/build/test, scalyc for the flags
SCALY=$("$ROOT/tools/scaly-of.sh" "$BIN")
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
if "$SCALY" build tests/tool/hello.scaly -o "$TMP/hello$SCALY_EXE" > "$TMP/build.log" 2>&1; then
  out=$("$TMP/hello$SCALY_EXE" one two)
  [ "$out" = "hello one two" ] && ok || bad "build: got '$out'"
else
  bad "build: rc=$? $(tail -3 "$TMP/build.log" | tr '\n' ' ')"
fi

# spaced: an output path with a space reaches the linker as ONE argument
mkdir -p "$TMP/out dir"
if "$SCALY" build tests/tool/hello.scaly -o "$TMP/out dir/hello$SCALY_EXE" > "$TMP/spaced.log" 2>&1; then
  out=$("$TMP/out dir/hello$SCALY_EXE" one two)
  [ "$out" = "hello one two" ] && ok || bad "spaced: got '$out'"
else
  bad "spaced: rc=$? $(tail -3 "$TMP/spaced.log" | tr '\n' ' ')"
fi

# spaced-scratch (Windows): the compiler's scratch directory comes from TMP
# there, and its object and the runtime archive are named on the link line
if [ "$SCALY_COFF" = 1 ]; then
  mkdir -p "$TMP/scratch dir"
  cp /tmp/libscaly.lib "$TMP/scratch dir/"
  if TMP="$TMP/scratch dir" TEMP="$TMP/scratch dir" "$BIN" -o "$TMP/out dir/hello_s$SCALY_EXE" tests/tool/hello.scaly > "$TMP/scratch.log" 2>&1; then
    out=$("$TMP/out dir/hello_s$SCALY_EXE" one two)
    [ "$out" = "hello one two" ] && ok || bad "spaced-scratch: got '$out'"
  else
    bad "spaced-scratch: rc=$? $(tail -3 "$TMP/scratch.log" | tr '\n' ' ')"
  fi
else
  echo "SKIP spaced-scratch (the scratch directory is /tmp off Windows)"
fi

# cached
touch "$TMP/marker"
sleep 1
if "$SCALY" build tests/tool/hello.scaly -o "$TMP/hello2$SCALY_EXE" > "$TMP/build2.log" 2>&1; then
  newer=$(find "$SCALY_CACHE" -type f -newer "$TMP/marker" | wc -l | tr -d ' ')
  [ "$newer" = 0 ] && ok || bad "cached: $newer cache files rewritten"
else
  bad "cached: rc=$?"
fi

# run
if scaly_jit_available 2>/dev/null; then
  out=$("$SCALY" run tests/tool/hello.scaly three 2>&1)
  [ "$out" = "hello three" ] && ok || bad "run: got '$out'"
  out=$("$SCALY" run tests/tool/globals.scaly 2>&1)
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
  if ! "$SCALY" build "$f" -o "$TMP/$t$SCALY_EXE" > "$TMP/$t.log" 2>&1; then
    bad "http $t(build): $(tail -1 "$TMP/$t.log")"; continue
  fi
  out=$(cd "$TMP" && SCALY_POISON=1 "./$t$SCALY_EXE" 2>/dev/null)
  [ "$out" = "$expected" ] && ok || bad "http $t: '$out'"
done

# test
if scaly_jit_available 2>/dev/null; then
  out=$("$SCALY" test tests/tool/sums.scaly 2>&1); rc=$?
  if [ "$rc" = 1 ] && echo "$out" | grep -q '^test test_wrong \.\.\. FAIL (answered 2)$' \
     && echo "$out" | grep -q '^1 of 3 failed$' && ! echo "$out" | grep -q 'the program itself'; then
    ok
  else
    bad "test: rc=$rc '$(echo "$out" | tr '\n' '|')'"
  fi
  out=$("$SCALY" test tests/tool/sums.scaly sum 2>&1); rc=$?
  [ "$rc" = 0 ] && [ "$out" = "$(printf 'test test_sum ... ok\n1 passed')" ] && ok || bad "test filter: rc=$rc '$out'"
fi

# no-tests method: a method named `test` is not a test
if "$SCALY" build tests/tool/testmethod.scaly --no-tests -o "$TMP/testmethod$SCALY_EXE" > "$TMP/testmethod.log" 2>&1; then
  out=$("$TMP/testmethod$SCALY_EXE")
  [ "$out" = "PASS" ] && ok || bad "no-tests method: got '$out'"
else
  bad "no-tests method: $(tail -2 "$TMP/testmethod.log" | tr '\n' ' ')"
fi

# release: the whole program as one module -- the same output, and a binary
# that holds what the program uses instead of the stdlib's whole object; a
# program over a package (the http server) under poison
if "$SCALY" build tests/tool/hello.scaly --release -o "$TMP/hello_rel$SCALY_EXE" > "$TMP/rel.log" 2>&1; then
  out=$("$TMP/hello_rel$SCALY_EXE" one two)
  plain=$(wc -c < "$TMP/hello$SCALY_EXE"); rel=$(wc -c < "$TMP/hello_rel$SCALY_EXE")
  [ "$out" = "hello one two" ] && [ "$rel" -lt "$plain" ] && ok || bad "release: got '$out', $rel bytes against $plain"
else
  bad "release: rc=$? $(tail -3 "$TMP/rel.log" | tr '\n' ' ')"
fi
if "$SCALY" build tests/http/server.scaly --release -o "$TMP/server_rel$SCALY_EXE" > "$TMP/rel2.log" 2>&1; then
  out=$(SCALY_POISON=1 "$TMP/server_rel$SCALY_EXE" 2>&1)
  [ "$out" = "$(sed -n 's/^; Expected: //p' tests/http/server.scaly)" ] && ok || bad "release http: '$out'"
else
  bad "release http(build): $(tail -1 "$TMP/rel2.log")"
fi

# export: a release build that keeps every definition as a visible symbol.
# Counted on a name the hello program never calls (HashMap's), so the plain
# release build has none of it.
if [ "$SCALY_COFF" = 1 ]; then
  echo "SKIP export (the Windows export is tools/win-link.sh --export)"
elif "$SCALY" build tests/tool/hello.scaly --release --export -o "$TMP/hello_exp" > "$TMP/exp.log" 2>&1; then
  out=$("$TMP/hello_exp" one two)
  kept=$(nm -g "$TMP/hello_exp" 2>/dev/null | grep -c ' [TtWw] _*_ZN7HashMap')
  plain_kept=$(nm -g "$TMP/hello_rel" 2>/dev/null | grep -c ' [TtWw] _*_ZN7HashMap')
  [ "$out" = "hello one two" ] && [ "$kept" -gt 0 ] && [ "$plain_kept" = 0 ] && ok \
    || bad "export: got '$out', $kept HashMap symbols with --export, $plain_kept without"
else
  bad "export: rc=$? $(tail -3 "$TMP/exp.log" | tr '\n' ' ')"
fi

# target: `scaly build --target` links for the target as well -- an x86_64
# macOS program on an arm64 Mac, run under Rosetta 2 (the probe is
# tests/target/rosetta.sh's: only running an x86_64 binary says it can run)
echo 'int main(void){return 42;}' > "$TMP/probe.c"
if [ "$(uname -s)" = Darwin ] && [ "$(uname -m)" = arm64 ] \
   && clang -arch x86_64 "$TMP/probe.c" -o "$TMP/probe" 2> /dev/null && { "$TMP/probe"; [ $? = 42 ]; }; then
  for mode in "" --release; do
    if "$SCALY" build tests/tool/hello.scaly $mode --target x86_64-apple-darwin -o "$TMP/hello_x86" > "$TMP/x86.log" 2>&1; then
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
# name where they are not installed. On Windows too since 2026-10-02 (the
# win-tool balloon: it was skipped there by name and had never been tried).
(
  . tools/llvm-env.sh > /dev/null 2>&1
  [ -x "${LLVM_PREFIX:-/nonexistent}/bin/llvm-profdata" ] || command -v "llvm-profdata-${LLVM_MAJOR:-0}" > /dev/null 2>&1
) && have_pgo=1 || have_pgo=0
if [ "$have_pgo" = 0 ]; then
  echo "SKIP pgo (needs LLVM's clang and llvm-profdata)"
elif "$SCALY" build tests/tool/hello.scaly --pgo-train -o "$TMP/hello_train$SCALY_EXE" > "$TMP/pgo.log" 2>&1 \
     && LLVM_PROFILE_FILE="$TMP/hello.profraw" "$TMP/hello_train$SCALY_EXE" one two > /dev/null 2>&1 \
     && [ -s "$TMP/hello.profraw" ] \
     && "$SCALY" build tests/tool/hello.scaly --pgo "$TMP/hello.profraw" -o "$TMP/hello_pgo$SCALY_EXE" >> "$TMP/pgo.log" 2>&1; then
  out=$("$TMP/hello_pgo$SCALY_EXE" one two)
  [ "$out" = "hello one two" ] && ok || bad "pgo: got '$out'"
elif grep -q 'libclang_rt.profile' "$TMP/pgo.log"; then
  echo "SKIP pgo (LLVM's profile runtime is not installed: libclang-rt-<major>-dev)"
else
  bad "pgo: $(tail -3 "$TMP/pgo.log" | tr '\n' ' ')"
fi

# repl
if scaly_jit_available 2>/dev/null; then
  SCALY_POISON=1 "$SCALY" < tests/tool/repl.session > "$TMP/repl.out" 2>&1
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
  printf 'let x 40\nlet y 2\nx + 2\n:time\n:ir\n' | SCALY_POISON=1 "$SCALY" > "$TMP/repl2.out" 2>&1
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
    out=$(python3 tests/tool/repl_pty.py "$SCALY" "$TMP" 2>&1); rc=$?
    [ "$rc" = 0 ] && ok || bad "repl-pty: $(echo "$out" | tail -2 | tr '\n' '|')"
  fi
fi

# project: a package the installation does not have is the project's own,
# ./packages/<name>/<version> under the directory the tool runs in
# (Modeler.package_directory#) -- and a project's package of an INSTALLED
# name does not get in front of the installed one
proj="$TMP/proj"
mkdir -p "$proj/packages/mine/0.1.0/mine" "$proj/packages/json/0.1.0"
cat > "$proj/packages/mine/0.1.0/mine.scaly" <<'PKG'
define mine
{
    module greet
}
PKG
cat > "$proj/packages/mine/0.1.0/mine/greet.scaly" <<'PKG'
function greeting(n: int) returns int
    n + 40
PKG
printf 'this is no package at all (\n' > "$proj/packages/json/0.1.0/json.scaly"
cat > "$proj/main.scaly" <<'PKG'
package mine 0.1.0
package json 0.1.0

print("project `greeting(2)`")
PKG
here="$PWD"
case "$SCALY" in /*) scaly_abs="$SCALY" ;; *) scaly_abs="$here/$SCALY" ;; esac
if ( cd "$proj" && SCALY_HOME="$here" "$scaly_abs" build main.scaly -o "$TMP/proj_main$SCALY_EXE" ) > "$TMP/proj.log" 2>&1; then
  out=$("$TMP/proj_main$SCALY_EXE")
  [ "$out" = "project 42" ] && ok || bad "project: got '$out'"
else
  bad "project: rc=$? $(tail -3 "$TMP/proj.log" | tr '\n' ' ')"
fi
# ... and a package neither has is reported with both places named
printf 'package nowhere 0.1.0\nprint("x")\n' > "$proj/missing.scaly"
out=$( cd "$proj" && SCALY_HOME="$here" "$scaly_abs" build missing.scaly -o "$TMP/proj_missing$SCALY_EXE" 2>&1 ); rc=$?
if [ "$rc" != 0 ] && echo "$out" | grep -q 'package not found: nowhere 0.1.0' && echo "$out" | grep -q 'in ./packages of the directory'; then
  ok
else
  bad "project-missing: rc=$rc $(echo "$out" | tail -2 | tr '\n' ' ')"
fi

# new: what `scaly new` writes is a program, or a package, that works as it is
newdir="$TMP/new"
mkdir -p "$newdir"
if ( cd "$newdir" && SCALY_HOME="$here" "$scaly_abs" new hello ) > "$TMP/new.log" 2>&1 \
   && [ "$(head -1 "$TMP/new.log")" = "created hello/hello.scaly" ]; then
  out=$(cd "$newdir/hello" && SCALY_HOME="$here" "$scaly_abs" run hello.scaly 2>&1)
  [ "$out" = "Hello, world!" ] && ok || bad "new: the program printed '$out'"
  out=$(cd "$newdir/hello" && SCALY_HOME="$here" "$scaly_abs" test hello.scaly 2>&1); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q '^1 passed$' && ok || bad "new: its test rc=$rc '$(echo "$out" | tail -1)'"
else
  bad "new: $(tr '\n' ' ' < "$TMP/new.log")"
fi
if ( cd "$newdir" && SCALY_HOME="$here" "$scaly_abs" new shapes --lib ) > "$TMP/newlib.log" 2>&1 \
   && [ -f "$newdir/shapes/packages/shapes/0.1.0/shapes.scaly" ]; then
  out=$(cd "$newdir/shapes" && SCALY_HOME="$here" "$scaly_abs" test packages/shapes/0.1.0/shapes.scaly 2>&1); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q '^1 passed$' && ok || bad "new --lib: its test rc=$rc '$(echo "$out" | tail -1)'"
  printf 'package shapes 0.1.0\nprint(shapes.greeting("user"))\n' > "$newdir/shapes/user.scaly"
  out=$(cd "$newdir/shapes" && SCALY_HOME="$here" "$scaly_abs" run user.scaly 2>&1)
  [ "$out" = "Hello, user!" ] && ok || bad "new --lib: a program beside it printed '$out'"
else
  bad "new --lib: $(tr '\n' ' ' < "$TMP/newlib.log")"
fi
refused=0
for args in "hello" "9lives" "a-b" "" "--lib"; do
  # shellcheck disable=SC2086
  ( cd "$newdir" && SCALY_HOME="$here" "$scaly_abs" new $args ) > /dev/null 2>&1 || refused=$((refused+1))
done
[ "$refused" = 5 ] && [ ! -e "$newdir/9lives" ] && [ ! -e "$newdir/a-b" ] && ok || bad "new: $refused of 5 bad invocations refused"

# collide: no symbol carries its package, so two packages defining one are
# refused -- on every route, because every route merged them differently
col="$TMP/collide"
for p in alpha beta; do
  mkdir -p "$col/packages/$p/0.1.0/$p"
  printf 'package scaly 0.1.0\n\ndefine %s\n{\n    module util\n    module Box\n}\n' "$p" > "$col/packages/$p/0.1.0/$p.scaly"
done
write_pair() {  # write_pair <helper of beta> <record of beta>
  printf 'function helper(n: int) returns int\n    n + 1\n\nfunction from_alpha(n: int) returns int\n    helper(n)\n' > "$col/packages/alpha/0.1.0/alpha/util.scaly"
  printf 'function %s(n: int) returns int\n    n + 100\n\nfunction from_beta(n: int) returns int\n    %s(n)\n' "$1" "$1" > "$col/packages/beta/0.1.0/beta/util.scaly"
  printf 'define Box\n(\n    v: int\n)\n{\n    function grown(this) returns int\n        v + 1\n}\n\nfunction alpha_box(n: int) returns int\n{\n    let b Box(n)\n    b.grown()\n}\n' > "$col/packages/alpha/0.1.0/alpha/Box.scaly"
  printf 'define %s\n(\n    v: int\n)\n{\n    function grown(this) returns int\n        v + 100\n}\n\nfunction beta_box(n: int) returns int\n{\n    let b %s(n)\n    b.grown()\n}\n' "$2" "$2" > "$col/packages/beta/0.1.0/beta/Box.scaly"
}
printf 'package alpha 0.1.0\npackage beta 0.1.0\n\nprint("`from_alpha(1)` `from_beta(1)` `alpha_box(1)` `beta_box(1)`")\n' > "$col/main.scaly"
write_pair helper Box
for route in "run main.scaly" "build main.scaly -o $TMP/col_a$SCALY_EXE" "build main.scaly --release -o $TMP/col_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$col" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  if [ "$rc" != 0 ] && echo "$out" | grep -q 'packages alpha and beta both define helper (_Z6helperi)' \
     && echo "$out" | grep -q 'both define Box.grown (_ZN3Box5grownEv)' && ! echo "$out" | grep -q '^2 '; then
    ok
  else
    bad "collide (${route%% *} ${route##*main.scaly}): rc=$rc '$(echo "$out" | head -2 | tr '\n' '|')'"
  fi
done
# ... and with the names apart the three routes give ONE answer
write_pair helper_b BoxB
for route in "run main.scaly" "build main.scaly -o $TMP/col_a$SCALY_EXE" "build main.scaly --release -o $TMP/col_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$col" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *col_a*) [ "$rc" = 0 ] && out=$("$TMP/col_a$SCALY_EXE") ;;
    *col_b*) [ "$rc" = 0 ] && out=$("$TMP/col_b$SCALY_EXE") ;;
  esac
  [ "$out" = "2 101 2 101" ] && ok || bad "collide, names apart (${route%% *}): rc=$rc got '$(echo "$out" | head -1)'"
done

# source: the third place a package is looked for, what a fetch laid down
src="$TMP/source"
fetched="$TMP/fetched"
mkdir -p "$src"
printf 'package shapes 0.1.0 "github.com/someone/shapes"\n\nprint(shapes.greeting("fetched"))\n' > "$src/main.scaly"
# (asked of scalyc, which only looks: the tool would go and fetch it)
case "$BIN" in /*) BIN_ABS="$BIN" ;; *) BIN_ABS="$here/$BIN" ;; esac
out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$BIN_ABS" -S -o "$TMP/source.ll" main.scaly 2>&1); rc=$?
if [ "$rc" != 0 ] && echo "$out" | grep -q 'package not found: shapes 0.1.0' \
   && echo "$out" | grep -q 'fetched/github.com/someone/shapes/shapes/0.1.0/shapes.scaly' \
   && echo "$out" | grep -q 'not fetched from "github.com/someone/shapes"'; then
  ok
else
  bad "source, missing: rc=$rc '$(echo "$out" | head -1 | cut -c1-160)'"
fi
( cd "$TMP" && SCALY_HOME="$here" "$scaly_abs" new shapes --lib ) > /dev/null 2>&1
mkdir -p "$fetched/github.com/someone/shapes/shapes"
cp -R "$TMP/shapes/packages/shapes/0.1.0" "$fetched/github.com/someone/shapes/shapes/"
for route in "run main.scaly" "build main.scaly -o $TMP/src_a$SCALY_EXE" "build main.scaly --release -o $TMP/src_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *src_a*) [ "$rc" = 0 ] && out=$("$TMP/src_a$SCALY_EXE") ;;
    *src_b*) [ "$rc" = 0 ] && out=$("$TMP/src_b$SCALY_EXE") ;;
  esac
  [ "$out" = "Hello, fetched!" ] && ok || bad "source (${route%% *}): rc=$rc got '$(echo "$out" | head -1 | cut -c1-120)'"
done
same=0
for spelling in 'https://github.com/someone/shapes.git' 'git@github.com:someone/shapes.git' 'ssh://git@github.com/someone/shapes/'; do
  printf 'package shapes 0.1.0 "%s"\nprint(shapes.greeting("x"))\n' "$spelling" > "$src/spelled.scaly"
  out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$scaly_abs" run spelled.scaly 2>&1)
  [ "$out" = "Hello, x!" ] && same=$((same+1))
done
[ "$same" = 3 ] && ok || bad "source: $same of 3 spellings of one source found the package"
printf 'package shapes 0.1.0 42\nprint("no")\n' > "$src/nostring.scaly"
out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$scaly_abs" run nostring.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q "is the package's source, a string" && ok || bad "source, no string: rc=$rc '$(echo "$out" | head -1 | cut -c1-120)'"
mkdir -p "$src/packages/shapes/0.1.0"
sed 's/Hello/Local hello/' "$TMP/shapes/packages/shapes/0.1.0/shapes.scaly" > "$src/packages/shapes/0.1.0/shapes.scaly"
out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$scaly_abs" run main.scaly 2>&1)
[ "$out" = "Local hello, fetched!" ] && ok || bad "source: a local ./packages did not win, got '$(echo "$out" | head -1 | cut -c1-120)'"

# fetch: git is the registry (local repositories, so no network)
if ! command -v git > /dev/null 2>&1; then
  echo "SKIP fetch (no git on the PATH)"
else
  fr="$TMP/fetchrepos"; fp="$TMP/fetchproj"; fb="$TMP/fetchbase"
  mkdir -p "$fr" "$fp"
  tg() { git -c user.name=t -c user.email=t@example.invalid -c init.defaultBranch=main "$@"; }
  ft() { ( cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" "$scaly_abs" "$@" ); }
  ( cd "$fr" && SCALY_HOME="$here" "$scaly_abs" new inner --lib ) > /dev/null 2>&1
  ( cd "$fr/inner" && tg init -q && tg add -A && tg commit -q -m one ) > /dev/null 2>&1
  mkdir -p "$fr/outer/packages/outer/0.1.0"
  printf 'package scaly 0.1.0\npackage inner 0.1.0 "%s"\n\ndefine outer\n{\n    function twice(name: String) returns String\n        inner.greeting(inner.greeting(name))\n}\n' "$fr/inner" > "$fr/outer/packages/outer/0.1.0/outer.scaly"
  printf 'int outer_c(void) { return 7; }\n' > "$fr/outer/packages/outer/0.1.0/extra.c"
  ( cd "$fr/outer" && tg init -q && tg add -A && tg commit -q -m one ) > /dev/null 2>&1
  printf 'package outer 0.1.0 "%s"\n\nprint(outer.twice("git"))\n' "$fr/outer" > "$fp/main.scaly"
  case "$BIN" in /*) BIN_ABS="$BIN" ;; *) BIN_ABS="$here/$BIN" ;; esac
  # scalyc alone looks and does not fetch
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" "$BIN_ABS" -S -o "$TMP/fetch.ll" main.scaly 2>&1); rc=$?
  [ "$rc" != 0 ] && [ ! -e "$fb" ] && echo "$out" | grep -q 'scaly build, scaly run and scaly test fetch it' && ok || bad "fetch: scalyc alone rc=$rc '$(echo "$out" | head -1 | cut -c1-120)'"
  out=$(ft run main.scaly 2> "$TMP/fetch.err"); rc=$?
  if [ "$rc" = 0 ] && [ "$out" = "Hello, Hello, git!!" ] && grep -q '^scaly: fetching outer 0.1.0 from ' "$TMP/fetch.err" \
     && grep -q '^scaly: fetching inner 0.1.0 from ' "$TMP/fetch.err" && grep -q 'outer 0.1.0 brings native code - 1 C or assembly files' "$TMP/fetch.err"; then
    ok
  else
    bad "fetch: rc=$rc out='$out' $(head -3 "$TMP/fetch.err" | tr '\n' '|' | cut -c1-200)"
  fi
  # a second run fetches nothing; build and --release use what lies there
  out=$(ft run main.scaly 2> "$TMP/fetch2.err")
  [ "$out" = "Hello, Hello, git!!" ] && [ ! -s "$TMP/fetch2.err" ] && ok || bad "fetch: the second run said '$(head -1 "$TMP/fetch2.err" | cut -c1-120)'"
  ft build main.scaly --release -o "$TMP/fetch_m$SCALY_EXE" > "$TMP/fetchb.log" 2>&1 && [ "$("$TMP/fetch_m$SCALY_EXE")" = "Hello, Hello, git!!" ] && ok || bad "fetch: --release $(tail -1 "$TMP/fetchb.log" | cut -c1-120)"
  # laid down read-only, commit and tree beside it
  fd=$(find "$fb" -type d -path '*/inner/inner/0.1.0' | head -1)
  if [ -n "$fd" ] && [ ! -w "$fd/inner.scaly" ] && grep -q '^commit [0-9a-f]\{40\}$' "$fd.fetched" && grep -q '^tree [0-9a-f]\{40\}$' "$fd.fetched"; then ok; else bad "fetch: the record or the read-only files ($fd)"; fi
  # a published version does not change
  ( cd "$fr/inner" && sed 's/Hello/Changed/' packages/inner/0.1.0/inner.scaly > x && mv x packages/inner/0.1.0/inner.scaly && tg commit -q -am two ) > /dev/null 2>&1
  rm -rf "$fd"
  out=$(ft run main.scaly 2>&1); rc=$?
  [ "$rc" != 0 ] && echo "$out" | grep -q 'is not what it was when it was first fetched here' && [ ! -e "$fd" ] && ok || bad "fetch: a changed version rc=$rc '$(echo "$out" | sed -n 2p | cut -c1-120)'"
  said=0
  printf 'package nosuch 0.1.0 "%s"\nprint("x")\n' "$fr/inner" > "$fp/a.scaly"
  ft run a.scaly 2>&1 | grep -q 'has no package nosuch 0.1.0 - no directory packages/nosuch/0.1.0' && said=$((said+1))
  printf 'package inner 0.1.0 "x; touch %s/PWNED"\nprint("x")\n' "$TMP" > "$fp/b.scaly"
  ft run b.scaly 2>&1 | grep -q 'is no source' && [ ! -e "$TMP/PWNED" ] && said=$((said+1))
  printf 'package opt 0.1.0 "--upload-pack=x"\nprint("x")\n' > "$fp/c.scaly"
  ft run c.scaly 2>&1 | grep -q 'is no source' && said=$((said+1))
  printf 'package gone 0.1.0 "%s/nowhere"\nprint("x")\n' "$fr" > "$fp/d.scaly"
  ft run d.scaly 2>&1 | grep -q 'git could not fetch' && said=$((said+1))
  printf 'package up 0.1.0 "../../climb"\nprint("x")\n' > "$fp/e.scaly"
  ft run e.scaly > /dev/null 2>&1; [ ! -e "$TMP/climb" ] && [ ! -e "$fb/../climb" ] && said=$((said+1))
  [ "$said" = 5 ] && ok || bad "fetch: $said of 5 wrong sources answered as they should"
fi

# bare: no clang, no LIB -- only the Build Tools, found by the compiler
if [ "$SCALY_COFF" = 1 ]; then
  home="$TMP/home"
  mkdir -p "$home/packages"
  cp -r packages/scaly "$home/packages/"
  if tools/native-objects.sh "$BIN" "$home" > "$TMP/native.log" 2>&1; then
    # Git Bash's own tools and Windows; the binary by its full path
    bare_path="/usr/bin:/bin:$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32"
    for flavour in "" "--release"; do
      exe="$TMP/bare${flavour#--}$SCALY_EXE"
      if ( unset LIB INCLUDE SCALY_CC CC; PATH="$bare_path" SCALY_HOME="$home" SCALY_CACHE="$TMP/bare-cache" \
             "$SCALY" build tests/tool/hello.scaly $flavour -o "$exe" ) > "$TMP/bare.log" 2>&1; then
        out=$("$exe" one two)
        [ "$out" = "hello one two" ] && ok || bad "bare $flavour: got '$out'"
      else
        bad "bare $flavour: rc=$? $(tail -3 "$TMP/bare.log" | tr '\n' ' ')"
      fi
    done
    # ... and no native file was compiled: the cache holds the package alone
    n=$(ls "$TMP/bare-cache" 2>/dev/null | grep -c '\.[cS]\.o$')
    [ "$n" = 0 ] && ok || bad "bare: $n native objects were compiled although ready-made ones were there"
  else
    bad "bare: native-objects: $(tail -2 "$TMP/native.log" | tr '\n' ' ')"
  fi
else
  echo "SKIP bare (the Build Tools' linker is a Windows matter)"
fi

echo "tool: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
