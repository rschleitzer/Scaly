#!/bin/bash
# tools/make-bindist.sh for the Windows box (Git Bash), x64 or arm64: the three
# programs as docs/website/install.ps1 hands them out -- LLVM linked in
# statically, nothing to load but what Windows 10 and newer bring -- and what
# `scaly build` needs so that the Visual Studio Build Tools are its ONLY
# prerequisite (ROADMAP-public.md, "Windows: Rust's way").
#
# Output: <outdir>/scaly-<version>-windows-<arch>.zip, <arch> arm64 or x86_64.
# Payload:
#   libexec/scalyc.exe scaly.exe scalyls.exe
#   lib/libscaly.lib                    the runtime archive `scalyc -o` links
#   packages/scaly/<v>/_native/windows-<arch>/*.o
#                                       the stdlib's C and assembly files
#                                       READY-MADE (tools/native-objects.sh):
#                                       the tool links them and asks no clang
# The packages and the seed travel in tools/make-dist.sh's tarball, as on
# macOS and Linux; the zip is unpacked OVER it (the _native directory lands
# inside the stdlib's).
#
# Needs LLVM's static libraries, the unpacked `clang+llvm-21.1.8-<arch>-pc-
# windows-msvc` release: $SCALY_STATIC_LLVM_DIR, default
# %LOCALAPPDATA%\Programs\llvm-21.1.8-static -- tests/win32/static-llvm.sh
# fetches them there (WINDOWS-BOX.md §10 has what the link owes them).
# No profile build yet: the programs are the seed recipe's.
#
# Usage: tools/make-bindist-windows.sh [version] [outdir]   (default 0.1.0 dist)
set -e
cd "$(dirname "$0")/.."
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) ;;
  *) echo "make-bindist-windows: this is for the Windows box; tools/make-bindist.sh is the POSIX one"; exit 1 ;;
esac
. tools/win-env.sh || exit 1
source tools/llvm-env.sh > /dev/null
VERSION="${1:-0.1.0}"
OUT="${2:-dist}"
case "$SCALY_WIN_TRIPLE" in
  aarch64*) ARCH=arm64 ;;
  *)        ARCH=x86_64 ;;
esac
ZIP="$OUT/scaly-$VERSION-windows-$ARCH.zip"
LIBS="${SCALY_STATIC_LLVM_DIR:-$(cygpath -u "${LOCALAPPDATA:-$HOME/AppData/Local}")/Programs/llvm-21.1.8-static}"
[ -f "$LIBS/lib/LLVMCore.lib" ] || { echo "make-bindist-windows: FAIL — no LLVM static libraries in $LIBS (tests/win32/static-llvm.sh fetches them)"; exit 1; }

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/libexec" "$STAGE/lib"

# 1. from the seed, LLVM linked in
SCALY_STATIC_LLVM_DIR="$LIBS" tools/build-from-seed.sh "$STAGE/libexec/scalyc.exe" > "$STAGE/build.log" 2>&1 \
  || { tail -20 "$STAGE/build.log"; echo "make-bindist-windows: FAIL — build-from-seed"; exit 1; }
for p in scalyc scaly scalyls; do
  [ -f "$STAGE/libexec/$p.exe" ] || { echo "make-bindist-windows: FAIL — $p.exe was not built"; exit 1; }
done
rm -f "$STAGE"/libexec/*.def "$STAGE/build.log"
cp "$(cygpath -u "${TMP:-${TEMP:-/tmp}}")/libscaly.lib" "$STAGE/lib/libscaly.lib"

# 2. the stdlib's native files, ready-made
mkdir -p "$STAGE/home/packages"
cp -R packages/scaly "$STAGE/home/packages/"
tools/native-objects.sh "$STAGE/libexec/scalyc.exe" "$STAGE/home" \
  || { echo "make-bindist-windows: FAIL — native-objects"; exit 1; }
( cd "$STAGE/home" && find packages -type d -name _native ) | while read -r d; do
  mkdir -p "$STAGE/$(dirname "$d")"
  cp -R "$STAGE/home/$d" "$STAGE/$d"
done
rm -rf "$STAGE/home"

# Nothing of LLVM may be left to load: only DLLs that are Windows' own.
deps=$(for p in scalyc scaly scalyls; do llvm-objdump -p "$STAGE/libexec/$p.exe" | sed -n 's/^ *DLL Name: //p'; done \
       | sort -u | grep -viE '^(KERNEL32|ADVAPI32|SHELL32|ole32|OLEAUT32|ntdll|WS2_32|api-ms-win-crt-[a-z0-9-]*)\.dll$' || true)
[ -z "$deps" ] || { echo "make-bindist-windows: FAIL — the programs still load:"; echo "$deps"; exit 1; }

mkdir -p "$OUT"
rm -f "$ZIP"
# Windows' own tar (bsdtar) writes a zip by the name's suffix; Git Bash's does not
ZIP_W="$(cygpath -w "$(cd "$OUT" && pwd)/$(basename "$ZIP")")"
( cd "$STAGE" && "$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32/tar.exe" -a -c -f "$ZIP_W" libexec lib packages ) \
  || { echo "make-bindist-windows: FAIL — the zip"; exit 1; }
echo "make-bindist-windows: OK -> $ZIP ($(du -h "$ZIP" | cut -f1)), without a profile"
