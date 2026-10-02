#!/bin/bash
# Link an executable on the Windows box the way CI's `windows` job links one
# (.github/workflows/seed.yml, rungs 9 and 12) — the ONE place the link line
# lives locally, so the bar's scripts never spell it a second time.
#
#   tools/win-link.sh [--llvm] [--runtime] [--lto] <out.exe> <inputs...>
#
#   --runtime  append the runtime archive, $TMP/libscaly.lib (Git Bash mounts
#              $TMP as /tmp; tools/win-archive.sh builds it, cli.scaly's link
#              driver looks for it there)
#   --llvm     -L<llvm lib> -lLLVM-C, and LLVM-C.dll copied BESIDE the output:
#              the compiler and the language server CALL the library, and a
#              DLL beside the .exe is found before anything on PATH, so which
#              LLVM answers is not decided by the shell that happened to start
#              it (the winget LLVM 23 is the trap, CLAUDE-tooling.md)
#   --lto      the inputs are bitcode from `clang -flto=full -c`: adds
#              -O2 -fuse-ld=lld -flto=full (link.exe cannot read bitcode; -O2
#              on the LINK is what sets the LTO pipeline's level) and
#              -errorlimit:0 (lld-link stops at 20 errors, and that truncation
#              reads like a short clean answer — rung 12's own lesson)
#
# Always: -lws2_32 (the one-object runtime archive drags in the IOCP backend,
# same argument as -lm on Linux, opposite library; no -lm here — the MSVC CRT
# carries the math), and the 64 MB stack reserve `-Xlinker -stack:` — the
# COFF spelling of `ulimit -s 65520`, because Windows gives a thread 1 MB and
# the planner is bound in nesting depth. ★`-stack:`, never `/STACK:`: under
# Git Bash an argument beginning with `/` is rewritten into a path on its way
# to a native program (tests/win32/WINDOWS-BOX.md).
set -eu
cd "$(dirname "$0")/.."
. tools/win-env.sh || exit 1
source tools/llvm-env.sh > /dev/null
T=${SCALY_WIN_TRIPLE:-x86_64-pc-windows-msvc}
LLVM=0; RT=0; LTO=0
while [ $# -gt 0 ]; do
  case "$1" in
    --llvm) LLVM=1 ;;
    --runtime) RT=1 ;;
    --lto) LTO=1 ;;
    --) shift; break ;;
    -*) echo "win-link: unknown flag $1" >&2; exit 2 ;;
    *) break ;;
  esac
  shift
done
OUT=${1:?usage: tools/win-link.sh [--llvm] [--runtime] [--lto] <out.exe> <inputs...>}
shift
[ $# -gt 0 ] || { echo "win-link: nothing to link" >&2; exit 2; }

args=(clang --target=$T)
[ "$LTO" = 1 ] && args+=(-O2 -fuse-ld=lld -flto=full)
args+=("$@")
if [ "$RT" = 1 ]; then
  rt="$(cygpath -u "${TMP:-${TEMP:-/tmp}}")/libscaly.lib"
  [ -f "$rt" ] || { echo "win-link: no $rt — build it: tools/win-archive.sh <compiler>" >&2; exit 1; }
  args+=("$rt")
fi
args+=(-lws2_32)
[ "$LLVM" = 1 ] && args+=(-L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME")
args+=(-Xlinker -stack:67108864,1048576)
[ "$LTO" = 1 ] && args+=(-Xlinker -errorlimit:0)
args+=(-o "$OUT")
mkdir -p "$(dirname "$OUT")"
"${args[@]}"

if [ "$LLVM" = 1 ]; then
  dll="$LLVM_PREFIX/bin/LLVM-C.dll"
  [ -f "$dll" ] || { echo "win-link: no $dll beside the LLVM that linked $OUT" >&2; exit 1; }
  dest="$(dirname "$OUT")/LLVM-C.dll"
  cmp -s "$dll" "$dest" 2>/dev/null || cp "$dll" "$dest"
fi
