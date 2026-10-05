#!/bin/bash
# tests/win32/static-llvm.sh — an EXPERIMENT for the Windows box (Git Bash),
# x64 or arm64: can the three programs be linked with LLVM's STATIC libraries,
# so that what we hand out loads no LLVM-C.dll? macOS and Linux do it since
# 2026-10-03 (tools/make-bindist.sh); this is the same question for Windows,
# written on a Mac; first run 2026-10-03 on the arm64 VM, all five steps OK
# after two changes. x64 ran cross-linked on
# that VM: SCALY_WIN_TRIPLE=x86_64-pc-windows-msvc, the x64 archive as [dir],
# and a `clang` in front of PATH that adds --target=x86_64-pc-windows-msvc
# (step 4 then compares with an arm64 compiler and reads DIFFERENT).
#
#   tests/win32/static-llvm.sh [dir]
#
# [dir] is where the release archive is unpacked (default
# %LOCALAPPDATA%\Programs\llvm-21.1.8-static; $SCALY_STATIC_LLVM_DIR wins).
# The archive is `clang+llvm-21.1.8-<arch>-pc-windows-msvc.tar.xz` of the LLVM
# release, about 1 GB to download; only its lib/LLVM*.lib and llvm-config are
# unpacked. A directory that already holds lib/LLVMCore.lib is used as it is.
#
# What it does, each step with its own verdict line, none of them fatal for
# the next (the point is to learn where it stops):
#   1 libs     the archive is there; how many LLVM*.lib, and whether they hold
#              machine code or LTO bitcode (the macOS release's were bitcode)
#   2 build    tools/build-from-seed.sh into a scratch directory with
#              SCALY_STATIC_LLVM_DIR set (tools/win-link.sh takes the static
#              libraries then and copies no DLL)
#   3 alone    the programs start from a directory with NO LLVM-C.dll and with
#              LLVM's bin off the PATH; what DLLs scalyc.exe imports
#   4 same     the static compiler emits the stdlib as the tree's dynamic one
#              does (scalyc/build/scalyc.exe, when it is there)
#   5 suites   tests/tool and tests/selfhosted against the static pair
# Everything is written under a scratch directory and the full logs stay
# there; the last lines say where. Paste the output from `== static-llvm` on.
# ★Side effect: like every build-from-seed run it rewrites /tmp/libscaly.lib
# (the same archive a normal build makes).
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
cd "$ROOT"
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) ;;
  *) echo "static-llvm: this is for the Windows box (Git Bash); $(uname -s) links LLVM statically through tools/make-bindist.sh"; exit 1 ;;
esac
. tools/win-env.sh || exit 1
source tools/llvm-env.sh > /dev/null

VERSION=21.1.8
case "$SCALY_WIN_TRIPLE" in
  aarch64*) ARCH=aarch64 ;;
  *)        ARCH=x86_64 ;;
esac
NAME="clang+llvm-$VERSION-$ARCH-pc-windows-msvc"
DIR="${SCALY_STATIC_LLVM_DIR:-${1:-$(cygpath -u "${LOCALAPPDATA:-$HOME/AppData/Local}")/Programs/llvm-$VERSION-static}}"
DIR="$(cygpath -u "$DIR")"
W="$(mktemp -d)"
OUT="$W/bin"
mkdir -p "$OUT"

echo "== static-llvm: $SCALY_WIN_TRIPLE, clang $(clang --version | head -1)"
echo "   libraries: $DIR"
echo "   scratch:   $W"

# ---------------------------------------------------------------- 1 libs
if [ ! -f "$DIR/lib/LLVMCore.lib" ]; then
  mkdir -p "$DIR"
  URL="https://github.com/llvm/llvm-project/releases/download/llvmorg-$VERSION/$NAME.tar.xz"
  echo "1 libs: downloading $URL (about 1 GB)"
  if curl -fL --retry 3 -o "$W/llvm.tar.xz" "$URL"; then
    # only the static libraries and llvm-config; --strip-components drops the
    # archive's top directory
    tar -xJf "$W/llvm.tar.xz" -C "$DIR" --strip-components=1 --wildcards \
        "$NAME/lib/LLVM*.lib" "$NAME/bin/llvm-config.exe" > "$W/untar.log" 2>&1 \
      || { echo "1 libs: tar said:"; tail -5 "$W/untar.log"; }
    # a tar without --wildcards (Windows' own bsdtar): the whole lib directory
    if [ ! -f "$DIR/lib/LLVMCore.lib" ]; then
      echo "1 libs: retrying without --wildcards (the whole lib directory)"
      tar -xJf "$W/llvm.tar.xz" -C "$DIR" --strip-components=1 "$NAME/lib" "$NAME/bin/llvm-config.exe" > "$W/untar2.log" 2>&1 \
        || { echo "1 libs: tar said:"; tail -5 "$W/untar2.log"; }
    fi
    rm -f "$W/llvm.tar.xz"
  else
    echo "1 libs: FAIL - the download did not work"
  fi
fi
if [ -f "$DIR/lib/LLVMCore.lib" ]; then
  n=$(ls "$DIR"/lib/LLVM*.lib | wc -l | tr -d ' ')
  size=$(du -sh "$DIR/lib" | cut -f1)
  # the first member of LLVMCore.lib: `BC` 0xC0DE is bitcode, anything else an object
  kind="unknown (no llvm-ar)"
  if command -v llvm-ar >/dev/null 2>&1; then
    member=$(llvm-ar t "$DIR/lib/LLVMCore.lib" 2>/dev/null | head -1)
    magic=$(llvm-ar p "$DIR/lib/LLVMCore.lib" "$member" 2>/dev/null | head -c 4 | od -An -tx1 | tr -d ' \n')
    case "$magic" in
      4243c0de|dec0170b) kind="LTO BITCODE (magic $magic): the link will run LLVM's codegen over them" ;;
      *)                 kind="machine code (first member $member, magic $magic)" ;;
    esac
  fi
  echo "1 libs: OK - $n LLVM*.lib, $size; $kind"
  [ -x "$DIR/bin/llvm-config.exe" ] && echo "   llvm-config --system-libs: $("$DIR/bin/llvm-config.exe" --link-static --system-libs 2>&1 | tr '\n' ' ')"
else
  echo "1 libs: FAIL - no $DIR/lib/LLVMCore.lib; nothing below can run"
  exit 1
fi

# ---------------------------------------------------------------- 2 build
start=$(date +%s)
SCALY_STATIC_LLVM_DIR="$DIR" tools/build-from-seed.sh "$OUT/scalyc.exe" > "$W/build.log" 2>&1
rc=$?
took=$(( $(date +%s) - start ))
if [ "$rc" = 0 ] && [ -f "$OUT/scalyc.exe" ]; then
  echo "2 build: OK in ${took}s"
  for p in scalyc scaly scalyls; do
    [ -f "$OUT/$p.exe" ] && echo "   $p.exe $(wc -c < "$OUT/$p.exe" | tr -d ' ') bytes" || echo "   $p.exe MISSING"
  done
  [ -f "$OUT/LLVM-C.dll" ] && echo "   ★ LLVM-C.dll was copied beside them - the static branch did not run"
else
  echo "2 build: FAIL rc=$rc after ${took}s - the last lines of $W/build.log:"
  # the linker's complaints are the finding: undefined symbols name the
  # library still missing, duplicate ones a C runtime linked twice
  grep -m 25 -i 'error\|undefined\|duplicate\|LNK\|lld-link' "$W/build.log"
  echo "   ..."
  tail -8 "$W/build.log"
  echo "== static-llvm: stopped at the build; full log $W/build.log"
  exit 1
fi

# ---------------------------------------------------------------- 3 alone
printf 'print("Hello, World!")\n' > "$W/hello.scaly"
# a PATH without any LLVM: only Git Bash's own tools and Windows
BARE_PATH="/usr/bin:/bin:$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32"
( cd "$W" && PATH="$BARE_PATH" SCALY_HOME="$ROOT" "$OUT/scalyc.exe" -S -o "$W/hello.ll" "$W/hello.scaly" ) > "$W/alone.log" 2>&1
rc=$?
if [ "$rc" = 0 ] && [ -s "$W/hello.ll" ]; then
  echo "3 alone: OK - scalyc.exe starts and emits with no LLVM on PATH and no DLL beside it"
else
  echo "3 alone: FAIL rc=$rc - $(tail -2 "$W/alone.log" | tr '\n' ' ')"
fi
if command -v llvm-objdump >/dev/null 2>&1; then
  echo "   scalyc.exe imports: $(llvm-objdump -p "$OUT/scalyc.exe" 2>/dev/null | sed -n 's/^ *DLL Name: //p' | sort -u | tr '\n' ' ')"
elif command -v llvm-readobj >/dev/null 2>&1; then
  echo "   scalyc.exe imports: $(llvm-readobj --coff-imports "$OUT/scalyc.exe" 2>/dev/null | sed -n 's/^ *Name: //p' | sort -u | tr '\n' ' ')"
else
  echo "   (no llvm-objdump or llvm-readobj here to list the imported DLLs)"
fi
out=$( cd "$W" && PATH="$BARE_PATH" SCALY_HOME="$ROOT" SCALY_CACHE="$W/cache" "$OUT/scaly.exe" run "$W/hello.scaly" 2> "$W/run.log" )
[ "$out" = "Hello, World!" ] && echo "   scaly run (the JIT): OK" || echo "   scaly run (the JIT): FAIL - got '$out' $(tail -2 "$W/run.log" | tr '\n' ' ')"

# ---------------------------------------------------------------- 4 same
if [ -x scalyc/build/scalyc.exe ]; then
  "$OUT/scalyc.exe" -S --no-prelude --no-tests -o "$W/static.ll" packages/scaly/0.1.0/scaly.scaly > "$W/emit1.log" 2>&1
  scalyc/build/scalyc.exe -S --no-prelude --no-tests -o "$W/dynamic.ll" packages/scaly/0.1.0/scaly.scaly > "$W/emit2.log" 2>&1
  if [ -s "$W/static.ll" ] && cmp -s "$W/static.ll" "$W/dynamic.ll"; then
    echo "4 same: OK - the stdlib's IR is byte-identical to scalyc/build/scalyc.exe's"
  else
    echo "4 same: DIFFERENT (or one did not emit) - an older scalyc/build than the seed explains it too; logs $W/emit1.log $W/emit2.log"
  fi
else
  echo "4 same: SKIP - no scalyc/build/scalyc.exe to compare with"
fi

# ---------------------------------------------------------------- 5 suites
tests/tool/run.sh "$OUT/scalyc.exe" > "$W/tool.log" 2>&1
echo "5 suites: tool rc=$? - $(grep '^tool:' "$W/tool.log" | tail -1)"
grep -A 12 '^tool:' "$W/tool.log" | sed -n '2,13p' | cut -c1-200
tests/selfhosted/run.sh "$OUT/scalyc.exe" > "$W/self.log" 2>&1
echo "   selfhosted rc=$? - $(grep '^selfhosted:' "$W/self.log" | tail -1)"

echo "== static-llvm: done; programs in $OUT, logs in $W"
