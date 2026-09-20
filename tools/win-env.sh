#!/bin/bash
# Sourceable: what a Visual Studio developer prompt would provide, DERIVED on
# the Windows box instead of assumed — so the bar's scripts can run from a plain
# Git Bash. Three things, each only where it is MISSING; whatever is already
# there stays (a developer prompt's entries, or the stray ones other installs
# leave behind — a DB2 install leaves `LIB=;C:\PROGRA~1\IBM\SQLLIB\LIB`):
#
#   PATH     clang / llvm-ar from the standalone LLVM install, when `clang` does
#            not resolve. The compiler's link driver calls `clang` by NAME
#            (cli.scaly, link_compiler) through system(), so it must be on PATH;
#            SCALY_CC with a path containing a space would not survive cmd.exe.
#   LIB      the MSVC and Windows-SDK library directories, semicolon-separated,
#            Windows spelling. link.exe reads LIB itself; without it the first
#            symptom is `LNK1181: cannot open input file 'ws2_32.lib'`.
#   INCLUDE  the matching header directories, which clang's MSVC driver honours;
#            without them a shim's `#include <winsock2.h>` is `file not found`.
#
# ★tests/win32/corpus.sh warns against ASSIGNING to LIB, and it is right: on a
# runner that has a developer environment, replacing LIB with one archive
# discards every SDK search path. This file therefore never replaces LIB or
# INCLUDE: it APPENDS the SDK directories when no `Windows Kits` entry is there
# yet. ★"Only when empty" was the first version and set NOTHING on the dev box,
# because a database install had left a LIB with one unrelated directory in it.
#
# Discovery: vswhere for Visual Studio (any product), the newest MSVC toolset
# under it, the newest Windows 10/11 SDK under Program Files (x86). Usage:
#   . tools/win-env.sh || exit 1
# Silent on POSIX (returns 0 without touching anything).

case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*) ;;
  *) return 0 2>/dev/null || exit 0 ;;
esac

if ! command -v clang >/dev/null 2>&1; then
  for d in "/c/Program Files/LLVM/bin"; do
    if [ -x "$d/clang.exe" ]; then PATH="$d:$PATH"; break; fi
  done
  export PATH
fi
if ! command -v clang >/dev/null 2>&1; then
  echo "win-env: no clang on PATH and none under /c/Program Files/LLVM/bin" >&2
  return 1 2>/dev/null || exit 1
fi

case ";${LIB:-};${INCLUDE:-};" in
  *"Windows Kits"*) ;;
  *)
    vswhere="/c/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"
    vs=""
    if [ -x "$vswhere" ]; then
      vs=$("$vswhere" -latest -products '*' -property installationPath 2>/dev/null | tr -d '\r')
    fi
    msvc=""
    if [ -n "$vs" ]; then
      msvc=$(ls -d "$(cygpath -u "$vs")"/VC/Tools/MSVC/*/ 2>/dev/null | sort -V | tail -1)
    fi
    sdklib=$(ls -d "/c/Program Files (x86)/Windows Kits/10/Lib/"10.*/ 2>/dev/null | sort -V | tail -1)
    if [ -z "$msvc" ] || [ -z "$sdklib" ]; then
      echo "win-env: Visual Studio (vswhere) or the Windows SDK not found; set LIB and INCLUDE yourself" >&2
      return 1 2>/dev/null || exit 1
    fi
    sdkver=$(basename "$sdklib")
    sdkinc="/c/Program Files (x86)/Windows Kits/10/Include/$sdkver"
    LIB="${LIB:+$LIB;}$(cygpath -w "${msvc}lib/x64");$(cygpath -w "${sdklib}um/x64");$(cygpath -w "${sdklib}ucrt/x64")"
    INCLUDE="${INCLUDE:+$INCLUDE;}$(cygpath -w "${msvc}include");$(cygpath -w "$sdkinc/ucrt");$(cygpath -w "$sdkinc/um");$(cygpath -w "$sdkinc/shared")"
    export LIB INCLUDE
    ;;
esac
