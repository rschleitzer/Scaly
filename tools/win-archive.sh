#!/bin/bash
# Build the runtime archive on the Windows box: libscaly.lib where the compiler's
# link driver looks for it (cli.scaly runtime_archive_path: $TMP/libscaly.lib,
# which Git Bash mounts as /tmp). The POSIX equivalent lives inside
# tools/build-from-seed.sh / bootstrap.sh; those go through tools/llvm-env.sh
# and do not start here (CLAUDE-tooling.md), hence this file.
#
# The recipe is the one CI's `windows` job runs at rung 3 (.github/workflows/
# seed.yml), object for object: the stdlib emitted with --no-prelude --no-tests,
# linkonce_odr promoted to weak_odr BEFORE optimizing (a library has no main
# anchor, GlobalDCE would delete every body), compiled -O2; the four C shims
# with CI's strict flags; the Win64 fcontext switcher; llvm-ar.
#
# Usage: tools/win-archive.sh [compiler] [outdir]
#        defaults: scalyc/build/scalyc.exe, $TMP
set -e
cd "$(dirname "$0")/.." || exit 1
. tools/win-env.sh || exit 1
SCALYC=${1:-scalyc/build/scalyc.exe}
OUT=${2:-$(cygpath -u "${TMP:-${TEMP:-/tmp}}")}
T=x86_64-pc-windows-msvc
P=packages/scaly/0.1.0/scaly
if [ ! -x "$SCALYC" ]; then echo "win-archive: no compiler at $SCALYC" >&2; exit 1; fi

"$SCALYC" -S --no-prelude --no-tests -o "$OUT/libscaly_win.ll" packages/scaly/0.1.0/scaly.scaly
sed 's/^define linkonce_odr /define weak_odr /' "$OUT/libscaly_win.ll" > "$OUT/libscaly_win_weak.ll"
clang --target=$T -c -O2 -Wno-override-module -o "$OUT/libscaly_win.o" "$OUT/libscaly_win_weak.ll"
clang --target=$T -c "$P/fiber/fcontext_x86_64_windows.S" -o "$OUT/fcontext_win.o"
clang --target=$T -O2 -Wall -Wextra -Werror -c "$P/fiber/eio_windows.c"      -o "$OUT/eio_win.o"
clang --target=$T -O2 -Wall -Wextra -Werror -c "$P/win32/posixcompat_windows.c"  -o "$OUT/pc.o"
clang --target=$T -O2 -Wall -Wextra -Werror -c "$P/time/ctime.c"         -o "$OUT/ctime.o"
clang --target=$T -O2 -Wall -Wextra -Werror -c "$P/memory/panic.c"       -o "$OUT/panic.o"
rm -f "$OUT/libscaly.lib"
llvm-ar rcs "$OUT/libscaly.lib" "$OUT/libscaly_win.o" "$OUT/eio_win.o" "$OUT/ctime.o" "$OUT/panic.o" "$OUT/pc.o" "$OUT/fcontext_win.o"
echo "win-archive: $OUT/libscaly.lib"
llvm-ar t "$OUT/libscaly.lib" | sed 's/^/  /'
