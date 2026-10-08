#!/bin/bash
# tests/install/run-windows.sh — tests/install/run.sh for the Windows box (Git
# Bash): the two archives made of this tree — the packages and the seed
# (tools/make-dist.sh), the programs with LLVM linked in and the stdlib's
# ready-made native objects (tools/make-bindist-windows.sh) — installed by
# docs/website/install.ps1 into a scratch prefix, and what a newcomer does
# with the result. ★Everything after `install` runs on a BARE environment: a
# PATH of Git Bash's own tools and System32 (no clang, no LLVM), neither LIB
# nor INCLUDE — what a machine with the Build Tools and nothing else has.
# It is not a fresh Windows: the
# box has the Build Tools, and that is the one thing `scaly build` may use.
#   payload    the tarball carries the seed and the standard packages and none
#              of the compiler's, the language server's or the ports' sources
#   programs   the zip is built; its programs load nothing but Windows' own
#              DLLs (make-bindist-windows checks it) and it holds the ready-
#              made objects
#   install    install.ps1 ends with rc 0 (its own check ran a program and
#              built one), leaves toolchain\ and no toolchain.new, and touched
#              neither the user's PATH nor a SCALY_HOME (SCALY_NO_MODIFY_PATH)
#   run        `scaly run`, arguments handed on (the JIT)
#   build      `scaly build`, the program run with arguments; no native file
#              was compiled for it
#   release    `scaly build --release`, the binary smaller than the plain one
#   scalyc     `scalyc -o` links against the installed runtime archive
#   json       a program using the json package builds and runs
#   test       `scaly test` names a failing test and answers rc 1
#   repl       `scaly` alone answers an expression
#   scalyls    the language server answers `initialize`
#   again      a second installation over the first leaves a working one
#   moved      the toolchain directory copied elsewhere builds and runs a
#              program: nothing names where it was installed
# ★SCALY_HOME is UNSET throughout: the programs find their packages beside
# themselves (cli.adopt_home).
# Not here: the installer's winget route (it installs the Build Tools
# system-wide), and a build from the seed (the POSIX installer's fallback;
# the Windows one has none).
# Not in the bar: about five minutes of whole-program links.
# Nothing is written into the tree; the user's %USERPROFILE%\.scaly, PATH and
# SCALY_HOME are not touched.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
set -u
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) ;;
  *) echo "install-windows: SKIP (the Windows installer; tests/install/run.sh is this system's)"; exit 0 ;;
esac
cd "$ROOT"
. tools/win-env.sh || exit 1

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
PREFIX="$TMP/prefix"
T="$PREFIX/toolchain"
B="$T/libexec"
case "$SCALY_WIN_TRIPLE" in
  aarch64*) ARCH=arm64 ;;
  *)        ARCH=x86_64 ;;
esac

# by its full path: the bare PATH below does not hold it
PS="$(cygpath -u "${SYSTEMROOT:-C:\Windows}")/System32/WindowsPowerShell/v1.0/powershell.exe"

pass=0; fail=0; failures=()
ok() { pass=$((pass+1)); }
bad() { fail=$((fail+1)); failures+=("$1"); }
finish() {
  echo "install-windows: $pass PASS, $fail FAIL"
  if [ "$fail" -gt 0 ]; then printf '  %s\n' "${failures[@]}"; exit 1; fi
  exit 0
}

install() {
  SCALY_PREFIX="$(cygpath -w "$PREFIX")" SCALY_INSTALL_BASE="$(cygpath -w "$TMP/downloads")" \
  SCALY_NO_MODIFY_PATH=1 SCALY_BUILD_TOOLS=skip \
    "$PS" -NoProfile -ExecutionPolicy Bypass -File "$(cygpath -w "$ROOT/docs/website/install.ps1")" > "$1" 2>&1
}
user_env() {
  "$PS" -NoProfile -Command "[Environment]::GetEnvironmentVariable('SCALY_HOME','User') + '|' + [Environment]::GetEnvironmentVariable('Path','User')" | tr -d '\r'
}

# $INSTALL_WINDOWS_ARCHIVES: a directory that already holds the two archives
# (made by the two scripts below); they are taken as they are -- for working
# on the installer without the five minutes of links each time.
make_dist() {
  if [ -n "${INSTALL_WINDOWS_ARCHIVES:-}" ]; then
    mkdir -p "$TMP/downloads" && cp "$INSTALL_WINDOWS_ARCHIVES/scaly-0.1.0.tar.gz" "$TMP/downloads/"
  else
    tools/make-dist.sh 0.1.0 "$TMP/downloads"
  fi
}
make_bindist() {
  if [ -n "${INSTALL_WINDOWS_ARCHIVES:-}" ]; then
    cp "$INSTALL_WINDOWS_ARCHIVES/scaly-0.1.0-windows-$ARCH.zip" "$TMP/downloads/"
  else
    tools/make-bindist-windows.sh 0.1.0 "$TMP/downloads"
  fi
}

# payload
if make_dist > "$TMP/dist.log" 2>&1; then
  tar -tzf "$TMP/downloads/scaly-0.1.0.tar.gz" > "$TMP/payload.txt"
  private=$(grep -cE 'packages/(scalyc|scalyls|tscaly|scalygpu)/|\.o$|/\._|CLAUDE' "$TMP/payload.txt")
  missing=""
  for f in seed/scalyc.ll seed/SHA256SUMS packages/scaly/0.1.0/scaly.scaly packages/http/0.1.0/http.scaly \
           packages/json/0.1.0/json.scaly LICENSE VERSION; do
    grep -q "^\./$f" "$TMP/payload.txt" || missing="$missing $f"
  done
  [ "$private" = 0 ] && [ -z "$missing" ] && ok || bad "payload: $private entries that must not ship; missing:$missing"
else
  bad "payload: make-dist failed: $(tail -2 "$TMP/dist.log" | tr '\n' ' ')"
fi

# programs
ZIP="$TMP/downloads/scaly-0.1.0-windows-$ARCH.zip"
if make_bindist > "$TMP/bindist.log" 2>&1; then
  n=$("$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32/tar.exe" -tf "$(cygpath -w "$ZIP")" | tr -d '\r' | grep -c "_native/windows-$ARCH/.*\.o$")
  [ "$n" -ge 4 ] && ok || bad "programs: the zip holds $n ready-made objects"
else
  bad "programs: make-bindist-windows failed: $(tail -3 "$TMP/bindist.log" | tr '\n' ' ')"
  finish
fi

# install
before=$(user_env)
if install "$TMP/install.log"; then
  after=$(user_env)
  grep -q 'scaly run: ok' "$TMP/install.log" \
    && [ -f "$B/scaly.exe" ] && [ -f "$B/scalyc.exe" ] && [ -f "$B/scalyls.exe" ] && [ -d "$T/packages/scaly" ] \
    && [ -f "$T/lib/libscaly.lib" ] && [ ! -e "$PREFIX/toolchain.new" ] && [ "$before" = "$after" ] \
    && ok || bad "install: rc 0 but the prefix is incomplete or the user's environment changed"
else
  bad "install: rc=$? $(tail -4 "$TMP/install.log" | tr '\n' ' ')"
  finish
fi

# everything below from a directory that is not the tree, on a bare environment
mkdir -p "$TMP/work"
cd "$TMP/work" || exit 1
printf 'print("Hello, World!")\n' > hello.scaly
unset LIB INCLUDE SCALY_CC CC
export PATH="/usr/bin:/bin:$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32"
unset SCALY_HOME
export SCALY_CACHE="$(cygpath -m "$TMP/cache")"

# run
out=$("$B/scaly.exe" run "$ROOT/tests/tool/hello.scaly" one two 2> run.log)
[ "$out" = "hello one two" ] && ok || bad "run: got '$out' $(tail -2 run.log | tr '\n' ' ')"

# build
if "$B/scaly.exe" build "$ROOT/tests/tool/hello.scaly" -o h_b.exe > build.log 2>&1; then
  out=$(./h_b.exe one two)
  n=$(ls "$TMP/cache" | grep -c '\.[cS]\.o$')
  [ "$out" = "hello one two" ] && [ "$n" = 0 ] && ok || bad "build: got '$out', $n native objects compiled"
else
  bad "build: rc=$? $(tail -3 build.log | tr '\n' ' ')"
fi

# release
if "$B/scaly.exe" build "$ROOT/tests/tool/hello.scaly" -o h_r.exe --release > release.log 2>&1; then
  out=$(./h_r.exe one two)
  plain=$(wc -c < h_b.exe); rel=$(wc -c < h_r.exe)
  [ "$out" = "hello one two" ] && [ "$rel" -lt "$plain" ] && ok || bad "release: got '$out', $rel bytes against $plain"
else
  bad "release: rc=$? $(tail -3 release.log | tr '\n' ' ')"
fi

# scalyc
if "$B/scalyc.exe" -o h_c.exe hello.scaly > scalyc.log 2>&1; then
  [ "$(./h_c.exe)" = "Hello, World!" ] && ok || bad "scalyc: wrong output"
else
  bad "scalyc: rc=$? $(tail -3 scalyc.log | tr '\n' ' ')"
fi

# json
if "$B/scaly.exe" build "$ROOT/tests/json/value.scaly" -o h_j.exe > json.log 2>&1; then
  want=$(sed -n 's/^; Expected: //p' "$ROOT/tests/json/value.scaly" | head -1)
  out=$(./h_j.exe 2>&1 | tail -1)
  [ -n "$want" ] && [ "$out" = "$want" ] && ok || bad "json: got '$out', expected '$want'"
else
  bad "json: rc=$? $(tail -3 json.log | tr '\n' ' ')"
fi

# test
"$B/scaly.exe" test "$ROOT/tests/tool/sums.scaly" > test.log 2>&1; rc=$?
[ "$rc" = 1 ] && grep -q 'test_wrong ... FAIL' test.log && grep -q 'test_sum ... ok' test.log \
  && ok || bad "test: rc=$rc $(tail -2 test.log | tr '\n' ' ')"

# repl
out=$(printf '1 + 2\n:quit\n' | "$B/scaly.exe" 2>&1)
case "$out" in
  *"it: int = 3"*) ok ;;
  *) bad "repl: got '$out'" ;;
esac

# scalyls
body='{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"capabilities":{}}}'
out=$(printf 'Content-Length: %d\r\n\r\n%s' "${#body}" "$body" | "$B/scalyls.exe" 2> scalyls.log | head -c 400)
case "$out" in
  *'"capabilities"'*) ok ;;
  *) bad "scalyls: no answer to initialize: '$out'" ;;
esac

# again: over the first, with the Build Tools check on (the box has them)
cd "$ROOT"
if ( SCALY_PREFIX="$(cygpath -w "$PREFIX")" SCALY_INSTALL_BASE="$(cygpath -w "$TMP/downloads")" \
     SCALY_NO_MODIFY_PATH=1 "$PS" -NoProfile -ExecutionPolicy Bypass \
       -File "$(cygpath -w "$ROOT/docs/website/install.ps1")" < /dev/null > "$TMP/again.log" 2>&1 ); then
  grep -q 'scaly build: ok' "$TMP/again.log" && [ ! -e "$PREFIX/toolchain.new" ] && [ ! -e "$PREFIX/toolchain.old" ] \
    && [ "$("$B/scaly.exe" run "$TMP/work/hello.scaly" 2>/dev/null)" = "Hello, World!" ] \
    && ok || bad "again: the second installation is incomplete: $(tail -3 "$TMP/again.log" | tr '\n' ' ')"
else
  bad "again: rc=$? $(tail -4 "$TMP/again.log" | tr '\n' ' ')"
fi

# moved
cp -R "$T" "$TMP/elsewhere"
cd "$TMP/work" || exit 1
if "$TMP/elsewhere/libexec/scaly.exe" build "$ROOT/tests/tool/hello.scaly" -o h_m.exe > moved.log 2>&1; then
  out=$(./h_m.exe one two)
  [ "$out" = "hello one two" ] && [ "$("$TMP/elsewhere/libexec/scaly.exe" run hello.scaly 2>/dev/null)" = "Hello, World!" ] \
    && ok || bad "moved: got '$out'"
else
  bad "moved: rc=$? $(tail -3 moved.log | tr '\n' ' ')"
fi

finish
