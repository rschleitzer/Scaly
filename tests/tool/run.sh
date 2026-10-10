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
#              record with a method of one name) stand side by side under
#              `scaly run`, `scaly build` and `--release` alike: every symbol
#              of a package carries its package (Emitter.tag_packages#) --
#              before, they were refused (and before that the three routes
#              gave three different answers at rc 0)
#   twobox     two packages hand out a record of one name with other fields:
#              a program holds both, with and without a `use` of one, on
#              all three routes; one package's handed to the other is refused
#   diamond    two lines of one package under two packages of a program: both
#              run, each with its own types and globals, on all three routes;
#              the program's own line whichever it declares first; refused: a
#              value across the lines, one file declaring both, C files twice
#   pkgglobals a package's global read from outside: the live cell on all three
#              routes, a generic's own package's constant in either order of
#              the program's declarations, a bare name of two packages refused
#   next       `scaly next <package> <version>`: the directory renamed, what
#              names it following, published things left alone, a path
#              behind a variable named as not followed; its three
#              refusals; a declaration of the old version builds on
#   crlf       a package whose files end their lines CR LF (a checkout on
#              Windows): built twice, the second time through its interface
#   source     `package name version "source"`: a package neither the
#              installation nor the project has is taken from where a fetch
#              lays it, $SCALY_PACKAGES/<host>/<path>/packages/<name>/<version>, on
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
#              files is announced; scalyc alone fetches nothing; one package
#              asked for out of two sources is refused, two spellings of one
#              source are one; a declaration is a MINIMUM -- the highest one
#              asked for is taken, by the program and by the packages built
#              for it, on all three routes (the package's interface written
#              into the cache on the way and read from there afterwards),
#              two lines of one package (1.0 beside 0.1, 0.2 beside 0.1)
#              stand side by side, each declarer with its own,
#              and so are 0.2 beside 0.1 (below 1.0 the second number breaks);
#              `scaly install` builds the programs a package offers (its
#              programs/), all or one by name, their sourceless declarations
#              of the package and its sibling found in the same source; a
#              package without programs and an unknown program are said;
#              `scaly install` alone lists what was installed, `scaly
#              uninstall` removes one of those and nothing else there.
#              SKIPs by name without git
#   publish    `scaly publish [--check]` (local repositories, no network):
#              published is what packages/<name>/published names -- nothing
#              before `scaly publish` writes the line (version, commit, tree),
#              commits it and pushes; again it finds nothing new. A published
#              version does not change: a change not committed, a committed
#              one and an altered record line are each refused (rc 1). Its
#              directory may leave the tree: the next version is compared
#              with it out of its commit, and a program that declares it
#              still gets it from there. A remote that is not there and a
#              directory that is not the repository's root are said (rc 2);
#              a package linked in out of another checkout is not asked. A
#              package that declares one whose newer line is published is
#              told so (a note; rc 0), and a next version that moves a
#              declared package to another line has changed what it declares.
#              The version rule (tool.publish_compare#): below 1.0 a changed
#              declaration takes the second number and anything else the
#              third, from 1.0 on a changed one the first and an addition the
#              second -- eight cases; a new version that does not compile is
#              refused. SKIPs by name without git
#   clean      `scaly clean` empties the cache, and the next build fills it
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

# file names: a name outside ASCII arrives in the file system as written, out
# of the program's own text and in through argv -- built, and under the JIT
# (whose host is the program that carries the code page there)
mkdir -p "$TMP/names" "$TMP/names jit"
if "$SCALY" build tests/tool/umlaut.scaly -o "$TMP/umlaut$SCALY_EXE" > "$TMP/umlaut.log" 2>&1; then
  ( cd "$TMP/names" && "$TMP/umlaut$SCALY_EXE" "größe.txt" )
  [ -f "$TMP/names/empfänger.txt" ] && [ -f "$TMP/names/größe.txt" ] && ok \
    || bad "file names: got '$(ls "$TMP/names" | tr '\n' ' ')'"
else
  bad "file names: rc=$? $(tail -3 "$TMP/umlaut.log" | tr '\n' ' ')"
fi
( cd "$TMP/names jit" && SCALY_HOME="$ROOT" "$SCALY" run "$ROOT/tests/tool/umlaut.scaly" "größe.txt" > "$TMP/umlaut-jit.log" 2>&1 )
[ -f "$TMP/names jit/empfänger.txt" ] && [ -f "$TMP/names jit/größe.txt" ] && ok \
  || bad "file names (jit): got '$(ls "$TMP/names jit" | tr '\n' ' ')' $(tail -2 "$TMP/umlaut-jit.log" | tr '\n' ' ')"

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

# collide: a symbol carries its package (an ABI tag, `_Z6helperB5alphaB4v0_1i`), so
# two packages that each define a function of one name, and a record of one
# name with a method, stand side by side -- on every route; and each has its
# own cell for a mutable global of one name (`calls`: 1, 10, 2 -- one shared
# cell would answer 1, 11, 12), and its own instance of a generic free function
# over its record (`allocate_slice[Box]`, 8 bytes in one and 24 in the other:
# the buffers lie 32 and 96 apart)
col="$TMP/collide"
for p in alpha beta; do
  mkdir -p "$col/packages/$p/0.1.0/$p"
  printf 'package scaly 0.1.0\n\ndefine %s\n{\n    module util\n    module Box\n}\n' "$p" > "$col/packages/$p/0.1.0/$p.scaly"
done
write_pair() {  # write_pair <helper of beta> <record of beta>
  printf 'function helper(n: int) returns int\n    n + 1\n\nfunction from_alpha(n: int) returns int\n    helper(n)\n\nmutable calls: int 0\n\nprocedure bump_alpha() returns int mutable calls\n{\n    calls := calls + 1\n    calls\n}\n' > "$col/packages/alpha/0.1.0/alpha/util.scaly"
  printf 'function %s(n: int) returns int\n    n + 100\n\nfunction from_beta(n: int) returns int\n    %s(n)\n\nmutable calls: int 0\n\nprocedure bump_beta() returns int mutable calls\n{\n    calls := calls + 10\n    calls\n}\n' "$1" "$1" > "$col/packages/beta/0.1.0/beta/util.scaly"
  printf 'use scaly.memory.Page\n\ndefine Box\n(\n    v: int\n)\n{\n    function grown(this) returns int\n        v + 1\n}\n\nfunction alpha_box(n: int) returns int\n{\n    let b Box(n)\n    b.grown()\n}\n\nprocedure alpha_span(host: ref[Page]) returns int\n{\n    var a allocate_slice[Box](host, 4 as size_t)\n    var b allocate_slice[Box](host, 4 as size_t)\n    ((b.data as size_t) - (a.data as size_t)) as int\n}\n' > "$col/packages/alpha/0.1.0/alpha/Box.scaly"
  printf 'use scaly.memory.Page\n\ndefine %s\n(\n    v: int\n    w: int\n    x: int\n)\n{\n    function grown(this) returns int\n        v + 100\n}\n\nfunction beta_box(n: int) returns int\n{\n    let b %s(n, 0, 0)\n    b.grown()\n}\n\nprocedure beta_span(host: ref[Page]) returns int\n{\n    var a allocate_slice[%s](host, 4 as size_t)\n    var b allocate_slice[%s](host, 4 as size_t)\n    ((b.data as size_t) - (a.data as size_t)) as int\n}\n' "$2" "$2" "$2" "$2" > "$col/packages/beta/0.1.0/beta/Box.scaly"
}
printf 'package alpha 0.1.0\npackage beta 0.1.0\n\nuse scaly.memory.Page\n\nlet host Page.allocate_page()\nlet one bump_alpha()\nlet ten bump_beta()\nlet two bump_alpha()\nprint("`from_alpha(1)` `from_beta(1)` `alpha_box(1)` `beta_box(1)` `one` `ten` `two` `alpha_span(host)` `beta_span(host)`")\n' > "$col/main.scaly"
write_pair helper Box
for route in "run main.scaly" "build main.scaly -o $TMP/col_a$SCALY_EXE" "build main.scaly --release -o $TMP/col_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$col" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *col_a*) [ "$rc" = 0 ] && out=$("$TMP/col_a$SCALY_EXE") ;;
    *col_b*) [ "$rc" = 0 ] && out=$("$TMP/col_b$SCALY_EXE") ;;
  esac
  [ "$out" = "2 101 2 101 1 10 2 32 96" ] && ok || bad "collide (${route%% *} ${route##*main.scaly}): rc=$rc got '$(echo "$out" | head -2 | tr '\n' '|')'"
done
# the tag is in the symbols the packages define
grep -q '_Z6helperB5alphaB4v0_1i' "$SCALY_CACHE"/alpha-0.1.0-*.defs && grep -q 'allocate_slice3BoxB4betaB4v0_1' "$SCALY_CACHE"/beta-0.1.0-*.defs && grep -q '_ZN3BoxB4betaB4v0_15grownEv' "$SCALY_CACHE"/beta-0.1.0-*.defs && ok || bad "collide: the definitions carry no package tag"
# ... and with the names apart the three routes give ONE answer
write_pair helper_b BoxB
for route in "run main.scaly" "build main.scaly -o $TMP/col_a$SCALY_EXE" "build main.scaly --release -o $TMP/col_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$col" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *col_a*) [ "$rc" = 0 ] && out=$("$TMP/col_a$SCALY_EXE") ;;
    *col_b*) [ "$rc" = 0 ] && out=$("$TMP/col_b$SCALY_EXE") ;;
  esac
  [ "$out" = "2 101 2 101 1 10 2 32 96" ] && ok || bad "collide, names apart (${route%% *}): rc=$rc got '$(echo "$out" | head -1)'"
done

# twobox: two packages each HAND OUT a record of one name with other fields.
# A type's name carries its package, a name written in a package means that
# package's own concept, so the program holds both, calls a method on each and
# hands each back to its own package -- on every route. Handing one package's
# to the other is refused. (Until 2026-10-09: garbage at rc 0 with a `use` of
# one of them, the layout check silent because a `use` names the name.)
tb="$TMP/twobox"
for p in alpha beta; do
  mkdir -p "$tb/packages/$p/0.1.0/$p"
  printf 'package scaly 0.1.0\n\ndefine %s\n{\n    module Box\n}\n' "$p" > "$tb/packages/$p/0.1.0/$p.scaly"
done
printf 'define Box\n(\n    v: int\n)\n{\n    function grown(this) returns int\n        v + 1\n}\n\nfunction make_alpha(n: int) returns Box\n    Box(n)\n\nfunction alpha_of(b: Box) returns int\n    b.v\n' > "$tb/packages/alpha/0.1.0/alpha/Box.scaly"
printf 'define Box\n(\n    v: int\n    w: int\n    x: int\n)\n{\n    function grown(this) returns int\n        v + w + x + 100\n}\n\nfunction make_beta(n: int) returns Box\n    Box(n, 10, 20)\n\nfunction beta_of(b: Box) returns int\n    b.x\n' > "$tb/packages/beta/0.1.0/beta/Box.scaly"
for variant in "" "use alpha.Box\n\n"; do
  printf 'package alpha 0.1.0\npackage beta 0.1.0\n\n'"$variant"'let a make_alpha(1)\nlet b make_beta(1)\nprint("`a.grown()` `b.grown()` `alpha_of(a)` `beta_of(b)`")\n' > "$tb/main.scaly"
  for route in "run main.scaly" "build main.scaly -o $TMP/tb_a$SCALY_EXE" "build main.scaly --release -o $TMP/tb_b$SCALY_EXE"; do
    # shellcheck disable=SC2086
    out=$(cd "$tb" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
    case "$route" in
      *tb_a*) [ "$rc" = 0 ] && out=$("$TMP/tb_a$SCALY_EXE") ;;
      *tb_b*) [ "$rc" = 0 ] && out=$("$TMP/tb_b$SCALY_EXE") ;;
    esac
    [ "$out" = "2 131 1 20" ] && ok || bad "twobox (${route%% *}${variant:+, use}): rc=$rc got '$(echo "$out" | head -1)'"
  done
done
# ... and a method on a value needs no `use` of its type where ONE package has
# the name either: `use` makes a name writable, a value brings its type along
printf 'package alpha 0.1.0\n\nlet a make_alpha(1)\nprint("`a.grown()` `a.v`")\n' > "$tb/one.scaly"
out=$(cd "$tb" && SCALY_HOME="$here" "$scaly_abs" run one.scaly 2>&1); rc=$?
[ "$out" = "2 1" ] && ok || bad "twobox: a member through a value, its type not used: rc=$rc '$(echo "$out" | head -1)'"
# ... while the NAME stays what `use` makes it: a construction without one is refused
printf 'package alpha 0.1.0\n\nlet a Box(1)\nprint("`a.v`")\n' > "$tb/named.scaly"
out=$(cd "$tb" && SCALY_HOME="$here" "$scaly_abs" run named.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'Box' && ok || bad "twobox: a construction of a type not used: rc=$rc '$(echo "$out" | head -1)'"
# ... and the two are two types to a debugger: the debug info names each with
# its package (lldb takes two types of one name for one)
( cd "$tb" && SCALY_HOME="$here" "${scaly_abs%/*}/$(basename "$BIN")" -S -g -o "$TMP/tb_dbg.ll" main.scaly ) > /dev/null 2>&1
grep -q 'name: "alpha.Box"' "$TMP/tb_dbg.ll" 2>/dev/null && grep -q 'name: "beta.Box"' "$TMP/tb_dbg.ll" && ok || bad "twobox: the debug info does not name the two records apart"
printf 'package alpha 0.1.0\npackage beta 0.1.0\n\nlet b make_beta(1)\nprint("`alpha_of(b)`")\n' > "$tb/cross.scaly"
out=$(cd "$tb" && SCALY_HOME="$here" "$scaly_abs" run cross.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'alpha_of takes the Box of alpha v0_1, and argument 1 is the Box of beta v0_1' && ok || bad "twobox: one package's record handed to the other: rc=$rc '$(echo "$out" | head -1)'"

# diamond: two LINES of one package in one program. `a` declares jdoc 0.1,
# `b` declares jdoc 0.2 (another record of the name, another answer, a global
# each); a program over both holds a value of each line, hands each back to
# its side, and the two lines count for themselves -- on every route. A name
# of jdoc written in a file means the line that file's package declares, in
# either order of the program's own declarations. Refused: a value of one
# line handed to the other, one file declaring both lines, and two lines of a
# package that brings C files.
dm="$TMP/diamond"
for v in 0.1.0 0.2.0; do
  mkdir -p "$dm/packages/jdoc/$v/jdoc"
  printf 'package scaly 0.1.0\n\ndefine jdoc\n{\n    module api\n}\n' > "$dm/packages/jdoc/$v/jdoc.scaly"
done
printf 'define Value\n(\n    n: int\n)\n\nmutable made: int 0\n\nprocedure make(n: int) returns Value mutable made\n{\n    made := made + 1\n    Value(n)\n}\n\nfunction get(v: Value) returns int\n    v.n\n\nprocedure count() returns int\n    made\n' > "$dm/packages/jdoc/0.1.0/jdoc/api.scaly"
printf 'define Value\n(\n    n: int\n    m: int\n)\n\nmutable made: int 0\n\nprocedure make(n: int) returns Value mutable made\n{\n    made := made + 10\n    Value(n, 1000)\n}\n\nfunction get(v: Value) returns int\n    v.n + v.m\n\nprocedure count() returns int\n    made\n' > "$dm/packages/jdoc/0.2.0/jdoc/api.scaly"
for p in a b; do
  v=0.1.0; [ "$p" = b ] && v=0.2.0
  mkdir -p "$dm/packages/$p/0.1.0/$p"
  printf 'package scaly 0.1.0\npackage jdoc %s\n\ndefine %s\n{\n    module use_it\n}\n' "$v" "$p" > "$dm/packages/$p/0.1.0/$p.scaly"
  printf 'use jdoc.api.Value\n\nprocedure %s_value(n: int) returns Value mutable made\n    make(n)\n\nfunction %s_get(v: Value) returns int\n    get(v)\n\nprocedure %s_count() returns int\n    count()\n' "$p" "$p" "$p" > "$dm/packages/$p/0.1.0/$p/use_it.scaly"
done
printf 'package a 0.1.0\npackage b 0.1.0\n\nlet x a_value(1)\nlet y b_value(1)\nlet z a_value(2)\nprint("`a_get(x)` `b_get(y)` `a_get(z)` `a_count()` `b_count()`")\n' > "$dm/main.scaly"
for route in "run main.scaly" "build main.scaly -o $TMP/dm_a$SCALY_EXE" "build main.scaly --release -o $TMP/dm_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$dm" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *dm_a*) [ "$rc" = 0 ] && out=$("$TMP/dm_a$SCALY_EXE") ;;
    *dm_b*) [ "$rc" = 0 ] && out=$("$TMP/dm_b$SCALY_EXE") ;;
  esac
  [ "$out" = "1 1001 2 2 10" ] && ok || bad "diamond (${route%% *} ${route##*main.scaly}): rc=$rc got '$(echo "$out" | head -1)'"
done
# the program's own line, whichever it declares first
for order in 'package b 0.1.0\npackage jdoc 0.1.0' 'package jdoc 0.1.0\npackage b 0.1.0'; do
  printf "$order"'\n\nuse jdoc.api.Value\n\nlet v make(5)\nlet w: Value make(6)\nprint("`get(v)` `get(w)` `b_get(b_value(1))`")\n' > "$dm/low.scaly"
  out=$(cd "$dm" && SCALY_HOME="$here" "$scaly_abs" run low.scaly 2>&1); rc=$?
  [ "$out" = "5 6 1001" ] && ok || bad "diamond: the program's own line (${order%%\\n*} first): rc=$rc got '$(echo "$out" | head -1)'"
done
printf 'package a 0.1.0\npackage b 0.1.0\n\nlet y b_value(1)\nprint("`a_get(y)`")\n' > "$dm/cross.scaly"
out=$(cd "$dm" && SCALY_HOME="$here" "$scaly_abs" run cross.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'a_get takes the Value of jdoc v0_1, and argument 1 is the Value of jdoc v0_2' && ok || bad "diamond: one line's value handed to the other: rc=$rc '$(echo "$out" | head -1)'"
printf 'package jdoc 0.1.0\npackage jdoc 0.2.0\n\nprint("x")\n' > "$dm/both.scaly"
out=$(cd "$dm" && SCALY_HOME="$here" "$scaly_abs" run both.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'package jdoc is declared in two lines here, 0.1.0 and 0.2.0' && ok || bad "diamond: one file declaring two lines: rc=$rc '$(echo "$out" | head -1)'"
for v in 0.1.0 0.2.0; do printf 'int jdoc_c(void) { return 1; }\n' > "$dm/packages/jdoc/$v/jdoc/extra.c"; done
out=$(cd "$dm" && SCALY_HOME="$here" "$scaly_abs" run main.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'package jdoc is required in two lines .* and brings C or assembly files' && ok || bad "diamond: two lines of a package with C files: rc=$rc '$(echo "$out" | head -1)'"

# pkgglobals: a package's constant and mutable global, read from outside it.
# A program reads the package's LIVE cell (it had a second cell of its own, or
# crashed under the JIT, until 2026-10-09); a generic routine of one package,
# instantiated by the program, reads ITS package's constant whatever another
# package calls the same (it read the other's by the order of the program's
# `package` lines: 900 for 7); and a bare name two packages declare is refused
# where neither is the writer's own.
pg="$TMP/pkgglobals"
for p in alpha beta; do
  mkdir -p "$pg/packages/$p/0.1.0/$p"
  printf 'package scaly 0.1.0\n\ndefine %s\n{\n    module vals\n}\n' "$p" > "$pg/packages/$p/0.1.0/$p.scaly"
done
printf 'define LIMIT: int 7\nmutable calls: int 1\n\nprocedure alpha_calls() returns int mutable calls\n{\n    calls := calls + 1\n    calls\n}\n\nfunction scaled[T](v: T) returns int\n    LIMIT\n' > "$pg/packages/alpha/0.1.0/alpha/vals.scaly"
printf 'define LIMIT: int 900\nmutable calls: int 50\n\nfunction widened[T](v: T) returns int\n    LIMIT\n' > "$pg/packages/beta/0.1.0/beta/vals.scaly"
printf 'package alpha 0.1.0\n\nlet first alpha_calls()\nprint("`LIMIT` `first` `calls`")\n' > "$pg/live.scaly"
for route in "run live.scaly" "build live.scaly -o $TMP/pg_a$SCALY_EXE" "build live.scaly --release -o $TMP/pg_b$SCALY_EXE"; do
  # shellcheck disable=SC2086
  out=$(cd "$pg" && SCALY_HOME="$here" "$scaly_abs" $route 2>&1); rc=$?
  case "$route" in
    *pg_a*) [ "$rc" = 0 ] && out=$("$TMP/pg_a$SCALY_EXE") ;;
    *pg_b*) [ "$rc" = 0 ] && out=$("$TMP/pg_b$SCALY_EXE") ;;
  esac
  [ "$out" = "7 2 2" ] && ok || bad "pkgglobals, the live cell (${route%% *} ${route##*live.scaly}): rc=$rc got '$(echo "$out" | head -1)'"
done
for order in 'package alpha 0.1.0\npackage beta 0.1.0' 'package beta 0.1.0\npackage alpha 0.1.0'; do
  printf "$order"'\n\nprint("`scaled[int](1)` `widened[int](1)`")\n' > "$pg/generic.scaly"
  out=$(cd "$pg" && SCALY_HOME="$here" "$scaly_abs" run generic.scaly 2>&1); rc=$?
  [ "$out" = "7 900" ] && ok || bad "pkgglobals: a generic reads its own package's constant (${order%%\\n*} first): rc=$rc got '$(echo "$out" | head -1)'"
done
printf 'package alpha 0.1.0\npackage beta 0.1.0\n\nprint("`LIMIT`")\n' > "$pg/bare.scaly"
out=$(cd "$pg" && SCALY_HOME="$here" "$scaly_abs" run bare.scaly 2>&1); rc=$?
[ "$rc" != 0 ] && echo "$out" | grep -q 'LIMIT is declared by two packages, alpha v0_1 and beta v0_1' && ok || bad "pkgglobals: a bare name of two packages: rc=$rc '$(echo "$out" | head -1)'"

# next: `scaly next <package> <version>` begins a package's next version -- the
# directory renamed, every tracked file that names it following, a
# `published` record and another package's published directory left alone, a
# longer number that only begins alike left alone too; refused with
# uncommitted changes, for a number that is not higher, for none at all. And
# a program that still declares the old version builds against the new one.
if command -v git > /dev/null 2>&1; then
  nx="$TMP/next"; mkdir -p "$nx/packages/demo/0.1.0" "$nx/packages/other/0.1.0" "$nx/tools"
  ng() { git -C "$nx" -c user.name=t -c user.email=t@example.invalid -c init.defaultBranch=main "$@"; }
  printf 'package scaly 0.1.0\n\ndefine demo\n{\n    function answer() returns int\n        42\n}\n' > "$nx/packages/demo/0.1.0/demo.scaly"
  printf '; reads packages/demo/0.1.0/demo.scaly\npackage scaly 0.1.0\n\ndefine other\n{\n}\n' > "$nx/packages/other/0.1.0/other.scaly"
  printf '0.1.0 0000000000000000000000000000000000000000 0000000000000000000000000000000000000000\n' > "$nx/packages/other/published"
  printf '0.1.0 0000000000000000000000000000000000000000 0000000000000000000000000000000000000000\n' > "$nx/packages/demo/published"
  printf '#!/bin/sh\ncat packages/demo/0.1.0/demo.scaly packages/demo/0.1.0/demo.scaly\necho packages/demo/0.1.01 packages/demo/0.1.0.2\n' > "$nx/tools/show.sh"; chmod +x "$nx/tools/show.sh"
  printf 'package demo 0.1.0\n\nprint("`demo.answer()`")\n' > "$nx/main.scaly"
  printf '#!/bin/sh\nPKG=packages/demo\ncat "$PKG"/0.1.0/demo.scaly\n' > "$nx/tools/var.sh"
  ng init -q && ng add -A && ng commit -q -m one
  nn() { ( cd "$nx" && SCALY_HOME="$here" "$scaly_abs" next "$@" ) 2>&1; }
  out=$(nn demo 0.1.0); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'demo is at 0.1.0, 0.1.0 is not higher' && ok || bad "next: a number that is not higher: rc=$rc '$out'"
  out=$(nn demo one); rc=$?
  [ "$rc" = 1 ] && [ -d "$nx/packages/demo/0.1.0" ] && ok || bad "next: no version: rc=$rc '$out'"
  echo "; edited" >> "$nx/main.scaly"
  out=$(nn demo 0.1.1); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'changes that are not committed' && [ -d "$nx/packages/demo/0.1.0" ] && ok || bad "next: uncommitted changes: rc=$rc '$out'"
  ng checkout -q -- main.scaly
  out=$(nn demo 0.1.1); rc=$?
  if [ "$rc" = 0 ] && [ -f "$nx/packages/demo/0.1.1/demo.scaly" ] && [ ! -e "$nx/packages/demo/0.1.0" ] \
     && echo "$out" | grep -q 'demo 0.1.0 -> 0.1.1' && echo "$out" | grep -q '0.1.0 is published' \
     && echo "$out" | grep -q '1 more files spell /0.1.0/' && echo "$out" | grep -q '^    tools/var.sh$' && ! echo "$out" | grep -q '^    packages/other' \
     && [ "$(grep -c 'packages/demo/0.1.1/demo.scaly' "$nx/tools/show.sh")" = 1 ] && grep -q 'packages/demo/0.1.01 packages/demo/0.1.0.2' "$nx/tools/show.sh" \
     && [ -x "$nx/tools/show.sh" ] && grep -q 'packages/demo/0.1.0/demo.scaly' "$nx/packages/other/0.1.0/other.scaly" \
     && grep -q '^0.1.0 ' "$nx/packages/demo/published" && grep -q '^package demo 0.1.0' "$nx/main.scaly"; then
    ok
  else
    bad "next: the rename: rc=$rc '$(echo "$out" | head -2 | tr '\n' '|')' show.sh: $(tr '\n' '|' < "$nx/tools/show.sh")"
  fi
  out=$(cd "$nx" && SCALY_HOME="$here" "$scaly_abs" run main.scaly 2>&1); rc=$?
  [ "$out" = "42" ] && ok || bad "next: a declaration of the old version against the new directory: rc=$rc '$(echo "$out" | head -1)'"
else
  echo "SKIP next (no git)"
fi

# crlf: a package whose lines end CR LF -- what git hands out on Windows where
# nothing forbids it. The interface the first build writes into the cache is
# what the second build reads; its writer took a CR for part of a name and set
# a record's attribute before the break (`expected Concept` at the record's
# parenthesis, on Windows only).
cr="$TMP/crlf"; mkdir -p "$cr/packages/kit/0.1.0/kit"
printf 'package scaly 0.1.0\r\n\r\ndefine kit\r\n{\r\n    module parts\r\n}\r\n' > "$cr/packages/kit/0.1.0/kit.scaly"
printf 'define LIMIT: int 58\r\n\r\ndefine Bag\r\n(\r\n    var items: Array[int]\r\n)\r\n{\r\n    init()\r\n    {\r\n        items := Array[int]^this()\r\n    }\r\n\r\n    procedure put(mutable this, n: int)\r\n    {\r\n        items.add(n)\r\n    }\r\n\r\n    function count(this) returns int\r\n        items.length as int\r\n}\r\n\r\nprocedure make_bag() returns int\r\n{\r\n    var b Bag()\r\n    b.put(1)\r\n    b.put(2)\r\n    b.count() + LIMIT\r\n}\r\n' > "$cr/packages/kit/0.1.0/kit/parts.scaly"
printf 'package kit 0.1.0\r\n\r\nprint("`make_bag()`")\r\n' > "$cr/main.scaly"
for turn in first second; do
  out=$(cd "$cr" && SCALY_CACHE="$TMP/crlf-cache" SCALY_HOME="$here" "$scaly_abs" build main.scaly -o "$TMP/crlf-bin" 2>&1 && "$TMP/crlf-bin"); rc=$?
  [ "$out" = "60" ] && ok || bad "crlf: the $turn build: rc=$rc '$(echo "$out" | head -1 | cut -c1-160)'"
done

# source: the third place a package is looked for, what a fetch laid down
src="$TMP/source"
fetched="$TMP/userhome2/.scaly/packages"
mkdir -p "$src"
printf 'package shapes 0.1.0 "github.com/someone/shapes"\n\nprint(shapes.greeting("fetched"))\n' > "$src/main.scaly"
# (asked of scalyc, which only looks: the tool would go and fetch it)
case "$BIN" in /*) BIN_ABS="$BIN" ;; *) BIN_ABS="$here/$BIN" ;; esac
out=$(cd "$src" && SCALY_HOME="$here" SCALY_PACKAGES="$fetched" "$BIN_ABS" -S -o "$TMP/source.ll" main.scaly 2>&1); rc=$?
if [ "$rc" != 0 ] && echo "$out" | grep -q 'package not found: shapes 0.1.0' \
   && echo "$out" | grep -q 'userhome2/.scaly/packages/github.com/someone/shapes/packages/shapes/0.1.0/shapes.scaly' \
   && echo "$out" | grep -q 'not fetched from "github.com/someone/shapes"'; then
  ok
else
  bad "source, missing: rc=$rc '$(echo "$out" | head -1 | cut -c1-160)'"
fi
( cd "$TMP" && SCALY_HOME="$here" "$scaly_abs" new shapes --lib ) > /dev/null 2>&1
mkdir -p "$fetched/github.com/someone/shapes/packages/shapes"
cp -R "$TMP/shapes/packages/shapes/0.1.0" "$fetched/github.com/someone/shapes/packages/shapes/"
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
  # ★The base has the shape of the DEFAULT one, ~/.scaly/packages: a path with
  # a `packages` segment BEFORE the source's. With a base that had none, every
  # check here passed while the default place could not build a package that
  # reads a sibling's constant (the planner took the first such segment for
  # the package's own; found on a user's machine, 2026-10-06).
  fr="$TMP/fetchrepos"; fp="$TMP/fetchproj"; fb="$TMP/userhome/.scaly/packages"
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
  fd=$(find "$fb" -type d -path '*/inner/packages/inner/0.1.0' | head -1)
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
  # versions: a declaration names a minimum, the build takes the highest one
  # asked for within a major version (Modeler.select_versions#) -- for the
  # program AND for the packages compiled for it
  vr="$fr/mini"; mkdir -p "$vr"
  for v in 0.1.2 0.1.5 0.2.0 1.0.0; do
    mkdir -p "$vr/packages/mini/$v"
    printf 'package scaly 0.1.0\n\ndefine mini\n{\n    function version() returns String\n        "%s"\n}\n' "$v" > "$vr/packages/mini/$v/mini.scaly"
  done
  ( cd "$vr" && tg init -q && tg add -A && tg commit -q -m one ) > /dev/null 2>&1
  mkdir -p "$fr/via/packages/via/0.1.0"
  printf 'package scaly 0.1.0\npackage mini 0.1.2 "%s"\n\ndefine via\n{\n    function seen() returns String\n        mini.version()\n}\n' "$vr" > "$fr/via/packages/via/0.1.0/via.scaly"
  ( cd "$fr/via" && tg init -q && tg add -A && tg commit -q -m one ) > /dev/null 2>&1
  printf 'package via 0.1.0 "%s"\npackage mini 0.1.5 "%s"\n\nprint("`mini.version()` `via.seen()`")\n' "$fr/via" "$vr" > "$fp/newer.scaly"
  printf 'package via 0.1.0 "%s"\n\nprint(via.seen())\n' "$fr/via" > "$fp/older.scaly"
  printf 'package via 0.1.0 "%s"\npackage mini 1.0.0 "%s"\n\nprint("`mini.version()` `via.seen()`")\n' "$fr/via" "$vr" > "$fp/major.scaly"
  printf 'package via 0.1.0 "%s"\npackage mini 0.2.0 "%s"\n\nprint("`mini.version()` `via.seen()`")\n' "$fr/via" "$vr" > "$fp/minor.scaly"
  agree=0
  for route in "run newer.scaly" "build newer.scaly -o $TMP/ver_a$SCALY_EXE" "build newer.scaly --release -o $TMP/ver_b$SCALY_EXE"; do
    # shellcheck disable=SC2086
    out=$(ft $route 2> /dev/null); rc=$?
    case "$route" in
      *ver_a*) [ "$rc" = 0 ] && out=$("$TMP/ver_a$SCALY_EXE") ;;
      *ver_b*) [ "$rc" = 0 ] && out=$("$TMP/ver_b$SCALY_EXE") ;;
    esac
    [ "$out" = "0.1.5 0.1.5" ] && agree=$((agree+1)) || echo "    versions (${route%% *}): got '$out'"
  done
  [ "$agree" = 3 ] && ok || bad "versions: $agree of 3 routes took 0.1.5 for the program and for the package that asked for 0.1.2"
  # the same package for a program that asks for nothing newer: its own minimum,
  # and not the object the other program left in the cache
  out=$(ft run older.scaly 2> /dev/null); out2=""
  ft build older.scaly -o "$TMP/ver_c$SCALY_EXE" > /dev/null 2>&1 && out2=$("$TMP/ver_c$SCALY_EXE")
  [ "$out" = "0.1.2" ] && [ "$out2" = "0.1.2" ] && ok || bad "versions: alone the package took '$out' / '$out2', not its minimum 0.1.2"
  # two LINES of one package stand side by side (2026-10-09; refused until
  # then): the program's `mini` is the line it declares, the package's its own
  for pair in "major 1.0.0" "minor 0.2.0"; do
    lines=0
    for route in "run ${pair%% *}.scaly" "build ${pair%% *}.scaly -o $TMP/ver_d$SCALY_EXE" "build ${pair%% *}.scaly --release -o $TMP/ver_e$SCALY_EXE"; do
      # shellcheck disable=SC2086
      out=$(ft $route 2> /dev/null); rc=$?
      case "$route" in
        *ver_d*) [ "$rc" = 0 ] && out=$("$TMP/ver_d$SCALY_EXE") ;;
        *ver_e*) [ "$rc" = 0 ] && out=$("$TMP/ver_e$SCALY_EXE") ;;
      esac
      [ "$out" = "${pair##* } 0.1.2" ] && lines=$((lines+1)) || echo "    versions, two lines (${route%% *}): got '$out'"
    done
    [ "$lines" = 3 ] && ok || bad "versions: $lines of 3 routes ran ${pair##* } for the program beside 0.1.2 for the package"
  done
  # a fetched package's interface is written into the cache when the package
  # is compiled (tool.interface_wanted#) and read from there afterwards: the
  # bodies gone (`linked`), and a build that compiles nothing writes none anew
  ifaces=$(ls -d "$SCALY_CACHE"/mini-*.iface 2>/dev/null | wc -l | tr -d ' ')
  linked=$(cat "$SCALY_CACHE"/mini-*.iface/interface/mini.scaly 2>/dev/null | grep -c ' linked$')
  ft build newer.scaly -o "$TMP/ver_d$SCALY_EXE" > /dev/null 2>&1; out=$("$TMP/ver_d$SCALY_EXE" 2> /dev/null)
  again=$(ls -d "$SCALY_CACHE"/mini-*.iface 2>/dev/null | wc -l | tr -d ' ')
  [ "$ifaces" -gt 0 ] && [ "$linked" -gt 0 ] && [ "$again" = "$ifaces" ] && [ "$out" = "0.1.5 0.1.5" ] && ok || bad "interfaces in the cache: $ifaces written, $linked routines linked, $again after a second build, the program says '$out'"
  # install: the programs a package offers are the files of its programs/,
  # one of them called like the package; they name their own package and its
  # sibling WITHOUT a source, as in the repository, and fetched they still
  # find them (Modeler.inherited_source#)
  kr="$fr/kit"; mkdir -p "$kr/packages/kit/0.1.0/programs" "$kr/packages/kithelp/0.1.0" "$kr/packages/bare/0.1.0"
  mkdir -p "$kr/packages/kithelp/0.1.0/kithelp"
  printf 'package scaly 0.1.0\n\ndefine kithelp\n{\n    module marks\n}\n' > "$kr/packages/kithelp/0.1.0/kithelp.scaly"
  # a module-level constant of the sibling, read by the package beside it: the
  # planner finds a file's package by the `packages` segment of its path
  printf 'define KIT_MARKS: int 1\n\nfunction mark() returns String\n    "!"\n' > "$kr/packages/kithelp/0.1.0/kithelp/marks.scaly"
  printf 'package scaly 0.1.0\npackage kithelp 0.1.0\n\ndefine kit\n{\n    function say(what: String) returns String\n        "kit says `what``mark()``KIT_MARKS`"\n}\n' > "$kr/packages/kit/0.1.0/kit.scaly"
  printf 'package kit 0.1.0\n\nprint(kit.say("hello"))\n' > "$kr/packages/kit/0.1.0/programs/kit.scaly"
  printf 'package kit 0.1.0\n\nprint(kit.say("twice twice"))\n' > "$kr/packages/kit/0.1.0/programs/kit_twice.scaly"
  printf 'package scaly 0.1.0\n\ndefine bare\n{\n    function one() returns int\n        1\n}\n' > "$kr/packages/bare/0.1.0/bare.scaly"
  ( cd "$kr" && tg init -q && tg add -A && tg commit -q -m one ) > /dev/null 2>&1
  kb="$TMP/kitbin"
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 0.1.0 2> "$TMP/install.err"); rc=$?
  if [ "$rc" = 0 ] && echo "$out" | grep -q "^installed $kb/kit$SCALY_EXE\$" && echo "$out" | grep -q "^installed $kb/kit_twice$SCALY_EXE\$" \
     && [ "$("$kb/kit$SCALY_EXE")" = "kit says hello!1" ] && [ "$("$kb/kit_twice$SCALY_EXE")" = "kit says twice twice!1" ] \
     && grep -q '^scaly: fetching kithelp 0.1.0 from ' "$TMP/install.err"; then
    ok
  else
    bad "install: rc=$rc '$(echo "$out" | tr '\n' '|' | cut -c1-200)' $(grep -v '^scaly: ' "$TMP/install.err" | head -1 | cut -c1-120)"
  fi
  # a directory that is not on the PATH is said, one that is, is not
  noted=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 0.1.0 kit 2>/dev/null)
  quiet=$(cd "$fp" && PATH="$PATH:$kb/" SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 0.1.0 kit 2>/dev/null)
  echo "$noted" | grep -q "^note: $kb is not on your PATH" && ! echo "$quiet" | grep -q 'not on your PATH' && ok \
    || bad "install: the PATH note '$(echo "$noted" | tail -1 | cut -c1-120)' / '$(echo "$quiet" | tail -1 | cut -c1-80)'"
  # what was installed is listed, and uninstall removes one of THOSE and
  # nothing else of the directory
  : > "$kb/mine$SCALY_EXE"
  listed=$(SCALY_HOME="$here" SCALY_BIN="$kb" "$scaly_abs" install 2>&1)
  gone=$(SCALY_HOME="$here" SCALY_BIN="$kb" "$scaly_abs" uninstall kit 2>&1); rc_gone=$?
  refused=$(SCALY_HOME="$here" SCALY_BIN="$kb" "$scaly_abs" uninstall mine 2>&1); rc_ref=$?
  after=$(SCALY_HOME="$here" SCALY_BIN="$kb" "$scaly_abs" install 2>&1)
  if echo "$listed" | grep -q "^  kit kit 0.1.0 $kr\$" && echo "$listed" | grep -q "^  kit_twice kit 0.1.0 " \
     && [ "$rc_gone" = 0 ] && [ ! -e "$kb/kit$SCALY_EXE" ] && [ -x "$kb/kit_twice$SCALY_EXE" ] \
     && [ "$rc_ref" != 0 ] && [ -e "$kb/mine$SCALY_EXE" ] && echo "$refused" | grep -q 'was not installed by scaly install' \
     && ! echo "$after" | grep -q '^  kit kit ' && echo "$after" | grep -q '^  kit_twice kit '; then
    ok
  else
    bad "install: list / uninstall '$(echo "$listed" | tr '\n' '|' | cut -c1-160)' gone=$rc_gone refused=$rc_ref"
  fi
  rm -rf "$kb"
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 0.1.0 kit_twice 2>&1)
  [ -x "$kb/kit_twice$SCALY_EXE" ] && [ ! -e "$kb/kit$SCALY_EXE" ] && ok || bad "install: one program by name '$(echo "$out" | tail -1 | cut -c1-120)'"
  told=0
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" bare 0.1.0 2>&1); rc=$?
  [ "$rc" != 0 ] && echo "$out" | grep -q 'offers no programs - it has no directory programs/, it is a library' && told=$((told+1))
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 0.1.0 nosuch 2>&1); rc=$?
  [ "$rc" != 0 ] && echo "$out" | grep -q 'offers no program nosuch' && told=$((told+1))
  out=$(cd "$fp" && SCALY_HOME="$here" SCALY_PACKAGES="$fb" SCALY_BIN="$kb" "$scaly_abs" install "$kr" kit 2>&1); rc=$?
  [ "$rc" != 0 ] && echo "$out" | grep -q '^Usage: scaly install' && told=$((told+1))
  [ "$told" = 3 ] && ok || bad "install: $told of 3 wrong invocations answered as they should"
  # one package comes from one source: main asks for inner out of a second
  # repository while outer asks for it out of the first
  cp -R "$fr/inner" "$fr/inner2"
  rm -rf "$fb"
  ( cd "$fr/inner" && tg checkout -q HEAD~1 -- . && tg commit -q -m back ) > /dev/null 2>&1
  printf 'package inner 0.1.0 "%s"\npackage outer 0.1.0 "%s"\n\nprint(outer.twice("git"))\n' "$fr/inner2" "$fr/outer" > "$fp/two.scaly"
  out=$(ft run two.scaly 2>&1); rc=$?
  [ "$rc" != 0 ] && echo "$out" | grep -q 'package inner is required from two sources' && ok || bad "fetch: two sources rc=$rc '$(echo "$out" | grep -v '^scaly: ' | head -1 | cut -c1-160)'"
  # ... and two spellings of one source are one
  printf 'package inner 0.1.0 "file://%s"\npackage outer 0.1.0 "%s"\n\nprint(outer.twice("git"))\n' "$fr/inner" "$fr/outer" > "$fp/one.scaly"
  out=$(ft run one.scaly 2> "$TMP/fetch3.err")
  [ "$out" = "Hello, Hello, git!!" ] && ok || bad "fetch: one source in two spellings got '$out' $(grep -v '^scaly: ' "$TMP/fetch3.err" | head -1 | cut -c1-160)"
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

# publish: a published version does not change (stage E1)
if ! command -v git > /dev/null 2>&1; then
  echo "SKIP publish (no git on the PATH)"
else
  pg() { git -c user.name=t -c user.email=t@example.invalid -c init.defaultBranch=main "$@"; }
  pub="$TMP/publish"; mkdir -p "$pub"
  # a repository with the package `scaly new --lib` writes, a bare one as its origin
  pubrepo() {
    ( cd "$pub" && SCALY_HOME="$here" "$scaly_abs" new demo --lib && mv demo "$1" && cd "$1" \
        && { [ "$2" = 0.1.0 ] || mv packages/demo/0.1.0 "packages/demo/$2"; } \
        && pg init -q && git config user.name t && git config user.email t@example.invalid \
        && pg add -A && pg commit -q -m one && pg clone -q --bare . "$pub/$1.git" && pg remote add origin "$pub/$1.git"
    ) > "$TMP/publish-$1.log" 2>&1
  }
  pp() { ( cd "$pub/work" && SCALY_HOME="$here" "$scaly_abs" publish "$@" ) 2>&1; }
  pubrepo work 0.1.0
  # nothing is published before `scaly publish` says so
  out=$(pp --check); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q "demo 0.1.0 is new, the package's first version" && [ ! -e "$pub/work/packages/demo/published" ] && ok || bad "publish --check, nothing published yet: rc=$rc '$(echo "$out" | tail -1 | cut -c1-120)'"
  out=$(pp); rc=$?
  line=$(grep '^0\.1\.0 ' "$pub/work/packages/demo/published" 2>/dev/null)
  at=$(echo "$line" | cut -d' ' -f2)
  [ "$rc" = 0 ] && [ -n "$at" ] && [ "$(echo "$line" | cut -d' ' -f3)" = "$(git -C "$pub/work" rev-parse "$at:packages/demo/0.1.0" 2>/dev/null)" ] \
    && [ "$(git --git-dir="$pub/work.git" show HEAD:packages/demo/published 2>/dev/null | grep -c '^0\.1\.0 ')" = 1 ] \
    && ok || bad "publish: rc=$rc line '$line' '$(echo "$out" | tail -1 | cut -c1-140)'"
  out=$(pp); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q 'nothing new to publish' && ok || bad "publish, again: rc=$rc '$(echo "$out" | tail -1 | cut -c1-120)'"
  out=$(pp --check); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q '^publish: 1 published version(s) unchanged' && ok || bad "publish --check, unchanged: rc=$rc '$(echo "$out" | head -1 | cut -c1-120)'"
  echo '; more' >> "$pub/work/packages/demo/0.1.0/demo.scaly"
  out=$(pp --check); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'demo 0.1.0 was published as tree .* not committed' && ok || bad "publish, not committed: rc=$rc '$(echo "$out" | sed -n 2p | cut -c1-140)'"
  ( cd "$pub/work" && pg commit -q -am two )
  out=$(pp --check); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'demo 0.1.0 was published as tree .*; its directory is tree ' && ok || bad "publish, committed change: rc=$rc '$(echo "$out" | sed -n 2p | cut -c1-140)'"
  ( cd "$pub/work" && pg reset -q --hard HEAD~1 )
  # a line of the record does not change either
  sed -i.bak 's/^0\.1\.0 ......./0.1.0 0000000/' "$pub/work/packages/demo/published"; rm -f "$pub/work/packages/demo/published.bak"
  out=$(pp --check); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'demo 0.1.0: its line in packages/demo/published is gone or reads differently' && ok || bad "publish, a record line changed: rc=$rc '$(echo "$out" | sed -n 2p | cut -c1-140)'"
  ( cd "$pub/work" && pg checkout -q -- packages/demo/published )
  # the next version, and the published one leaves the tree: it lives on at
  # its commit, and the new one is compared with it out of there
  cp -R "$pub/work/packages/demo/0.1.0" "$pub/work/packages/demo/0.1.1"
  sed -i.bak 's/function greeting(name: String)/function greeting(who: String)/; s/`name`/`who`/' "$pub/work/packages/demo/0.1.1/demo.scaly"; rm -f "$pub/work/packages/demo/0.1.1/demo.scaly.bak"
  ( cd "$pub/work" && pg rm -q -r packages/demo/0.1.0 && pg add -A && pg commit -q -m next )
  out=$(pp --check); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q '^publish: 1 published version(s) unchanged' && echo "$out" | grep -q 'demo 0.1.1 understates what it changed since 0.1.0; that takes 0.2.0 at least' && ok || bad "publish, compared with a version out of history: rc=$rc '$(echo "$out" | tail -4 | tr '\n' '|' | cut -c1-240)'"
  ( cd "$pub/work" && pg mv packages/demo/0.1.1 packages/demo/0.2.0 && pg commit -q -m "as 0.2.0" )
  out=$(pp); rc=$?
  [ "$rc" = 0 ] && [ "$(git --git-dir="$pub/work.git" show HEAD:packages/demo/published 2>/dev/null | grep -c '^0\.[12]\.0 ')" = 2 ] && [ -z "$(cd "$pub/work" && git worktree list | sed 1d)" ] && ok || bad "publish, the second version: rc=$rc '$(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)'"
  # ... and a program still gets the version that left the tree, out of its commit
  mkdir -p "$pub/user"
  printf 'package demo 0.1.0 "%s"\n\nprint(demo.greeting("history"))\n' "$pub/work.git" > "$pub/user/old.scaly"
  printf 'package demo 0.2.0 "%s"\n\nprint(demo.greeting("head"))\n' "$pub/work.git" > "$pub/user/new.scaly"
  out=$( cd "$pub/user" && SCALY_HOME="$here" SCALY_PACKAGES="$pub/home/.scaly/packages" "$scaly_abs" run old.scaly 2> /dev/null )
  out2=$( cd "$pub/user" && SCALY_HOME="$here" SCALY_PACKAGES="$pub/home/.scaly/packages" "$scaly_abs" run new.scaly 2> /dev/null )
  [ "$out" = "Hello, history!" ] && [ "$out2" = "Hello, head!" ] && grep -q "^commit $at" "$(find "$pub/home/.scaly/packages" -name 0.1.0.fetched | head -1)" 2>/dev/null && ok || bad "publish: a program fetching the version that left the tree got '$out' / '$out2'"
  # a package linked into packages/ out of another checkout is not this
  # repository's to publish, its record there no claim on this one
  ( cd "$pub/work" && ln -s "$pub/rule-other/packages/demo" packages/other && echo /packages/other >> .git/info/exclude ) 2> /dev/null
  mkdir -p "$pub/rule-other/packages/demo/0.1.0"; printf '0.1.0 0000000000000000000000000000000000000000 0000000000000000000000000000000000000000\n' > "$pub/rule-other/packages/demo/published"
  out=$(pp --check); rc=$?
  [ "$rc" = 0 ] && ok || bad "publish, a linked package of another checkout: rc=$rc '$(echo "$out" | sed -n 2p | cut -c1-140)'"
  rm -f "$pub/work/packages/other"
  # a package that declares one whose newer LINE is published is told so
  # (demo 0.2.0 beside the declared 0.1.0), and is published all the same
  ( cd "$pub" && SCALY_HOME="$here" "$scaly_abs" new client --lib && cd client \
      && sed -i.bak "s|^package scaly 0.1.0\$|package scaly 0.1.0\\
package demo 0.1.0 \"$pub/work.git\"|" packages/client/0.1.0/client.scaly && rm -f packages/client/0.1.0/client.scaly.bak \
      && pg init -q && git config user.name t && git config user.email t@example.invalid \
      && pg add -A && pg commit -q -m one && pg clone -q --bare . "$pub/client.git" && pg remote add origin "$pub/client.git"
  ) > "$TMP/publish-client.log" 2>&1
  out=$( cd "$pub/client" && SCALY_HOME="$here" SCALY_PACKAGES="$pub/home/.scaly/packages" "$scaly_abs" publish --check 2>&1 ); rc=$?
  [ "$rc" = 0 ] && echo "$out" | grep -q '^note: client 0.1.0 declares demo 0.1.0; demo 0.2.0 is published, a newer line' && ok || bad "publish, a dependency's newer line: rc=$rc '$(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-220)'"
  # ... and a next version that MOVES a declared package to another line has
  # changed what it declares: its users' builds must follow
  ( cd "$pub/client" && SCALY_HOME="$here" SCALY_PACKAGES="$pub/home/.scaly/packages" "$scaly_abs" publish ) > "$TMP/publish-client.out" 2>&1
  cp -R "$pub/client/packages/client/0.1.0" "$pub/client/packages/client/0.1.1"
  sed -i.bak 's/^package demo 0\.1\.0 /package demo 0.2.0 /' "$pub/client/packages/client/0.1.1/client.scaly"; rm -f "$pub/client/packages/client/0.1.1/client.scaly.bak"
  out=$( cd "$pub/client" && SCALY_HOME="$here" SCALY_PACKAGES="$pub/home/.scaly/packages" "$scaly_abs" publish --check 2>&1 ); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'client 0.1.1 understates what it changed since 0.1.0; that takes 0.2.0 at least' && echo "$out" | grep -q 'package demo 0.1.0 -> 0.2.0: another line' && ok || bad "publish, a dependency moved to another line: rc=$rc '$(echo "$out" | tail -4 | tr '\n' '|' | cut -c1-260)'"
  out=$(pp --check "$pub/nowhere.git"); rc=$?
  [ "$rc" = 2 ] && echo "$out" | grep -q 'could not ask' && ok || bad "publish, no such remote: rc=$rc '$(echo "$out" | head -1 | cut -c1-120)'"
  out=$( ( cd "$pub/work/packages" && SCALY_HOME="$here" "$scaly_abs" publish --check ) 2>&1 ); rc=$?
  [ "$rc" = 2 ] && echo "$out" | grep -q 'root of the repository' && ok || bad "publish, not at the root: rc=$rc '$(echo "$out" | head -1 | cut -c1-120)'"
  # E2: a new version's number says what it changed against the one before
  # (a package `scaly new --lib` wrote, published as 0.1.0 and, in a second
  # repository, as 1.0.0)
  pv() { ( cd "$pub/$1" && SCALY_HOME="$here" "$scaly_abs" publish --check ) 2>&1; }
  for first in 0.1.0 1.0.0; do
    pubrepo "rule-$first" "$first"
    ( cd "$pub/rule-$first" && SCALY_HOME="$here" "$scaly_abs" publish ) > "$TMP/publish-rule-$first.out" 2>&1
  done
  # the next version as a copy of the first, its text run through sed
  nextv() { rm -rf "$pub/$1/packages/demo/$3"; cp -R "$pub/$1/packages/demo/$2" "$pub/$1/packages/demo/$3"; sed -i.bak "$4" "$pub/$1/packages/demo/$3/demo.scaly"; rm -f "$pub/$1/packages/demo/$3/demo.scaly.bak"; }
  body='s/"Hello, `name`!"/"Hello,  `name`!"/'
  added='s/^    function test_greeting/    function farewell(name: String) returns String\
        "Bye, `name`!"\
\
    function test_greeting/'
  changed='s/function greeting(name: String)/function greeting(who: String)/; s/`name`/`who`/'
  rule=0
  # below 1.0: bodies and additions take the third number, a changed declaration the second
  nextv rule-0.1.0 0.1.0 0.1.1 "$body";    out=$(pv rule-0.1.0); [ $? = 0 ] && echo "$out" | grep -q 'demo 0.1.1 is new after 0.1.0: 0 declaration(s) gone or changed, 0 added' && rule=$((rule+1)) || echo "    publish rule (0.x body): $(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)"
  nextv rule-0.1.0 0.1.0 0.1.1 "$added";   out=$(pv rule-0.1.0); [ $? = 0 ] && echo "$out" | grep -q '0 declaration(s) gone or changed, 1 added' && rule=$((rule+1)) || echo "    publish rule (0.x added): $(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)"
  nextv rule-0.1.0 0.1.0 0.1.1 "$changed"; out=$(pv rule-0.1.0); [ $? = 1 ] && echo "$out" | grep -q 'demo 0.1.1 understates what it changed since 0.1.0; that takes 0.2.0 at least' && echo "$out" | grep -q 'demo.scaly demo.| function greeting(name: String) returns String' && rule=$((rule+1)) || echo "    publish rule (0.x changed): $(echo "$out" | tail -4 | tr '\n' '|' | cut -c1-300)"
  rm -rf "$pub/rule-0.1.0/packages/demo/0.1.1"
  nextv rule-0.1.0 0.1.0 0.2.0 "$changed"; out=$(pv rule-0.1.0); [ $? = 0 ] && echo "$out" | grep -q 'demo 0.2.0 is new after 0.1.0: 1 declaration(s) gone or changed, 1 added' && rule=$((rule+1)) || echo "    publish rule (0.2.0): $(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)"
  # from 1.0 on: an addition takes the second number, a changed declaration the first
  nextv rule-1.0.0 1.0.0 1.0.1 "$body";    out=$(pv rule-1.0.0); [ $? = 0 ] && rule=$((rule+1)) || echo "    publish rule (1.x body): $(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)"
  nextv rule-1.0.0 1.0.0 1.0.1 "$added";   out=$(pv rule-1.0.0); [ $? = 1 ] && echo "$out" | grep -q 'that takes 1.1.0 at least' && rule=$((rule+1)) || echo "    publish rule (1.x added): $(echo "$out" | tail -4 | tr '\n' '|' | cut -c1-300)"
  rm -rf "$pub/rule-1.0.0/packages/demo/1.0.1"
  nextv rule-1.0.0 1.0.0 1.1.0 "$changed"; out=$(pv rule-1.0.0); [ $? = 1 ] && echo "$out" | grep -q 'that takes 2.0.0 at least' && rule=$((rule+1)) || echo "    publish rule (1.x changed): $(echo "$out" | tail -4 | tr '\n' '|' | cut -c1-300)"
  rm -rf "$pub/rule-1.0.0/packages/demo/1.1.0"
  nextv rule-1.0.0 1.0.0 2.0.0 "$changed"; out=$(pv rule-1.0.0); [ $? = 0 ] && rule=$((rule+1)) || echo "    publish rule (2.0.0): $(echo "$out" | tail -2 | tr '\n' '|' | cut -c1-200)"
  [ "$rule" = 8 ] && ok || bad "publish: the version rule held in $rule of 8 cases"
  # a new version that does not compile is not published
  nextv rule-0.1.0 0.1.0 0.3.0 's/returns String$/returns Strin/'
  out=$(pv rule-0.1.0); rc=$?
  [ "$rc" = 1 ] && echo "$out" | grep -q 'demo 0.3.0 does not compile' && ok || bad "publish, a version that does not compile: rc=$rc '$(echo "$out" | tail -1 | cut -c1-120)'"
fi

# clean: the cache is emptied, what was fetched stays, and the next build
# makes what it needs again
before=$(find "$SCALY_CACHE" -type f 2>/dev/null | wc -l | tr -d ' ')
out=$(SCALY_HOME="$here" "$scaly_abs" clean 2>&1); rc=$?
after=$(find "$SCALY_CACHE" -mindepth 1 2>/dev/null | wc -l | tr -d ' ')
[ "$rc" = 0 ] && [ "$before" -gt 0 ] && [ "$after" = 0 ] && echo "$out" | grep -q "^clean: $before files removed" && ok || bad "clean: rc=$rc, $before files before, $after entries after, '$(echo "$out" | head -1 | cut -c1-100)'"
( cd "$ROOT" && SCALY_HOME="$here" "$scaly_abs" build tests/tool/hello.scaly -o "$TMP/after_clean$SCALY_EXE" ) > /dev/null 2>&1
out=$("$TMP/after_clean$SCALY_EXE" a b 2>&1)
[ "$out" = "hello a b" ] && [ "$(find "$SCALY_CACHE" -type f | wc -l | tr -d ' ')" -gt 0 ] && ok || bad "clean: the build after it: '$(echo "$out" | head -1 | cut -c1-100)'"

echo "tool: $pass PASS, $fail FAIL"
for x in "${failures[@]+"${failures[@]}"}"; do echo "  $x"; done
[ "$fail" = 0 ]
