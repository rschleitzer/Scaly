#!/bin/bash
# tests/platform.sh — sourced by the suite runners; the ONE place that tells the
# Windows box (Git Bash, COFF, MSVC link) from the POSIX hosts. Everything a
# runner does differently there comes from here, so a runner reads the same on
# both and the difference is auditable in one file.
#
#   SCALY_COFF           1 on the Windows box, 0 elsewhere
#   SCALY_EXE            ".exe" there, empty elsewhere — appended to every
#                        binary a runner builds and executes
#   SCALY_STAGE_DEFAULT  the compiler a runner uses when none is passed:
#                        /tmp/scalyc_stage2 on POSIX (tools/bootstrap.sh), the
#                        tree's scalyc/build/scalyc.exe on Windows, where the
#                        bootstrap scripts do not start (CLAUDE-tooling.md)
#   LLVM_LIBDIR/LLVM_LIBNAME  preset on Windows for the `; link: llvm` fixtures
#                        (tools/llvm-env.sh finds no LLVM there); the 8.3
#                        spelling because cli.scaly's link driver hands -L through
#                        system() UNQUOTED, and `Program Files` has a space
#   scaly_need_archive <suite> [compiler]
#                        a suite that LINKS calls this first: on Windows nothing
#                        tops the archive up, so a missing /tmp/libscaly.lib is
#                        refused with the command that builds it
#   scaly_jit_available  false on Windows: the in-process JIT resolves the runtime
#                        through GetProcAddress, and an .exe exports nothing
#                        (tests/win32/WINDOWS-BOX.md §1; measured 2026-09-20,
#                        `--jit` dies with SIGSEGV). A suite that needs it says
#                        SKIP by name instead of failing or passing vacuously.
#
# The POSIX values are byte-for-byte what every runner hard-coded before
# 2026-09-20; sourcing this on a POSIX host changes nothing. tools/win-env.sh
# supplies the developer-prompt environment (clang, LIB, INCLUDE) on Windows.
# Git Bash mounts $TMP as /tmp, so the /tmp paths below are one string on both.

SCALY_COFF=0
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) SCALY_COFF=1 ;; esac
SCALY_EXE=
SCALY_STAGE_DEFAULT=/tmp/scalyc_stage2
if [ "$SCALY_COFF" = 1 ]; then
  # By this file's own location, not the cwd: tests/sgml/coding/run.sh works
  # from `tests/`, not from the repository root.
  . "$(dirname "${BASH_SOURCE[0]}")/../tools/win-env.sh" || { return 1 2>/dev/null || exit 1; }
  SCALY_EXE=.exe
  # The POSIX default with the suffix: tools/bootstrap.sh produces it here too
  # since 2026-09-20 (it was scalyc/build/scalyc.exe while the bootstrap
  # scripts did not start on this box).
  SCALY_STAGE_DEFAULT=/tmp/scalyc_stage2.exe
  if [ -z "${LLVM_LIBDIR:-}" ] && [ -d "/c/Program Files/LLVM/lib" ]; then
    LLVM_LIBDIR=$(cygpath -u "$(cygpath -d '/c/Program Files/LLVM/lib')")
    LLVM_LIBNAME=LLVM-C
  fi
  # ★`TMP` is not a free name here (tests/win32/WINDOWS-BOX.md): Windows
  # EXPORTS it, and a dozen runners write `TMP="$(mktemp -d)"` for their own
  # scratch — an assignment to an exported name stays exported, so every
  # compiler they then start read ITS scratch dir off the runner's, and the
  # link died on `<runner scratch>/libscaly.lib` (measured 2026-09-20, every
  # suite that links). Dropping the export attribute once, here, keeps those
  # assignments shell-local; the compiler and clang fall back to `TEMP`, the
  # same directory, which nothing in the tree assigns.
  export -n TMP
  # ★Windows' installer detection judges an executable by its NAME: one that
  # contains `setup`, `install`, `update` or `patch` and carries no manifest
  # asks for elevation, and a start from bash is then `Permission denied`,
  # rc 126. A test program named after its fixture meets that:
  # rt_generic_operator_dis-patch-.exe (measured 2026-10-02 on the arm64 VM;
  # the same file under another name printed PASS). RunAsInvoker tells the
  # loader to start what it is given with the caller's rights.
  export __COMPAT_LAYER=RunAsInvoker
fi

scaly_need_archive() {
  if [ "$SCALY_COFF" = 1 ] && [ ! -f /tmp/libscaly.lib ]; then
    echo "$1: no /tmp/libscaly.lib — build it: tools/win-archive.sh ${2:-$SCALY_STAGE_DEFAULT}" >&2
    return 1
  fi
  return 0
}

scaly_jit_available() { [ "$SCALY_COFF" = 0 ]; }

# stdout of a program on the Windows box arrived with CRLF until 2026-10-03: the
# CRT's fd 1 starts in TEXT mode and turned every `\n` our runtime writes into
# `\r\n`. A Scaly program's standard streams are BINARY there since (set before
# main by the runtime; tests/win32/WINDOWS-BOX.md §8), so for a program a current
# compiler built this filter removes nothing; it stays for objects an older
# compiler emitted. Every Windows rung of CI strips at the COMPARISON
# (tests/win32/lf-wrapper.sh has the account); this is the same filter,
# in the C locale so that it is a byte filter. Never in a pipeline with the
# program itself — `prog | scaly_lf` reports the FILTER's exit code — always on
# a captured file. Identity on POSIX. ★The single-line comparisons pass without
# it only because the msys bash strips a trailing CR with the trailing newline
# in `$(...)`; a second line keeps its CR, which is how cluster's roundtrip
# ('R1 PASS' / 'R2 PASS') fell while forty-five single-line fiber fixtures did not.
scaly_lf() { if [ "$SCALY_COFF" = 1 ]; then LC_ALL=C tr -d '\r'; else cat; fi; }
