#!/bin/bash
# Link an executable on the Windows box the way CI's `windows` job links one
# (.github/workflows/seed.yml, rungs 9 and 12) — the ONE place the link line
# lives locally, so the bar's scripts never spell it a second time.
#
#   tools/win-link.sh [--llvm] [--runtime] [--lto] [--export F]... <out.exe> <inputs...>
#
#   --runtime  append the runtime archive, $TMP/libscaly.lib (Git Bash mounts
#              $TMP as /tmp; tools/win-archive.sh builds it, cli.scaly's link
#              driver looks for it there)
#   --llvm     (with $SCALY_STATIC_LLVM_DIR set, see below, LLVM's static
#              libraries instead and no DLL) otherwise
#              -L<llvm lib> -lLLVM-C, and LLVM-C.dll copied BESIDE the output:
#              the compiler and the language server CALL the library, and a
#              DLL beside the .exe is found before anything on PATH, so which
#              LLVM answers is not decided by the shell that happened to start
#              it (the winget LLVM 23 is the trap)
#   --lto      the inputs are bitcode from `clang -flto=full -c`: adds
#              -O2 -fuse-ld=lld -flto=full (link.exe cannot read bitcode; -O2
#              on the LINK is what sets the LTO pipeline's level) and
#              -errorlimit:0 (lld-link stops at 20 errors, and that truncation
#              reads like a short clean answer — rung 12's own lesson)
#   --export F export the functions input F defines (repeatable; an object,
#              an archive or bitcode): what a JIT host's GetProcAddress needs.
#              A --llvm --runtime link exports the runtime archive by itself.
#
# Always: -lws2_32 (the one-object runtime archive drags in the IOCP backend,
# same argument as -lm on Linux, opposite library; no -lm here — the MSVC CRT
# carries the math), and the 64 MB stack reserve `-Xlinker -stack:` — the
# COFF spelling of `ulimit -s 65520`, because Windows gives a thread 1 MB and
# the planner is bound in nesting depth. ★`-stack:`, never `/STACK:`: under
# Git Bash an argument beginning with `/` is rewritten into a path on its way
# to a native program.
set -eu
cd "$(dirname "$0")/.."
. tools/win-env.sh || exit 1
source tools/llvm-env.sh > /dev/null
T=${SCALY_WIN_TRIPLE:-x86_64-pc-windows-msvc}
LLVM=0; RT=0; LTO=0; EXPORTS=()
while [ $# -gt 0 ]; do
  case "$1" in
    --llvm) LLVM=1 ;;
    --runtime) RT=1 ;;
    --lto) LTO=1 ;;
    --export) shift; EXPORTS+=("$1") ;;
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
# The code page resource: every program linked here reads and writes file
# names, arguments and the environment as UTF-8, which is what a Scaly string
# is (scaly/win32/utf8.manifest; the driver adds it too, cli.scaly
# utf8_resource#). A .res is an input the linker takes as it is -- once:
# where the driver's own line comes through here (the archive build's
# compiler wrapper) the resource is among the inputs already, and link.exe
# refuses a second one (CVT1100; lld-link merges them).
case " $* " in
  *utf8.res\ *) ;;
  *) args+=(packages/scaly/0.1.1/scaly/win32/utf8.res) ;;
esac
if [ "$RT" = 1 ]; then
  rt="$(cygpath -u "${TMP:-${TEMP:-/tmp}}")/libscaly.lib"
  [ -f "$rt" ] || { echo "win-link: no $rt — build it: tools/win-archive.sh <compiler>" >&2; exit 1; }
  args+=("$rt")
fi
args+=(-lws2_32)
# ★AN EXPERIMENT, not yet a route (tests/win32/static-llvm.sh, 2026-10-03):
# with $SCALY_STATIC_LLVM_DIR naming an unpacked `clang+llvm-…-windows-msvc`
# release, a --llvm link takes LLVM's STATIC libraries from its lib/ instead
# of the import library of LLVM-C.dll, and copies no DLL -- the programs we
# hand out are to load nothing of LLVM (macOS and Linux do it since the same
# day, tools/build-from-seed.sh SCALY_STATIC_LLVM). Every LLVM*.lib but
# LLVM-C.lib, which IS the DLL's import library; lld-link takes from an
# archive only what is referenced. The system libraries are the ones LLVM's
# Support asks for on Windows (`llvm-config --link-static --system-libs` adds
# ws2_32, which is on the line anyway, and xml2s.lib, which the archive does
# not ship and nothing here references). Unset, nothing here changes.
# ★`-lucrt` BEFORE LLVM's libraries is load-bearing: the release is built with
# LLVM_INTEGRATED_CRT_ALLOC, so LLVMSupport.lib carries rpmalloc and with it
# `malloc`, `free`, `_malloc_base` & co. -- meant to replace the heap of a
# STATIC C library. The first archive on the line that defines `free` wins;
# without this line that was LLVMSupport, and the program freed with rpmalloc
# what ucrtbase.dll had allocated (strdup): an access violation in `free`
# under Emitter.emit, on hello world. With the UCRT's import library first the
# rpmalloc member is never taken and there is ONE heap, ucrtbase's -- the same
# one the JIT's code resolves (the comment on the hybrid CRT below).
# ★The runtime's JIT arena (scaly_jit_use_arena, eio_windows.c) reaches three
# functions of the C API through GetProcAddress -- on LLVM-C.dll, and with no
# DLL on the program itself. So a static link EXPORTS them (STATIC_EXPORTS
# below goes into the .def); an export is also the root that pulls them out
# of LLVMOrcJIT.lib. Without it the arena is silently off and JIT sections
# land out of each other's reach: `scaly test` on opensp died in 9 of 24 runs.
STATIC_LLVM=0
STATIC_EXPORTS=()
if [ "$LLVM" = 1 ] && [ -n "${SCALY_STATIC_LLVM_DIR:-}" ]; then
  STATIC_LLVM=1
  STATIC_EXPORTS=(LLVMOrcLLJITBuilderSetObjectLinkingLayerCreator
                  LLVMOrcCreateRTDyldObjectLinkingLayerWithMCJITMemoryManagerLikeCallbacks
                  LLVMOrcCreateRTDyldObjectLinkingLayerWithSectionMemoryManager)
  sdir="$(cygpath -u "$SCALY_STATIC_LLVM_DIR")/lib"
  [ -f "$sdir/LLVMCore.lib" ] || { echo "win-link: no LLVMCore.lib in $sdir (\$SCALY_STATIC_LLVM_DIR)" >&2; exit 1; }
  args+=(-lucrt)
  for lib in "$sdir"/LLVM*.lib; do
    [ "$(basename "$lib")" = "LLVM-C.lib" ] && continue
    args+=("$lib")
  done
  args+=(-lpsapi -lshell32 -lole32 -luuid -ladvapi32 -lntdll)
elif [ "$LLVM" = 1 ]; then
  args+=(-L"$LLVM_LIBDIR" -l"$LLVM_LIBNAME")
fi
# ★A JIT host takes the UCRT from ucrtbase.dll (the "hybrid" CRT: startup and
# vcruntime static as everywhere, the C library dynamic). The JIT resolves
# fopen, malloc & co. in that DLL, and the runtime's shims in this .exe must
# use the SAME instance: with the static UCRT here, scaly_eio_seek locked a
# FILE* the JIT had opened in ucrtbase -- two CRTs, one stream, an access
# violation inside the lock (2026-10-03). ucrtbase is part of Windows 10+.
[ "$LLVM" = 1 ] && args+=(-Xlinker -nodefaultlib:libucrt.lib -Xlinker -defaultlib:ucrt.lib)
# The JIT host: a program that runs the in-process JIT resolves the runtime
# through GetProcAddress on ITSELF, and an .exe exports nothing -- every stdlib
# function was then a stub answering 0 (Emitter.emit_jit_stubs) and every C
# shim an unresolved symbol (2026-10-03). So a
# link that names runtime inputs with --export (and every --llvm --runtime
# link: the archive) exports the FUNCTIONS they define, through a .def written
# here. Only the runtime: exporting the whole compiler would keep LTO from
# dropping and internalising it. No import library and no .exp beside the .exe.
[ "$LLVM" = 1 ] && [ "$RT" = 1 ] && EXPORTS+=("$rt")
if [ ${#EXPORTS[@]} -gt 0 ]; then
  def="$(dirname "$OUT")/$(basename "$OUT" .exe).exports.def"
  mkdir -p "$(dirname "$OUT")"
  # ★Beside the runtime's own functions -- among them the POSIX names the UCRT
  # DLL exports only with an underscore, which posixcompat_windows.c defines --
  # `atexit`, which lives in the static startup code only. Checked 2026-10-03
  # against every extern of packages/ with GetProcAddress over the system DLLs.
  { echo EXPORTS
    llvm-nm --defined-only --extern-only "${EXPORTS[@]}" 2>/dev/null \
      | awk 'NF==3 && ($2=="T" || $2=="W") && $3 !~ /^[?.]/ && $3 !~ /^__/ {print "    " $3}' | sort -u
    echo "    atexit"
    for e in ${STATIC_EXPORTS[@]+"${STATIC_EXPORTS[@]}"}; do echo "    $e"; done
  } > "$def"
  args+=(-Xlinker "-def:$(cygpath -w "$def")" -Xlinker -noimplib)
fi
args+=(-Xlinker -stack:67108864,1048576)
[ "$LTO" = 1 ] && args+=(-Xlinker -errorlimit:0)
args+=(-o "$OUT")
mkdir -p "$(dirname "$OUT")"
"${args[@]}"

if [ "$LLVM" = 1 ] && [ "$STATIC_LLVM" = 0 ]; then
  dll="$LLVM_PREFIX/bin/LLVM-C.dll"
  [ -f "$dll" ] || { echo "win-link: no $dll beside the LLVM that linked $OUT" >&2; exit 1; }
  dest="$(dirname "$OUT")/LLVM-C.dll"
  cmp -s "$dll" "$dest" 2>/dev/null || cp "$dll" "$dest"
fi
