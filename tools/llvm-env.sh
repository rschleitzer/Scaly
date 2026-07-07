# Sourceable LLVM-18 environment detector for the seed pipeline.
#   source tools/llvm-env.sh   ->  sets LLVM_PREFIX, LLC, LLVM_LIBDIR, LLVM_LIBNAME
# Override any of them by exporting before sourcing (e.g. LLVM18=/path).
# Mirrors build.sh's prefix logic so the C++ bootstrap and the seed agree.

# 1. LLVM prefix
if [ -z "$LLVM_PREFIX" ]; then
  if [ -n "$LLVM18" ]; then
    LLVM_PREFIX="$LLVM18"
  elif command -v brew >/dev/null 2>&1 && brew --prefix llvm@18 >/dev/null 2>&1; then
    LLVM_PREFIX="$(brew --prefix llvm@18)"
  elif [ -d /usr/lib/llvm-18 ]; then
    LLVM_PREFIX=/usr/lib/llvm-18
  elif command -v llvm-config-18 >/dev/null 2>&1; then
    LLVM_PREFIX="$(llvm-config-18 --prefix)"
  elif command -v llvm-config >/dev/null 2>&1 && [ "$(llvm-config --version | cut -d. -f1)" = "18" ]; then
    LLVM_PREFIX="$(llvm-config --prefix)"
  fi
fi

# 2. llc — must be LLVM 18 (it accepts the seed's mul/ptrtoint-GEP constexprs
#    that some system clangs reject). Prefer the prefix's own llc.
if [ -z "$LLC" ]; then
  for cand in "$LLVM_PREFIX/bin/llc" "$LLVM_PREFIX/bin/llc-18" llc-18; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then LLC="$cand"; break; fi
  done
fi

# 3. lib dir + libLLVM name (so the final link finds -lLLVM-18)
LLVM_LIBDIR=${LLVM_LIBDIR:-$LLVM_PREFIX/lib}
LLVM_LIBNAME=${LLVM_LIBNAME:-LLVM-18}

# 3b. opt + llvm-link (optional — used for the whole-program -O2 seed build;
#     the pipeline falls back to per-module llc when either is missing, so
#     their absence never fails validation).
if [ -z "$OPT" ]; then
  for cand in "$LLVM_PREFIX/bin/opt" "$LLVM_PREFIX/bin/opt-18" opt-18; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then OPT="$cand"; break; fi
  done
fi
if [ -z "$LLVM_LINK" ]; then
  for cand in "$LLVM_PREFIX/bin/llvm-link" "$LLVM_PREFIX/bin/llvm-link-18" llvm-link-18; do
    if [ -n "$cand" ] && command -v "$cand" >/dev/null 2>&1; then LLVM_LINK="$cand"; break; fi
  done
fi

# 4. report / validate
llvm_env_ok=1
[ -n "$LLVM_PREFIX" ] && [ -d "$LLVM_PREFIX" ] || { echo "llvm-env: LLVM-18 prefix not found (set LLVM18=/path)"; llvm_env_ok=0; }
[ -n "$LLC" ] || { echo "llvm-env: llc (LLVM 18) not found on PATH or in \$LLVM_PREFIX/bin"; llvm_env_ok=0; }
if [ "$llvm_env_ok" = "1" ]; then
  echo "llvm-env: prefix=$LLVM_PREFIX  llc=$LLC  lib=$LLVM_LIBDIR (-l$LLVM_LIBNAME)"
fi
export LLVM_PREFIX LLC LLVM_LIBDIR LLVM_LIBNAME OPT LLVM_LINK
