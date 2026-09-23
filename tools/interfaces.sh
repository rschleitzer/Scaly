#!/bin/bash
# tools/interfaces.sh — the generated package interfaces.
#
#   tools/interfaces.sh [--check] [scalyc-binary]
#
# Every library package carries packages/<p>/<v>/interface/: its module tree
# with every non-generic body replaced by `linked` and the facts a caller needs
# that the compiler derives from bodies written out (tools/interface/
# geninterface.py has the list). The loader reads a package through its
# interface whenever it has one, so a dependent root sees declarations only —
# the bodies are in the package's archive, which is what AOT compilation used
# from the sources anyway.
#
# Without --check the interfaces are (re)written; with it they are generated
# into a scratch directory and compared, and a difference fails: an interface
# that no longer matches its sources would let every dependent compile against
# a package that does not exist. Packages go in DEPENDENCY order, because the
# facts of a package are computed with its dependencies loaded through THEIR
# interfaces.
set -u
cd "$(dirname "$0")/.."
# The Windows box: a real python3 ahead of the Store stub, PYTHONUTF8; silent
# elsewhere (tools/win-env.sh returns 0 without touching anything on POSIX).
. tools/win-env.sh || exit 1
CHECK=0
if [ "${1:-}" = "--check" ]; then CHECK=1; shift; fi
BIN="${1:-scalyc/build/scalyc}"
PKGS="scaly opensp dazzle scalyc scalyls tscaly scalygpu"
T="$(mktemp -d)"
# One job per package, side by side (the tscaly facts alone take ~33 s and
# 6.5 GB; the others a few seconds each); the reports print in package order.
one() {
  local p=$1 root="packages/$1/0.1.0/$1.scaly" flags="" out="packages/$1/0.1.0/interface"
  [ "$p" = scaly ] && flags="--no-prelude"
  if ! ( ulimit -s 65520; SCALY_HOME="$PWD" "$BIN" --plan --no-tests $flags --interface-facts "$root" ) > "$T/$p.facts" 2> "$T/$p.err"; then
    echo "interfaces: FAIL (facts of $p)"; head -5 "$T/$p.err"; return 1
  fi
  if [ "$CHECK" = 1 ]; then
    python3 tools/interface/geninterface.py "$root" "$T/$p.facts" --out "$T/$p" > /dev/null || return 1
    if diff -r -q "$out" "$T/$p" > "$T/$p.diff" 2>&1; then
      echo "interfaces: $p current"
    else
      echo "interfaces: STALE $p — run tools/interfaces.sh"; head -5 "$T/$p.diff"; return 1
    fi
  else
    rm -rf "$out"
    python3 tools/interface/geninterface.py "$root" "$T/$p.facts" --out "$out" | sed "s#^#interfaces: $p: #"
  fi
}
pids=""
for p in $PKGS; do
  one "$p" > "$T/$p.log" 2>&1 & pids="$pids $!"
done
rc=0
set -- $pids
for p in $PKGS; do
  wait "$1" || rc=1; shift
  cat "$T/$p.log"
done
rm -rf "$T"
exit $rc
