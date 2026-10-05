#!/bin/bash
# tools/make-bindist.sh for the Windows box (Git Bash), x64 or arm64: the three
# programs as docs/website/install.ps1 hands them out -- LLVM linked in
# statically, nothing to load but what Windows 10 and newer bring -- and what
# `scaly build` needs so that the Visual Studio Build Tools are its ONLY
# prerequisite.
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
# fetches them there.
#
# Two steps, as tools/make-bindist.sh:
#   1. tools/build-from-seed.sh with $SCALY_STATIC_LLVM_DIR set: all three
#      programs and the runtime archive.
#   2. With a profile, ALWAYS unless told otherwise: the one
#      $SCALY_PGO_PROFILE names, else one made here and now by
#      tools/make-profile.sh with the tool of step 1 (45 s; a profile belongs
#      to the sources it was made from, so a fresh one is the right one --
#      dist/scalyc.profdata is NOT taken by itself, it may be old).
#      SCALY_PGO_PROFILE=none builds without. scalyc and scaly
#      are built AGAIN from the compiler's sources by the tool of step 1:
#      `scaly build --pgo <profile> --export`. The tool makes the ONE
#      optimised object; the LINK is tools/win-link.sh's static branch, not
#      the tool's own line -- LLVM's libraries, the JIT host's C runtime, the
#      64 MB stack and the runtime's EXPORTS (what the in-process JIT finds
#      the runtime by; the tool's own link exports nothing on Windows).
#      A driver handed over as $SCALY_CC does that.
#      scalyls stays step 1's. Making the profile needs the clang, the
#      profile runtime and llvm-profdata of the LLVM install -- which a box
#      that builds this has.
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
PROFILE="${SCALY_PGO_PROFILE:-}"
if [ -n "$PROFILE" ] && [ "$PROFILE" != none ] && [ ! -f "$PROFILE" ]; then echo "make-bindist-windows: FAIL — no profile $PROFILE"; exit 1; fi
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

# 2. the compiler and the tool again, from the sources, with the profile
MADE=""
if [ -z "$PROFILE" ]; then
  PROFILE="$STAGE/scalyc.profdata"
  SCALYC="$STAGE/libexec/scalyc.exe" tools/make-profile.sh "$PROFILE" > "$STAGE/profile.log" 2>&1 \
    || { tail -10 "$STAGE/profile.log"; echo "make-bindist-windows: FAIL — make-profile (SCALY_PGO_PROFILE=none builds without one)"; exit 1; }
  rm -f "$STAGE/profile.log"
  MADE=" made for this build"
fi
if [ "$PROFILE" != none ]; then
  PGO="$STAGE/pgo"; mkdir -p "$PGO"
  # The tool calls $SCALY_CC through cmd.exe: a .cmd that hands the line to a
  # bash script. `-c` is a package's native file (clang's); anything else is
  # the link, whose inputs go to win-link.sh and whose flags are dropped --
  # win-link.sh writes that line.
  cat > "$PGO/cc.sh" <<EOF
#!/bin/bash
for a in "\$@"; do [ "\$a" = "-c" ] && exec clang "\$@"; done
out=""; inputs=(); exports=()
while [ \$# -gt 0 ]; do
  case "\$1" in
    -o) shift; out="\$(cygpath -u "\$1")" ;;
    -*) ;;
    *) f="\$(cygpath -u "\$1")"; inputs+=("\$f"); exports+=(--export "\$f") ;;
  esac
  shift
done
cd "$PWD" && SCALY_STATIC_LLVM_DIR="$LIBS" exec tools/win-link.sh --llvm "\${exports[@]}" "\$out" "\${inputs[@]}"
EOF
  printf '@"%s" "%s" %%*\r\n' "$(cygpath -w "$(command -v bash)")" "$(cygpath -w "$PGO/cc.sh")" > "$PGO/cc.cmd"
  for pair in main:scalyc scaly_main:scaly; do
    root="${pair%%:*}"; prog="${pair##*:}"
    SCALY_CC="$(cygpath -w "$PGO/cc.cmd")" SCALY_CACHE="$PGO/cache" SCALY_HOME= \
      "$STAGE/libexec/scaly.exe" build "packages/scalyc/0.1.0/$root.scaly" --pgo "$PROFILE" --export \
      -o "$PGO/$prog.exe" > "$PGO/$prog.log" 2>&1 \
      || { tail -20 "$PGO/$prog.log"; echo "make-bindist-windows: FAIL — the profile build of $prog"; exit 1; }
    stale=$(grep -c 'profile data may be out of date\|function control flow change detected' "$PGO/$prog.log" || true)
    [ "$stale" = 0 ] || echo "make-bindist-windows: NOTE — $prog: $stale warnings of a profile that does not fit (tools/make-profile.sh)"
  done
  # they replace step 1's only when they can do what step 1's can: the same
  # emission, and a program run in process (the reason for the exports)
  "$STAGE/libexec/scalyc.exe" -S --no-prelude --no-tests -o "$PGO/a.ll" packages/scaly/0.1.0/scaly.scaly
  "$PGO/scalyc.exe" -S --no-prelude --no-tests -o "$PGO/b.ll" packages/scaly/0.1.0/scaly.scaly
  cmp -s "$PGO/a.ll" "$PGO/b.ll" || { echo "make-bindist-windows: FAIL — the profiled compiler emits differently"; exit 1; }
  out=$(SCALY_CACHE="$PGO/cache" SCALY_HOME= "$PGO/scaly.exe" run tests/tool/hello.scaly one two 2>/dev/null || true)
  [ "$out" = "hello one two" ] || { echo "make-bindist-windows: FAIL — the profiled tool cannot run a program in process: '$out'"; exit 1; }
  mv "$PGO/scalyc.exe" "$STAGE/libexec/scalyc.exe"
  mv "$PGO/scaly.exe" "$STAGE/libexec/scaly.exe"
  rm -rf "$PGO"
  if [ -n "$MADE" ]; then BUILT="with a profile$MADE"; else BUILT="with the profile $PROFILE"; fi
else
  echo "make-bindist-windows: NOTE — SCALY_PGO_PROFILE=none: the programs are the seed recipe's, about 10 % slower on the compiler's own work"
  BUILT="without a profile"
fi
rm -f "$STAGE/scalyc.profdata"

# 3. the stdlib's native files, ready-made
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
echo "make-bindist-windows: OK -> $ZIP ($(du -h "$ZIP" | cut -f1)), $BUILT"
