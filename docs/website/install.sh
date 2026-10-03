#!/bin/sh
# Scaly installer — https://scaly.io
#
#   curl -fsSL https://scaly.io/install.sh | sh
#
# Installs Scaly into ~/.scaly with NO sudo — three programs:
#   scaly     the tool: `scaly run`, `scaly build`, `scaly test`; alone, the REPL
#   scalyc    the compiler, with a C compiler's command line (-o -c -S -O2)
#   scalyls   the language server the VS Code extension starts
# and beside them the standard library and the standard packages (http, https,
# tls, json, compress, pg, redis, h3) as sources.
#
# The programs are downloaded ready to run for
#   macOS 26 on Apple silicon, Ubuntu 26.04 and newer (arm64, x86-64)
# and need nothing but a C compiler, which links the programs you build
# (macOS: xcode-select --install; Ubuntu: sudo apt install clang).
#
# On any other macOS or Linux, Scaly is BUILT here from its seed — the three
# programs as LLVM IR — for which LLVM 21 must be present:
#   macOS    brew install llvm@21
#   Ubuntu   sudo apt install llvm-21 llvm-21-dev lld-21 clang
#
# Environment overrides:
#   SCALY_PREFIX          install location           (default ~/.scaly)
#   SCALY_VERSION         version to fetch           (default 0.1.0)
#   SCALY_INSTALL_BASE    base URL for downloads     (default https://scaly.io)
#   SCALY_NO_MODIFY_PATH  set to 1 to skip editing your shell profile
#   SCALY_FROM_SEED       set to 1 to build from the seed in any case
#   LLVM21                path to an LLVM 21 install (building from the seed)
#   CC                    the C compiler to use      (default clang)
#
# Layout:  ~/.scaly/bin        the three commands (put on PATH): links to
#                              the programs, which find their packages
#                              beside themselves -- no script, no variable
#          ~/.scaly/toolchain  the programs, the packages, the seed
#          ~/.scaly/cache      compiled packages (made on first use)
# Running the script again replaces the toolchain; a run that fails leaves the
# installed one as it was.
#
# Uninstall: rm -rf ~/.scaly  and remove the PATH line this script adds.
#
# ★The seed route repeats the recipe of tools/build-from-seed.sh in the source
# tree and must follow it; tests/install/run.sh installs by both routes from
# archives made of the tree and runs what the installed programs must be able
# to do.
set -eu

VERSION="${SCALY_VERSION:-0.1.0}"
PREFIX="${SCALY_PREFIX:-$HOME/.scaly}"
BASE_URL="${SCALY_INSTALL_BASE:-https://scaly.io}"
TARBALL="scaly-$VERSION.tar.gz"
LLVM_MAJOR=21

say()  { printf 'scaly-install: %s\n' "$1"; }
die()  { printf 'scaly-install: error: %s\n' "$1" >&2; exit 1; }
have() { command -v "$1" >/dev/null 2>&1; }

OS="$(uname -s)"
case "$OS" in
  Darwin|Linux) ;;
  *) die "this installer covers macOS and Linux; $OS is not one of them" ;;
esac

SYSTEM="$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)"

have curl || have wget || die "need curl or wget to download the distribution"
have tar || die "need tar to unpack the distribution"

# fetch <url> <file>: quiet, and a failure is the caller's to explain
fetch() {
  if have curl; then curl -fsSL "$1" -o "$2" 2>/dev/null
  else wget -q -O "$2" "$1" 2>/dev/null; fi
}

# ---------------------------------------------------------------------------
# 1. A C compiler: it links the programs you build and compiles a package's C
#    files; the seed route also builds Scaly itself with it. clang first — the
#    installed programs call `clang` themselves when nothing else is named.
# ---------------------------------------------------------------------------
CC="${CC:-}"
if [ -z "$CC" ]; then
  for c in clang "clang-$LLVM_MAJOR" cc gcc; do
    if have "$c"; then CC="$c"; break; fi
  done
fi
if [ -n "$CC" ] && ! have "$CC"; then die "the C compiler $CC was not found"; fi

prerequisites() {
  cat >&2 <<EOF
scaly-install: error: $1
  There are no ready-made programs for this system ($SYSTEM), so Scaly is
  built here from its seed, with LLVM $LLVM_MAJOR:
    macOS:   brew install llvm@$LLVM_MAJOR
    Ubuntu:  sudo apt install llvm-$LLVM_MAJOR llvm-$LLVM_MAJOR-dev lld-$LLVM_MAJOR clang
  Then run this again, or set LLVM$LLVM_MAJOR=/path/to/llvm if it lives elsewhere.
EOF
  exit 1
}

# llvm_tool <name>: the prefix's own, else the versioned name on PATH
llvm_tool() {
  for cand in "$LLVM_PREFIX/bin/$1" "$LLVM_PREFIX/bin/$1-$LLVM_MAJOR" "$1-$LLVM_MAJOR"; do
    if have "$cand"; then printf '%s\n' "$cand"; return 0; fi
  done
  return 1
}

# What the seed route needs: LLVM 21 (llc, opt, llvm-link, libLLVM), ar, and
# on Linux lld.
find_llvm() {
  LLVM_PREFIX=""
  if [ -n "${LLVM21:-}" ]; then
    LLVM_PREFIX="$LLVM21"
  elif have brew && brew --prefix "llvm@$LLVM_MAJOR" >/dev/null 2>&1; then
    LLVM_PREFIX="$(brew --prefix "llvm@$LLVM_MAJOR")"
  elif [ -d "/usr/lib/llvm-$LLVM_MAJOR" ]; then
    LLVM_PREFIX="/usr/lib/llvm-$LLVM_MAJOR"
  elif have "llvm-config-$LLVM_MAJOR"; then
    LLVM_PREFIX="$("llvm-config-$LLVM_MAJOR" --prefix)"
  fi
  [ -n "$LLVM_PREFIX" ] && [ -d "$LLVM_PREFIX" ] || prerequisites "LLVM $LLVM_MAJOR not found."

  # llc, opt and llvm-link come in one package on both systems (Ubuntu's
  # llvm-21, NOT llvm-21-dev; a libLLVM another package pulled in is not the tools).
  LLC="$(llvm_tool llc)"             || prerequisites "llc of LLVM $LLVM_MAJOR not found (Ubuntu: the package llvm-$LLVM_MAJOR)."
  OPT="$(llvm_tool opt)"             || prerequisites "opt of LLVM $LLVM_MAJOR not found (Ubuntu: the package llvm-$LLVM_MAJOR)."
  LLVM_LINK="$(llvm_tool llvm-link)" || prerequisites "llvm-link of LLVM $LLVM_MAJOR not found (Ubuntu: the package llvm-$LLVM_MAJOR)."

  LLVM_LIBDIR="${LLVM_LIBDIR:-$LLVM_PREFIX/lib}"
  LLVM_LIBNAME="${LLVM_LIBNAME:-LLVM-$LLVM_MAJOR}"
  ls "$LLVM_LIBDIR"/lib"$LLVM_LIBNAME".* >/dev/null 2>&1 \
    || prerequisites "lib$LLVM_LIBNAME not found in $LLVM_LIBDIR (Ubuntu: the package llvm-$LLVM_MAJOR-dev)."

  if [ -z "$CC" ] && have "$LLVM_PREFIX/bin/clang"; then CC="$LLVM_PREFIX/bin/clang"; fi
  [ -n "$CC" ] || prerequisites "no C compiler found (clang, clang-$LLVM_MAJOR, cc or gcc)."

  AR=""
  for c in ar "$LLVM_PREFIX/bin/llvm-ar" "llvm-ar-$LLVM_MAJOR"; do
    if have "$c"; then AR="$c"; break; fi
  done
  [ -n "$AR" ] || die "no ar found (ar or llvm-ar)"

  # On Linux the GNU linker fails to link libLLVM ("failed to set dynamic
  # section sizes"); lld links it. And an ELF executable exports a symbol to
  # its own in-process JIT only when asked to (-rdynamic) — `scaly run`,
  # `scaly test` and the REPL find the runtime in the program itself.
  LINK_FLAGS=""
  if [ "$OS" = "Linux" ]; then
    LLD=""
    for c in "$LLVM_PREFIX/bin/ld.lld" "ld.lld-$LLVM_MAJOR" ld.lld; do
      if have "$c"; then LLD="$(command -v "$c")"; break; fi
    done
    [ -n "$LLD" ] || prerequisites "lld not found (Ubuntu: the package lld-$LLVM_MAJOR)."
    LINK_FLAGS="-fuse-ld=$LLD -rdynamic"
  fi
  say "LLVM $LLVM_MAJOR at $LLVM_PREFIX, C compiler $CC"
}

# ---------------------------------------------------------------------------
# 2. Download and unpack into a NEW toolchain directory beside the installed
#    one; it takes its place only when everything below succeeded. First what
#    is the same for every system: the packages and the seed.
# ---------------------------------------------------------------------------
WORK="$(mktemp -d)"
NEW="$PREFIX/toolchain.new"
TOOLCHAIN="$PREFIX/toolchain"
trap 'rm -rf "$WORK" "$NEW"' EXIT

URL="$BASE_URL/downloads/$TARBALL"
say "downloading $URL"
fetch "$URL" "$WORK/$TARBALL" || die "download failed: $URL"

rm -rf "$NEW"
mkdir -p "$NEW"
tar -xzf "$WORK/$TARBALL" -C "$NEW" || die "could not unpack $TARBALL"
SEED="$NEW/seed"
[ -f "$SEED/scalyc.ll" ] || die "distribution missing seed/scalyc.ll"

STDLIB=""
for d in "$NEW"/packages/scaly/*/; do
  [ -f "$d/scaly.scaly" ] && STDLIB="${d%/}"
done
[ -n "$STDLIB" ] || die "distribution missing the standard library (packages/scaly)"

mkdir -p "$WORK/check"
printf 'print("Hello, World!")\n' > "$WORK/check/hello.scaly"

# ---------------------------------------------------------------------------
# 3. The programs, ready-made for this system when there are any: an archive
#    of libexec/ and lib/. They are taken only if they START here — an older
#    macOS or C library than they were built for sends us to the seed instead.
# ---------------------------------------------------------------------------
READY=0
if [ "${SCALY_FROM_SEED:-0}" != "1" ]; then
  BIN_TARBALL="scaly-$VERSION-$SYSTEM.tar.gz"
  if fetch "$BASE_URL/downloads/$BIN_TARBALL" "$WORK/$BIN_TARBALL"; then
    if tar -xzf "$WORK/$BIN_TARBALL" -C "$NEW" 2>/dev/null \
       && SCALY_HOME="$NEW" "$NEW/libexec/scalyc" -S -o "$WORK/check/probe.ll" "$WORK/check/hello.scaly" >/dev/null 2>&1; then
      READY=1
      say "installed the programs for $SYSTEM"
    else
      rm -rf "$NEW/libexec" "$NEW/lib"
      say "the ready-made programs do not start on this system; building from the seed instead"
    fi
  else
    say "no ready-made programs for $SYSTEM; building from the seed instead"
  fi
fi

if [ "$READY" = 0 ]; then
find_llvm

# shasum is a Perl script and a fresh Ubuntu has only coreutils' sha256sum.
if have sha256sum; then SHA256="sha256sum"
elif have shasum; then SHA256="shasum -a 256"
else SHA256=""; fi
if [ -n "$SHA256" ] && [ -f "$SEED/SHA256SUMS" ]; then
  ( cd "$SEED" && $SHA256 -c SHA256SUMS >/dev/null 2>&1 ) || die "seed checksum mismatch — the download is damaged"
fi

# ---------------------------------------------------------------------------
# 4. (seed) The runtime's native files: the fiber context switch (assembly, one file
#    per ABI), evented I/O, civil time and the catch point (C). Every program
#    that contains the standard library links them.
# ---------------------------------------------------------------------------
case "$(uname -m)" in
  arm64|aarch64) FCONTEXT="$STDLIB/scaly/fiber/fcontext_arm64.S" ;;
  x86_64|amd64)  FCONTEXT="$STDLIB/scaly/fiber/fcontext_x86_64.S" ;;
  *) die "unsupported architecture $(uname -m) (arm64 and x86_64 are)" ;;
esac
say "compiling the runtime's native files"
"$CC" -c "$FCONTEXT" -o "$WORK/fcontext.o"                         || die "assembling $(basename "$FCONTEXT") failed"
"$CC" -O2 -c "$STDLIB/scaly/fiber/eio.c"   -o "$WORK/eio.o"        || die "compiling eio.c failed"
"$CC" -O2 -c "$STDLIB/scaly/time/ctime.c"  -o "$WORK/ctime.o"      || die "compiling ctime.c failed"
"$CC" -O2 -c "$STDLIB/scaly/memory/panic.c" -o "$WORK/panic.o"     || die "compiling panic.c failed"

# ---------------------------------------------------------------------------
# 5. (seed) The three programs, each its roots linked into one module and optimised
#    as a whole. scalyc and scaly run programs in process (--jit, `scaly run`,
#    `scaly test`, the REPL), and such a program finds the runtime IN the
#    program that runs it — so for those two every definition is kept
#    (linkonce_odr -> weak_odr, before the modules are linked) and stays a
#    visible symbol (unnamed_addr removed, after the optimiser has used it).
# ---------------------------------------------------------------------------
# build_program <name> <keeps runtime visible: 1|0> <root>...
build_program() {
  name="$1"; visible="$2"; shift 2
  inputs=""
  for f in "$@"; do
    if [ "$visible" = 1 ]; then
      sed 's/^define linkonce_odr /define weak_odr /' "$SEED/$f.ll" > "$WORK/$name.$f.ll"
    else
      cp "$SEED/$f.ll" "$WORK/$name.$f.ll"
    fi
    inputs="$inputs $WORK/$name.$f.ll"
  done
  # shellcheck disable=SC2086
  "$LLVM_LINK" -S $inputs -o "$WORK/$name.linked.ll"
  "$OPT" -O2 -S "$WORK/$name.linked.ll" -o "$WORK/$name.opt.ll"
  if [ "$visible" = 1 ]; then
    sed -e '/^define /s/local_unnamed_addr //g' -e '/^define /s/unnamed_addr //g' \
      "$WORK/$name.opt.ll" > "$WORK/$name.final.ll"
  else
    mv "$WORK/$name.opt.ll" "$WORK/$name.final.ll"
  fi
  # -relocation-model=pic: x86-64 Linux links position-independent executables.
  "$LLC" -relocation-model=pic -filetype=obj "$WORK/$name.final.ll" -o "$WORK/$name.o"
  # -lm last: on Linux the math functions are a library of their own, and the
  # linker takes from a library only what is still undefined when it reads it.
  # shellcheck disable=SC2086
  "$CC" $LINK_FLAGS "$WORK/$name.o" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" "$WORK/panic.o" \
    -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -lm -o "$NEW/libexec/$name"
}

mkdir -p "$NEW/libexec" "$NEW/lib"
say "building scalyc, scaly and scalyls from the seed (about a minute)"
( build_program scalyc  1 main scalyc scaly )                      > "$WORK/scalyc.log"  2>&1 & pid_c=$!
( build_program scaly   1 scaly_main scalyc scaly )                > "$WORK/scaly.log"   2>&1 & pid_t=$!
( build_program scalyls 0 scalyls_main scalyls json scalyc scaly ) > "$WORK/scalyls.log" 2>&1 & pid_l=$!
failed=""
wait "$pid_c" || failed="$failed scalyc"
wait "$pid_t" || failed="$failed scaly"
wait "$pid_l" || failed="$failed scalyls"
if [ -n "$failed" ]; then
  for name in $failed; do
    printf 'scaly-install: building %s failed:\n' "$name" >&2
    tail -20 "$WORK/$name.log" >&2
  done
  die "could not build:$failed"
fi

# ---------------------------------------------------------------------------
# 6. (seed) The runtime archive `scalyc -o` links a program against (`scaly build`
#    takes its packages out of the build cache instead). Optimised outside the
#    compiler: a library has no main to anchor it, and the in-process -O2
#    would delete every body.
# ---------------------------------------------------------------------------
say "building the runtime archive"
SCALY_HOME="$NEW" "$NEW/libexec/scalyc" -S --no-prelude --no-tests \
  -o "$WORK/libscaly.ll" "$STDLIB/scaly.scaly" > "$WORK/libscaly.log" 2>&1 \
  || { tail -20 "$WORK/libscaly.log" >&2; die "emitting the standard library failed"; }
sed 's/^define linkonce_odr /define weak_odr /' "$WORK/libscaly.ll" > "$WORK/libscaly_weak.ll"
"$OPT" -O2 "$WORK/libscaly_weak.ll" -o "$WORK/libscaly_opt.bc" || die "optimising the standard library failed"
"$LLC" -relocation-model=pic -O2 -filetype=obj "$WORK/libscaly_opt.bc" -o "$WORK/libscaly.o" \
  || die "compiling the standard library failed"
"$AR" rcs "$NEW/lib/libscaly.a" "$WORK/libscaly.o" "$WORK/fcontext.o" "$WORK/eio.o" "$WORK/ctime.o" "$WORK/panic.o" \
  || die "ar failed"
fi   # built from the seed

# ---------------------------------------------------------------------------
# 7. The new toolchain takes the old one's place, and the three commands are
#    LINKS to its programs. A program finds the packages beside itself
#    (<toolchain>/libexec/<name> -> <toolchain>/packages, the link resolved
#    first), so nothing sets SCALY_HOME; a SCALY_HOME that is set still wins.
#    The links are relative: the whole prefix can be moved.
# ---------------------------------------------------------------------------
# an installation of the first layout (everything directly under the prefix)
if [ -f "$PREFIX/seed/scalyc.ll" ]; then
  rm -rf "$PREFIX/seed" "$PREFIX/libexec" "$PREFIX/lib" "$PREFIX/packages/scaly" "$PREFIX/LICENSE" "$PREFIX/VERSION"
  rmdir "$PREFIX/packages" 2>/dev/null || true
fi
# The programs call `clang` to link and to compile a package's C files unless
# $SCALY_CC or $CC names another; say so only where `clang` is not what was
# found: the file CC in the toolchain, which a program reads as its SCALY_CC
# when the variable is not set.
if [ -n "$CC" ] && [ "$CC" != "clang" ]; then
  printf '%s\n' "$CC" > "$NEW/CC"
fi
rm -rf "$TOOLCHAIN"
mv "$NEW" "$TOOLCHAIN"

BINDIR="$PREFIX/bin"
mkdir -p "$BINDIR"
for name in scaly scalyc scalyls; do
  rm -f "$BINDIR/$name"     # the script of an earlier installation
  ln -s "../$(basename "$TOOLCHAIN")/libexec/$name" "$BINDIR/$name"
done

# ---------------------------------------------------------------------------
# 8. Prove it: a program run through `scaly run`, and one built and run.
#    (The first build also compiles the standard library into the cache, so
#    the next one is quick.)
# ---------------------------------------------------------------------------
say "checking the installation"
[ "$(cd "$WORK/check" && "$BINDIR/scaly" run hello.scaly 2>"$WORK/check/run.log")" = "Hello, World!" ] \
  || { tail -20 "$WORK/check/run.log" >&2; die "the installed scaly could not run a program"; }
if [ -n "$CC" ]; then
  ( cd "$WORK/check" && "$BINDIR/scaly" build hello.scaly -o hello ) > "$WORK/check/build.log" 2>&1 \
    || { tail -20 "$WORK/check/build.log" >&2; die "the installed scaly could not build a program"; }
  [ "$("$WORK/check/hello")" = "Hello, World!" ] || die "the program the installed scaly built does not run"
else
  say "NOTE: no C compiler found. scaly run, scaly test and the REPL work;"
  if [ "$OS" = "Darwin" ]; then
    say "      scaly build needs one to link:  xcode-select --install"
  else
    say "      scaly build needs one to link:  sudo apt install clang"
  fi
fi

# ---------------------------------------------------------------------------
# 9. Put the commands on PATH via the shell profile (unless already there).
# ---------------------------------------------------------------------------
case ":$PATH:" in
  *":$BINDIR:"*) say "$BINDIR is on your PATH" ;;
  *)
    if [ "${SCALY_NO_MODIFY_PATH:-0}" = "1" ]; then
      say "add this to your shell profile:  export PATH=\"$BINDIR:\$PATH\""
    else
      case "${SHELL:-}" in
        */zsh)  PROFILE="$HOME/.zshrc" ;;
        */bash) PROFILE="$HOME/.bashrc" ;;
        *)      PROFILE="$HOME/.profile" ;;
      esac
      LINE="export PATH=\"$BINDIR:\$PATH\"  # added by scaly-install"
      if [ -f "$PROFILE" ] && grep -qF "$BINDIR" "$PROFILE" 2>/dev/null; then
        say "$PROFILE already names $BINDIR"
      else
        printf '\n%s\n' "$LINE" >> "$PROFILE"
        say "added $BINDIR to PATH in $PROFILE"
      fi
      say "open a new terminal, or run:  export PATH=\"$BINDIR:\$PATH\""
    fi
    ;;
esac

cat <<EOF

scaly-install: done. Scaly $VERSION is installed in $PREFIX.
  Try it:
    printf 'print("Hello, World!")\\n' > hello.scaly
    scaly run hello.scaly              run it
    scaly build hello.scaly -o hello   build a program
    scaly                              the REPL (:help, :quit)
  Uninstall:  rm -rf "$PREFIX"  (and remove the PATH line above)
EOF
