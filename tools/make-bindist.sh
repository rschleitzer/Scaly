#!/bin/bash
# Build the three programs for THIS system as the installer hands them out:
# scalyc, scaly and scalyls with LLVM linked in statically, and the runtime
# archive `scalyc -o` links against.
#
# Output: <outdir>/scaly-<version>-<system>-<machine>.tar.gz, the two names as
# `uname -s` (lower case) and `uname -m` give them — darwin-arm64,
# linux-aarch64, linux-x86_64 — which is how docs/website/install.sh asks for
# it. Payload: libexec/scalyc, libexec/scaly, libexec/scalyls, lib/libscaly.a.
# The packages and the seed are NOT in it: they are the same for every system
# and travel in tools/make-dist.sh's tarball, which the installer unpacks
# first. A system without such an archive is built from the seed there.
#
# Two steps:
#   1. tools/build-from-seed.sh with SCALY_STATIC_LLVM=1 — the recipe every
#      development build uses, with LLVM linked in. It gives all three
#      programs and the runtime archive.
#   2. With a profile, ALWAYS unless told otherwise (as
#      tools/make-bindist-windows.sh): the one $SCALY_PGO_PROFILE names --
#      made once on the fast machine and carried over, say -- else one made
#      here and now by tools/make-profile.sh with the tool of step 1 (a
#      profile belongs to its sources; dist/scalyc.profdata is not taken by
#      itself, it may be old). SCALY_PGO_PROFILE=none builds without.
#      scalyc and scaly are built
#      AGAIN, from the compiler's sources, by the tool of step 1:
#      `scaly build --pgo <profile> --export` and the same static libraries.
#      About -30 % compile time. A profile applies only
#      where the compiler is compiled from its sources, which is why the seed
#      recipe of step 1 cannot take it. scalyls stays step 1's: it holds no
#      LLVM and was not measured.
#      Making the profile needs the clang, the profile runtime and
#      llvm-profdata of LLVM 21 (Ubuntu: libclang-rt-21-dev); where they are
#      missing the script FAILS and names the way out, it does not pack a
#      slower compiler silently.
#
# Needs the static LLVM libraries beside the tools:
#   macOS    brew install llvm@21 zstd
#   Ubuntu   apt install llvm-21 llvm-21-dev lld-21 clang zlib1g-dev libzstd-dev libxml2-dev
# ★The archive runs on the system it was built on and newer ones: Homebrew's
# LLVM is built for the macOS it runs on (26, the minimum we promise), and a
# Linux build carries the glibc version of its host as its floor — build it on
# the OLDEST Ubuntu we support (26.04).
#
# Usage: tools/make-bindist.sh [version] [outdir]   (default: the VERSION file, dist)
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-$(cat VERSION)}"
OUT="${2:-dist}"
SYSTEM="$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)"
TARBALL="$OUT/scaly-$VERSION-$SYSTEM.tar.gz"

PROFILE="${SCALY_PGO_PROFILE:-}"
if [ -n "$PROFILE" ] && [ "$PROFILE" != none ] && [ ! -f "$PROFILE" ]; then echo "make-bindist: FAIL — no profile $PROFILE"; exit 1; fi

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/libexec" "$STAGE/lib"

# 1. from the seed, LLVM linked in
SCALY_STATIC_LLVM=1 tools/build-from-seed.sh "$STAGE/libexec/scalyc" > "$STAGE/build.log" 2>&1 \
  || { tail -20 "$STAGE/build.log"; echo "make-bindist: FAIL — build-from-seed"; exit 1; }
for p in scalyc scaly scalyls; do
  [ -x "$STAGE/libexec/$p" ] || { echo "make-bindist: FAIL — $p was not built"; exit 1; }
done
cp /tmp/libscaly.a "$STAGE/lib/libscaly.a"
rm -f "$STAGE/build.log"

# 2. the compiler and the tool again, from the sources, with the profile
MADE=""
if [ -z "$PROFILE" ]; then
  PROFILE="$STAGE/scalyc.profdata"
  SCALYC="$STAGE/libexec/scalyc" tools/make-profile.sh "$PROFILE" > "$STAGE/profile.log" 2>&1 \
    || { tail -10 "$STAGE/profile.log"; echo "make-bindist: FAIL — make-profile (SCALY_PGO_PROFILE=<file> names a profile made elsewhere, =none builds without one)"; exit 1; }
  rm -f "$STAGE/profile.log"
  MADE=" made on the way"
fi
if [ "$PROFILE" != none ]; then
  source tools/llvm-env.sh > /dev/null
  LLVM_CONFIG=""
  for c in "$LLVM_PREFIX/bin/llvm-config" "llvm-config-$LLVM_MAJOR"; do
    command -v "$c" >/dev/null 2>&1 && { LLVM_CONFIG="$c"; break; }
  done
  [ -n "$LLVM_CONFIG" ] || { echo "make-bindist: FAIL — llvm-config not found"; exit 1; }
  static_libs=$("$LLVM_CONFIG" --link-static --libs all | sed 's/-lPolly[A-Za-z]*//g')
  # The link line is the tool's; what build-from-seed.sh adds to its own for
  # the static link goes in as archives named outright (the tool puts them on
  # the line as they are), as -l, and — the linker flags — through a driver
  # script handed over as $SCALY_CC: the tool takes that name as ONE program
  # (it quotes it), so flags cannot ride in the variable itself.
  # A C driver, not clang++: the tool compiles the stdlib's C files with the
  # same program, and clang++ would take them for C++.
  if [ "$(uname -s)" = "Darwin" ]; then
    driver_flags="-Wl,-dead_strip_dylibs"
    archives=("$(brew --prefix zstd)/lib/libzstd.a")
    system_libs=(-lz -lxml2 -lc++)
  else
    lld=""
    for c in "$LLVM_PREFIX/bin/ld.lld" "ld.lld-$LLVM_MAJOR" ld.lld; do
      command -v "$c" >/dev/null 2>&1 && { lld="$(command -v "$c")"; break; }
    done
    [ -n "$lld" ] || { echo "make-bindist: FAIL — lld not found"; exit 1; }
    driver_flags="-fuse-ld=$lld -static-libgcc"
    archives=()
    for a in libz.a libzstd.a libxml2.a libstdc++.a; do
      f="$(${CLANG:-clang} -print-file-name=$a)"
      [ -f "$f" ] || { echo "make-bindist: FAIL — $a not found (zlib1g-dev libzstd-dev libxml2-dev, libstdc++)"; exit 1; }
      archives+=("$f")
    done
    system_libs=()
  fi
  PGO="$STAGE/pgo"; mkdir -p "$PGO"
  driver="$PGO/cc"
  printf '#!/bin/sh\nexec %s %s "$@"\n' "${CLANG:-clang}" "$driver_flags" > "$driver"
  chmod 755 "$driver"
  for pair in main:scalyc scaly_main:scaly; do
    root="${pair%%:*}"; prog="${pair##*:}"
    # shellcheck disable=SC2086
    SCALY_CC="$driver" SCALY_CACHE="$PGO/cache" SCALY_HOME= \
      "$STAGE/libexec/scaly" build "packages/scalyc/0.1.0/$root.scaly" --pgo "$PROFILE" --export \
      "${archives[@]}" -L"$LLVM_LIBDIR" $static_libs "${system_libs[@]}" -o "$PGO/$prog" > "$PGO/$prog.log" 2>&1 \
      || { tail -20 "$PGO/$prog.log"; echo "make-bindist: FAIL — the profile build of $prog"; exit 1; }
    # a function whose control flow no longer matches the profile is built
    # without one: say how many, a stale profile is a slow compiler
    stale=$(grep -c 'profile data may be out of date\|function control flow change detected' "$PGO/$prog.log" || true)
    [ "$stale" = 0 ] || echo "make-bindist: NOTE — $prog: $stale warnings of a profile that does not fit (tools/make-profile.sh)"
  done
  # they replace step 1's only when they can do what step 1's can: the same
  # emission, and a program run in process (the reason for --export)
  "$STAGE/libexec/scalyc" -S --no-prelude --no-tests -o "$PGO/a.ll" packages/scaly/0.1.0/scaly.scaly
  "$PGO/scalyc" -S --no-prelude --no-tests -o "$PGO/b.ll" packages/scaly/0.1.0/scaly.scaly
  cmp -s "$PGO/a.ll" "$PGO/b.ll" || { echo "make-bindist: FAIL — the profiled compiler emits differently"; exit 1; }
  out=$(SCALY_CACHE="$PGO/cache" SCALY_HOME= "$PGO/scaly" run tests/tool/hello.scaly one two 2>/dev/null || true)
  [ "$out" = "hello one two" ] || { echo "make-bindist: FAIL — the profiled tool cannot run a program in process: '$out'"; exit 1; }
  mv "$PGO/scalyc" "$STAGE/libexec/scalyc"
  mv "$PGO/scaly" "$STAGE/libexec/scaly"
  rm -rf "$PGO"
  if [ -n "$MADE" ]; then BUILT="with a profile$MADE"; else BUILT="with the profile $PROFILE"; fi
  rm -f "$STAGE/scalyc.profdata"
else
  echo "make-bindist: NOTE — SCALY_PGO_PROFILE=none: the programs are the seed recipe's, about 30 % slower to compile with"
  BUILT="without a profile"
fi

# Nothing of LLVM, and nothing of a package manager, may be left to load.
if [ "$(uname -s)" = "Darwin" ]; then
  deps=$(otool -L "$STAGE"/libexec/* | grep -v ':$' | grep -v '^[[:space:]]*/usr/lib/' || true)
else
  deps=$(ldd "$STAGE"/libexec/* | grep -iE 'llvm|zstd|xml2|libz\.|stdc\+\+' || true)
fi
[ -z "$deps" ] || { echo "make-bindist: FAIL — the programs still load:"; echo "$deps"; exit 1; }

mkdir -p "$OUT"
COPYFILE_DISABLE=1 tar --no-xattrs -czf "$TARBALL" -C "$STAGE" libexec lib 2>/dev/null \
  || COPYFILE_DISABLE=1 tar -czf "$TARBALL" -C "$STAGE" libexec lib

echo "make-bindist: OK -> $TARBALL ($(du -h "$TARBALL" | cut -f1)), $BUILT"
