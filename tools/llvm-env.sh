# Sourceable LLVM-20 environment detector for the seed pipeline.
#   source tools/llvm-env.sh   ->  sets LLVM_PREFIX, LLC, LLVM_LIBDIR, LLVM_LIBNAME
# Override any of them by exporting before sourcing (e.g. LLVM20=/path).
# Mirrors build.sh's prefix logic so the C++ bootstrap and the seed agree.
# Safe under `set -u`: tools/link-lto.sh sources it with nounset on, and every
# variable below may legitimately be unset on entry.

# 0. The Windows box (Git Bash, the standalone LLVM installer). None of the
#    probes below can find anything there — no brew, no /usr/lib/llvm-20, no
#    llvm-config — and until 2026-09-20 the whole seed pipeline therefore did
#    not START here, which looks like nothing at all (CLAUDE-tooling.md, "THE
#    BAR'S SCRIPTS ARE POSIX-BOUND"). tools/win-env.sh derives the developer
#    environment; this block answers the four names the scripts ask for:
#      LLVM_PREFIX   the install clang came from (/c/Program Files/LLVM)
#      LLC / OPT     tools/win/llc and tools/win/opt — `clang -c` and
#                    `clang -emit-llvm` under the spelling the scripts use;
#                    the installer ships neither tool (nor llvm-link, which
#                    stays EMPTY: tools/link-lto.sh then falls back to its
#                    archive link, and the seed builds go through
#                    tools/win-lto.sh, i.e. clang -flto=full + lld-link)
#      LLVM_LIBDIR   in the 8.3 spelling: cli.scaly hands -L through system()
#                    UNQUOTED and `Program Files` has a space
#      LLVM_LIBNAME  LLVM-C — the C API is what the compiler links there
#    POSIX hosts never enter this block; every value below it is theirs.
SCALY_COFF=${SCALY_COFF:-0}
case "$(uname -s)" in
  MINGW*|MSYS*|CYGWIN*)
    . "$(dirname "${BASH_SOURCE[0]}")/win-env.sh" || { llvm_env_ok=0; return 1 2>/dev/null || exit 1; }
    if [ -z "${LLVM_PREFIX:-}" ]; then
      LLVM_PREFIX="$(cd "$(dirname "$(command -v clang)")/.." && pwd)"
    fi
    LLC=${LLC:-$SCALY_WIN_TOOLS/llc}
    OPT=${OPT:-$SCALY_WIN_TOOLS/opt}
    if [ -z "${LLVM_LIBDIR:-}" ]; then
      LLVM_LIBDIR=$(cygpath -u "$(cygpath -d "$LLVM_PREFIX/lib")")
    fi
    LLVM_LIBNAME=${LLVM_LIBNAME:-LLVM-C}
    ;;
esac

# 1. LLVM prefix
if [ -z "${LLVM_PREFIX:-}" ]; then
  if [ -n "${LLVM20:-}" ]; then
    LLVM_PREFIX="$LLVM20"
  elif command -v brew >/dev/null 2>&1 && brew --prefix llvm@20 >/dev/null 2>&1; then
    LLVM_PREFIX="$(brew --prefix llvm@20)"
  elif [ -d /usr/lib/llvm-20 ]; then
    LLVM_PREFIX=/usr/lib/llvm-20
  elif command -v llvm-config-20 >/dev/null 2>&1; then
    LLVM_PREFIX="$(llvm-config-20 --prefix)"
  elif command -v llvm-config >/dev/null 2>&1 && [ "$(llvm-config --version | cut -d. -f1)" = "20" ]; then
    LLVM_PREFIX="$(llvm-config --prefix)"
  fi
fi
LLVM_PREFIX=${LLVM_PREFIX:-}

# 2. llc — prefer the prefix's own.
#    ★The LIBRARY version below is the binding one; llc's is not. libLLVM is
#    what the compiler links and what PRINTS the IR, so it decides emission —
#    measured 2026-08-11 across the 18→20 move: the whole ~20 MB seed diff was
#    the `target datalayout` line plus `getelementptr inbounds` gaining `nuw`,
#    with zero remainder. `llc` by contrast only translates the seed TEXT, and
#    a newer one reads it happily: LLVM 20.1.8's llc/opt/llvm-link build a
#    compiler from the committed seed that re-emits byte-identically (stage 7's
#    Windows rung 12 relies on exactly that, with llc 20 against libLLVM 18).
#    So a version skew here is not automatically wrong — but keep both at the
#    same major unless a rung documents why not.
#    ★The skew is ONE-DIRECTIONAL: llc may be AHEAD of libLLVM, never behind.
#    An older llc cannot read a newer seed at all — today's carries 29 478
#    `getelementptr inbounds nuw` (LLVM-19 syntax) and llc-18 stops at the first
#    one with `error: expected type` under the `nuw` (measured 2026-08-14).
#    Note the failure below reads `llc (LLVM 20) not found`, which looks like a
#    PATH problem and is usually a missing PACKAGE: llc/opt/llvm-link ship in
#    Ubuntu's `llvm-20`, NOT in `llvm-20-dev`, and libLLVM-20.so may already be
#    present as another package's dependency. See CLAUDE.md's Dependencies.
if [ -z "${LLC:-}" ]; then
  for cand in "$LLVM_PREFIX/bin/llc" "$LLVM_PREFIX/bin/llc-20" llc-20; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then LLC="$cand"; break; fi
  done
fi

# 3. lib dir + libLLVM name (so the final link finds -lLLVM-20)
LLVM_LIBDIR=${LLVM_LIBDIR:-$LLVM_PREFIX/lib}
LLVM_LIBNAME=${LLVM_LIBNAME:-LLVM-20}

# 3b. opt + llvm-link (optional — used for the whole-program -O2 seed build;
#     the pipeline falls back to per-module llc when either is missing, so
#     their absence never fails validation).
if [ -z "${OPT:-}" ]; then
  for cand in "$LLVM_PREFIX/bin/opt" "$LLVM_PREFIX/bin/opt-20" opt-20; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then OPT="$cand"; break; fi
  done
fi
if [ -z "${LLVM_LINK:-}" ]; then
  for cand in "$LLVM_PREFIX/bin/llvm-link" "$LLVM_PREFIX/bin/llvm-link-20" llvm-link-20; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then LLVM_LINK="$cand"; break; fi
  done
fi

# 3c. llvm-split (optional — tools/llc-split.sh's parallel codegen; absent,
#     one llc runs over the whole module as before). Not on the Windows box:
#     its llc is a clang stand-in.
if [ -z "${LLVM_SPLIT:-}" ] && [ "$SCALY_COFF" != "1" ]; then
  for cand in "$LLVM_PREFIX/bin/llvm-split" "$LLVM_PREFIX/bin/llvm-split-20" llvm-split-20; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then LLVM_SPLIT="$cand"; break; fi
  done
fi

LLC=${LLC:-}
OPT=${OPT:-}
LLVM_LINK=${LLVM_LINK:-}
LLVM_SPLIT=${LLVM_SPLIT:-}

# 4. report / validate
llvm_env_ok=1
[ -n "$LLVM_PREFIX" ] && [ -d "$LLVM_PREFIX" ] || { echo "llvm-env: LLVM-20 prefix not found (set LLVM20=/path)"; llvm_env_ok=0; }
[ -n "$LLC" ] || { echo "llvm-env: llc (LLVM 20) not found on PATH or in \$LLVM_PREFIX/bin"; llvm_env_ok=0; }
if [ "$llvm_env_ok" = "1" ]; then
  echo "llvm-env: prefix=$LLVM_PREFIX  llc=$LLC  lib=$LLVM_LIBDIR (-l$LLVM_LIBNAME)"
fi
export LLVM_PREFIX LLC LLVM_LIBDIR LLVM_LIBNAME OPT LLVM_LINK LLVM_SPLIT SCALY_COFF
