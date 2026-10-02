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
#   PATH     (again) tools/win at the END: `ar`, `llc` and `opt` stand-ins for
#            the three names the bar's scripts call and this toolchain does not
#            ship (llvm-ar, `clang -c`, `clang -emit-llvm`; each file says what
#            it accepts). At the end, so a real one anywhere on PATH still wins.
#   PATH     (again) a real python3 when the one that resolves is the Microsoft
#            Store stub, which prints an advertisement and exits 49: winget's
#            Python.Python.3.13 lands under %LOCALAPPDATA%\Programs\Python and
#            is not on PATH at all (CLAUDE-tooling.md).
#   SCALY_COFF=1, SCALY_EXE=.exe, SCALY_WIN_TOOLS — the facts the scripts ask
#            (tests/platform.sh sets the POSIX values; tools/llvm-env.sh reads
#            these on the Windows box and points LLC/OPT at the stand-ins).
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

# ★Through cygpath, never a bare `pwd`: bash keeps the SPELLING of the directory
# it was started in, and a shell launched by a Windows program (an IDE, an
# agent, a CI step with `working-directory`) starts in `C:/repos/...`. As a PATH
# entry that is two entries, `C` and `/repos/...`, and the first symptom is
# `ar: command not found` a hundred lines into a bootstrap (met 2026-10-01).
SCALY_WIN_TOOLS="$(cygpath -u "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")/win"
# The triple every script here builds for: what the INSTALLED clang targets by
# default -- aarch64-pc-windows-msvc with the arm64 LLVM (LLVM-<v>-woa64.exe),
# x86_64-pc-windows-msvc otherwise. The toolchain decides, not `uname -m`: a
# Git Bash on an arm64 machine may itself be an emulated x64 program.
# (2026-10-02; until then the six scripts each spelled the x64 triple.)
if [ -z "${SCALY_WIN_TRIPLE:-}" ]; then
  case "$(clang -dumpmachine 2>/dev/null)" in
    aarch64*|arm64*) SCALY_WIN_TRIPLE=aarch64-pc-windows-msvc ;;
    *)               SCALY_WIN_TRIPLE=x86_64-pc-windows-msvc ;;
  esac
fi
export SCALY_WIN_TRIPLE
case ":$PATH:" in
  *":$SCALY_WIN_TOOLS:"*) ;;
  *) PATH="$PATH:$SCALY_WIN_TOOLS" ;;
esac
for t in ar llc opt; do
  if ! command -v "$t" >/dev/null 2>&1; then
    echo "win-env: \`$t\` does not resolve although $SCALY_WIN_TOOLS is on PATH" >&2
    return 1 2>/dev/null || exit 1
  fi
done
# The tools the installer does not ship (llvm-dwarfdump for tests/debuginfo),
# UNPACKED from the release tarball into %LOCALAPPDATA%\Programs\llvm-21.1.8
# — never a second installer run (tests/win32/WINDOWS-BOX.md §1) — and at the
# END of PATH: the installed clang stays the one that answers, and the LLC/OPT
# stand-ins are named explicitly by tools/llvm-env.sh, so the real llc/opt in
# there change nothing the bar measures.
xtools="$(cygpath -u "${LOCALAPPDATA:-$HOME/AppData/Local}")/Programs/llvm-21.1.8/bin"
if [ -d "$xtools" ]; then
  case ":$PATH:" in *":$xtools:"*) ;; *) PATH="$PATH:$xtools" ;; esac
fi
if ! python3 -c 'import sys' >/dev/null 2>&1; then
  py=$(ls -d "$(cygpath -u "${LOCALAPPDATA:-$HOME/AppData/Local}")"/Programs/Python/Python3*/ 2>/dev/null | sort -V | tail -1)
  if [ -n "$py" ] && [ -x "$py/python3.exe" ]; then PATH="${py%/}:$PATH"; fi
fi
export PATH
# Python on Windows reads and writes in the ANSI code page unless told: the
# tree's tools print `ü` and read UTF-8 sources, and tests/abi's grep for its
# own verdict line missed on a cp1252 byte (measured 2026-09-20). One switch,
# the one the interpreter documents for exactly this.
export PYTHONUTF8=1
SCALY_COFF=1
SCALY_EXE=.exe
export SCALY_COFF SCALY_EXE SCALY_WIN_TOOLS
