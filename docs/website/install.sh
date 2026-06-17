#!/bin/sh
# Scaly installer — https://scaly.io
#
#   curl -fsSL https://scaly.io/install.sh | sh
#
# Installs the Scaly compiler (scalyc) into ~/.scaly with NO sudo. The compiler
# ships as retargetable LLVM IR (a "seed"); this script turns it into a native
# executable on your machine, so you need LLVM 18 and a C compiler present:
#
#   macOS         brew install llvm@18
#   Ubuntu/Debian sudo apt install llvm-18 clang-18
#
# Then `scalyc -o hello hello.scaly` works from any directory.
#
# Environment overrides:
#   SCALY_PREFIX        install location           (default ~/.scaly)
#   SCALY_VERSION       version to fetch           (default 0.1.0)
#   SCALY_INSTALL_BASE  base URL for downloads     (default https://scaly.io)
#   SCALY_NO_MODIFY_PATH set to 1 to skip editing your shell profile
#   LLVM18              path to an LLVM 18 install (skips autodetection)
#
# Uninstall: rm -rf ~/.scaly  and remove the PATH line this script adds.
set -eu

VERSION="${SCALY_VERSION:-0.1.0}"
PREFIX="${SCALY_PREFIX:-$HOME/.scaly}"
BASE_URL="${SCALY_INSTALL_BASE:-https://scaly.io}"
TARBALL="scaly-$VERSION.tar.gz"

say()  { printf 'scaly-install: %s\n' "$1"; }
die()  { printf 'scaly-install: error: %s\n' "$1" >&2; exit 1; }
have() { command -v "$1" >/dev/null 2>&1; }

# ---------------------------------------------------------------------------
# 1. Locate LLVM 18 (llc + libLLVM) and a C compiler. Mirrors tools/llvm-env.sh.
# ---------------------------------------------------------------------------
LLVM_PREFIX=""
if [ -n "${LLVM18:-}" ]; then
  LLVM_PREFIX="$LLVM18"
elif have brew && brew --prefix llvm@18 >/dev/null 2>&1; then
  LLVM_PREFIX="$(brew --prefix llvm@18)"
elif [ -d /usr/lib/llvm-18 ]; then
  LLVM_PREFIX=/usr/lib/llvm-18
elif have llvm-config-18; then
  LLVM_PREFIX="$(llvm-config-18 --prefix)"
fi

LLC=""
for cand in "$LLVM_PREFIX/bin/llc" "$LLVM_PREFIX/bin/llc-18" llc-18; do
  if [ -n "$cand" ] && have "$cand"; then LLC="$cand"; break; fi
done

if [ -z "$LLVM_PREFIX" ] || [ ! -d "$LLVM_PREFIX" ] || [ -z "$LLC" ]; then
  cat >&2 <<EOF
scaly-install: error: LLVM 18 not found.
  The compiler is distributed as LLVM IR and needs LLVM 18 to build locally.
    macOS:         brew install llvm@18
    Ubuntu/Debian: sudo apt install llvm-18 clang-18
  Then re-run, or set LLVM18=/path/to/llvm-18 if it lives somewhere custom.
EOF
  exit 1
fi
LLVM_LIBDIR="${LLVM_LIBDIR:-$LLVM_PREFIX/lib}"
LLVM_LIBNAME="${LLVM_LIBNAME:-LLVM-18}"

CC="${CC:-}"
if [ -z "$CC" ]; then
  for c in cc clang gcc; do if have "$c"; then CC="$c"; break; fi; done
fi
[ -n "$CC" ] || die "no C compiler found (need cc/clang/gcc for the final link)"

have curl || have wget || die "need curl or wget to download the distribution"
have tar || die "need tar to unpack the distribution"

say "LLVM 18 at $LLVM_PREFIX (llc=$LLC); C compiler=$CC"

# ---------------------------------------------------------------------------
# 2. Download + unpack the distribution into $PREFIX.
#    Tarball payload: seed/*.ll, packages/scaly/..., LICENSE, VERSION.
# ---------------------------------------------------------------------------
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
URL="$BASE_URL/downloads/$TARBALL"
say "downloading $URL"
if have curl; then
  curl -fSL "$URL" -o "$WORK/$TARBALL" || die "download failed: $URL"
else
  wget -O "$WORK/$TARBALL" "$URL" || die "download failed: $URL"
fi

mkdir -p "$PREFIX"
tar -xzf "$WORK/$TARBALL" -C "$PREFIX" || die "could not unpack $TARBALL"
[ -f "$PREFIX/seed/scalyc.ll" ] || die "distribution missing seed/scalyc.ll"
say "unpacked Scaly $VERSION into $PREFIX"

# ---------------------------------------------------------------------------
# 3. Build the native compiler from the seed.
# ---------------------------------------------------------------------------
mkdir -p "$PREFIX/bin" "$PREFIX/libexec" "$PREFIX/lib"
say "building scalyc from seed (llc + link)"
for f in main scalyc scaly; do
  # -relocation-model=pic: x86-64 Linux links PIE and rejects llc's default
  # absolute relocations; PIC is the Mach-O default, so it's a no-op on macOS.
  "$LLC" -relocation-model=pic -filetype=obj "$PREFIX/seed/$f.ll" -o "$WORK/$f.o" \
    || die "llc failed on seed/$f.ll"
done

# On Linux, stock GNU ld (BFD) can fail to link libLLVM-18; prefer lld.
LD_ARG=""
if [ "$(uname -s)" = "Linux" ]; then
  for c in "$LLVM_PREFIX/bin/ld.lld" ld.lld ld.lld-18; do
    if have "$c"; then LD_ARG="-fuse-ld=$c"; break; fi
  done
fi
# shellcheck disable=SC2086
"$CC" $LD_ARG "$WORK/main.o" "$WORK/scalyc.o" "$WORK/scaly.o" \
  -L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME" -o "$PREFIX/libexec/scalyc" \
  || die "linking scalyc failed (is libLLVM-18 in $LLVM_LIBDIR?)"

# ---------------------------------------------------------------------------
# 4. Build the runtime archive the compiler links every program against.
#    Built from the shipped stdlib sources, into the persistent $PREFIX/lib.
# ---------------------------------------------------------------------------
say "building runtime archive lib/libscaly.a"
SCALY_HOME="$PREFIX" "$PREFIX/libexec/scalyc" -c --no-prelude --no-tests \
  -o "$PREFIX/lib/libscaly.o" "$PREFIX/packages/scaly/0.1.0/scaly.scaly" \
  || die "building libscaly.o failed"
ar rcs "$PREFIX/lib/libscaly.a" "$PREFIX/lib/libscaly.o" || die "ar failed"

# ---------------------------------------------------------------------------
# 5. Install the wrapper: sets SCALY_HOME so the prelude/stdlib resolve from
#    any directory, self-heals the runtime archive, and execs the compiler.
# ---------------------------------------------------------------------------
cat > "$PREFIX/bin/scalyc" <<EOF
#!/bin/sh
# Scaly compiler wrapper — installed by https://scaly.io/install.sh
export SCALY_HOME="\${SCALY_HOME:-$PREFIX}"
if [ ! -f "\$SCALY_HOME/lib/libscaly.a" ]; then
    "\$SCALY_HOME/libexec/scalyc" -c --no-prelude --no-tests \\
        -o "\$SCALY_HOME/lib/libscaly.o" "\$SCALY_HOME/packages/scaly/0.1.0/scaly.scaly" >/dev/null 2>&1 \\
        && ar rcs "\$SCALY_HOME/lib/libscaly.a" "\$SCALY_HOME/lib/libscaly.o"
fi
exec "\$SCALY_HOME/libexec/scalyc" "\$@"
EOF
chmod 755 "$PREFIX/bin/scalyc"
say "installed scalyc -> $PREFIX/bin/scalyc"

# ---------------------------------------------------------------------------
# 6. Put $PREFIX/bin on PATH via the shell profile (unless already there).
# ---------------------------------------------------------------------------
BINDIR="$PREFIX/bin"
case ":$PATH:" in
  *":$BINDIR:"*) say "$BINDIR already on PATH" ;;
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
        say "$PROFILE already references $BINDIR"
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
    scalyc -o hello hello.scaly && ./hello
  Uninstall:  rm -rf "$PREFIX"  (and remove the PATH line above)
EOF
