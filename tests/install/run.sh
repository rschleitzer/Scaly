#!/bin/bash
# tests/install/run.sh — the public installer (ROADMAP-public.md, stage F):
# the two archives made of this tree — the packages and the seed
# (tools/make-dist.sh), the programs for this system with LLVM linked in
# (tools/make-bindist.sh) — installed by docs/website/install.sh into a scratch
# prefix, and what a newcomer does with the result, from a directory OUTSIDE
# the tree with SCALY_HOME unset.
#   payload    the tarball carries the seed and the standard packages and none
#              of the compiler's, the language server's or the ports' sources
#   programs   the archive of programs is built, and they load nothing but
#              the system's own libraries (make-bindist checks it). With
#              $SCALY_PGO_PROFILE set (tools/make-profile.sh) they are the
#              programs built with the profile, and everything below runs on
#              those -- what a release hands out
#   install    the installer takes the READY-MADE programs and ends with rc 0
#              (its own check ran and built a program), leaves bin/ and
#              toolchain/ and no toolchain.new
#   scalyc     `scalyc -o` links a program against the installed runtime archive
#   build      `scaly build`, the program run with arguments
#   run        `scaly run`, arguments handed on (the in-process JIT finds the
#              runtime in the installed program)
#   release    `scaly build --release`, the binary smaller than the plain one
#   packages   a program declaring only `package https` builds: the standard
#              packages come out of the installed toolchain
#   json       a program using the json package builds and runs
#   test       `scaly test` names a failing test and answers rc 1
#   repl       `scaly` alone answers an expression
#   scalyls    the language server answers `initialize`
#   seed       a second installation over the first, offered programs that do
#              not start: the installer says so, builds from the SEED, and the
#              result runs and links
# The installer's seed route repeats the recipe of tools/build-from-seed.sh; this suite is
# what notices when the two have drifted apart (the installer on scaly.io did
# not link for weeks in 2026-09 and nothing was red).
# Not in the bar: it tests the INSTALLER, and costs six whole-program
# optimisations beside the lanes. Run it before tools/publish-install.sh.
# Nothing is written into the tree; the user's ~/.scaly is not touched.
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
set -u

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
export SCALY_CACHE="$TMP/cache"
unset SCALY_HOME
PREFIX="$TMP/prefix"
B="$PREFIX/bin"

pass=0; fail=0; failures=()
ok() { pass=$((pass+1)); }
bad() { fail=$((fail+1)); failures+=("$1"); }

# install <base directory> <log>
install() {
  SCALY_PREFIX="$PREFIX" SCALY_INSTALL_BASE="file://$1" SCALY_NO_MODIFY_PATH=1 \
    sh "$ROOT/docs/website/install.sh" > "$2" 2>&1
}

# payload
if "$ROOT/tools/make-dist.sh" 0.1.0 "$TMP/base/downloads" > "$TMP/dist.log" 2>&1; then
  tar -tzf "$TMP/base/downloads/scaly-0.1.0.tar.gz" > "$TMP/payload.txt"
  private=$(grep -cE 'packages/(scalyc|scalyls|dazzle|opensp|tscaly|scalygpu)/|\.o$|/\._|CLAUDE' "$TMP/payload.txt")
  missing=""
  for f in seed/main.ll seed/scaly_main.ll seed/scalyc.ll seed/scaly.ll seed/scalyls.ll seed/scalyls_main.ll \
           seed/json.ll seed/SHA256SUMS packages/scaly/0.1.0/scaly.scaly packages/http/0.1.0/http.scaly \
           packages/https/0.1.0/https.scaly packages/tls/0.1.0/tls.scaly packages/json/0.1.0/json.scaly \
           packages/http/0.1.0/interface/ LICENSE VERSION; do
    grep -q "^\./$f" "$TMP/payload.txt" || missing="$missing $f"
  done
  [ "$private" = 0 ] && [ -z "$missing" ] && ok || bad "payload: $private entries that must not ship; missing:$missing"
else
  bad "payload: make-dist failed: $(tail -2 "$TMP/dist.log" | tr '\n' ' ')"
fi

# programs
if "$ROOT/tools/make-bindist.sh" 0.1.0 "$TMP/base/downloads" > "$TMP/bindist.log" 2>&1; then
  ok
else
  bad "programs: make-bindist failed: $(tail -3 "$TMP/bindist.log" | tr '\n' ' ')"
fi

# install
if install "$TMP/base" "$TMP/install.log"; then
  grep -q 'installed the programs for' "$TMP/install.log" \
    && [ -x "$B/scaly" ] && [ -x "$B/scalyc" ] && [ -x "$B/scalyls" ] && [ -d "$PREFIX/toolchain/packages/scaly" ] \
    && [ ! -e "$PREFIX/toolchain.new" ] && ok || bad "install: rc 0 but not the ready-made programs, or the prefix is incomplete"
else
  bad "install: rc=$? $(tail -4 "$TMP/install.log" | tr '\n' ' ')"
  echo "install: $pass PASS, $fail FAIL"
  printf '  %s\n' "${failures[@]}"
  exit 1
fi

# everything below from a directory that is not the tree
mkdir -p "$TMP/work"
cd "$TMP/work" || exit 1
printf 'print("Hello, World!")\n' > hello.scaly

# scalyc
if "$B/scalyc" -o h_c hello.scaly > scalyc.log 2>&1; then
  [ "$(./h_c)" = "Hello, World!" ] && ok || bad "scalyc: wrong output"
else
  bad "scalyc: rc=$? $(tail -3 scalyc.log | tr '\n' ' ')"
fi

# build
if "$B/scaly" build "$ROOT/tests/tool/hello.scaly" -o h_b > build.log 2>&1; then
  out=$(./h_b one two)
  [ "$out" = "hello one two" ] && ok || bad "build: got '$out'"
else
  bad "build: rc=$? $(tail -3 build.log | tr '\n' ' ')"
fi

# run
out=$("$B/scaly" run "$ROOT/tests/tool/hello.scaly" one two 2> run.log)
[ "$out" = "hello one two" ] && ok || bad "run: got '$out' $(tail -2 run.log | tr '\n' ' ')"

# release
if "$B/scaly" build "$ROOT/tests/tool/hello.scaly" -o h_r --release > release.log 2>&1; then
  out=$(./h_r one two)
  plain=$(wc -c < h_b); rel=$(wc -c < h_r)
  [ "$out" = "hello one two" ] && [ "$rel" -lt "$plain" ] && ok || bad "release: got '$out', $rel bytes against $plain"
else
  bad "release: rc=$? $(tail -3 release.log | tr '\n' ' ')"
fi

# packages (compiled only: linking it needs OpenSSL, which the suite does not ask for)
if "$B/scalyc" -c -o transitive.o "$ROOT/tests/tool/transitive.scaly" > packages.log 2>&1; then
  ok
else
  bad "packages: rc=$? $(tail -3 packages.log | tr '\n' ' ')"
fi

# json
if "$B/scaly" build "$ROOT/tests/json/value.scaly" -o h_j > json.log 2>&1; then
  want=$(sed -n 's/^; Expected: //p' "$ROOT/tests/json/value.scaly" | head -1)
  out=$(./h_j 2>&1 | tail -1)
  [ -n "$want" ] && [ "$out" = "$want" ] && ok || bad "json: got '$out', expected '$want'"
else
  bad "json: rc=$? $(tail -3 json.log | tr '\n' ' ')"
fi

# test
"$B/scaly" test "$ROOT/tests/tool/sums.scaly" > test.log 2>&1; rc=$?
[ "$rc" = 1 ] && grep -q 'test_wrong ... FAIL' test.log && grep -q 'test_sum ... ok' test.log \
  && ok || bad "test: rc=$rc $(tail -2 test.log | tr '\n' ' ')"

# repl
out=$(printf '1 + 2\n:quit\n' | "$B/scaly" 2>&1)
case "$out" in
  *"it: int = 3"*) ok ;;
  *) bad "repl: got '$out'" ;;
esac

# scalyls
body='{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"capabilities":{}}}'
out=$(printf 'Content-Length: %d\r\n\r\n%s' "${#body}" "$body" | "$B/scalyls" 2> scalyls.log | head -c 400)
case "$out" in
  *'"capabilities"'*) ok ;;
  *) bad "scalyls: no answer to initialize: '$out'" ;;
esac

# seed: the same packages, and "programs" that cannot start
mkdir -p "$TMP/base2/downloads" "$TMP/broken/libexec" "$TMP/broken/lib"
cp "$TMP/base/downloads/scaly-0.1.0.tar.gz" "$TMP/base2/downloads/"
printf '#!/bin/sh\nexit 1\n' > "$TMP/broken/libexec/scalyc"
chmod 755 "$TMP/broken/libexec/scalyc"
system="$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)"
tar -czf "$TMP/base2/downloads/scaly-0.1.0-$system.tar.gz" -C "$TMP/broken" libexec lib
if install "$TMP/base2" "$TMP/seed.log"; then
  grep -q 'do not start on this system' "$TMP/seed.log" && grep -q 'from the seed' "$TMP/seed.log" \
    && [ ! -e "$PREFIX/toolchain.new" ] \
    && [ "$("$B/scaly" run hello.scaly 2>/dev/null)" = "Hello, World!" ] \
    && "$B/scalyc" -o h_s hello.scaly > seed_scalyc.log 2>&1 && [ "$(./h_s)" = "Hello, World!" ] \
    && ok || bad "seed: the installation from the seed does not run: $(tail -3 "$TMP/seed.log" | tr '\n' ' ')"
else
  bad "seed: rc=$? $(tail -4 "$TMP/seed.log" | tr '\n' ' ')"
fi

echo "install: $pass PASS, $fail FAIL"
if [ "$fail" -gt 0 ]; then
  printf '  %s\n' "${failures[@]}"
  exit 1
fi
