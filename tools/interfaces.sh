#!/bin/bash
# tools/interfaces.sh — the generated package interfaces.
#
#   tools/interfaces.sh [--check] [scalyc-binary]
#
# Every library package carries packages/<p>/<v>/interface/: its module tree
# with every non-generic body replaced by `linked` and the facts a caller needs
# that the compiler derives from bodies written out. The compiler writes it
# (`scalyc --emit-interface [-o dir] <root>`, compiler/Interface.scaly has the
# list); this script only runs it over the packages. The loader reads a package
# through its interface whenever it has one, so a dependent root sees
# declarations only — the bodies are in the package's archive, which is what
# AOT compilation used from the sources anyway.
#
# Without --check the interfaces are (re)written; with it they are generated
# into a scratch directory and compared, and a difference fails: an interface
# that no longer matches its sources would let every dependent compile against
# a package that does not exist. Packages go in DEPENDENCY order, because the
# facts of a package are computed with its dependencies loaded through THEIR
# interfaces.
set -u
cd "$(dirname "$0")/.."
CHECK=0
if [ "${1:-}" = "--check" ]; then CHECK=1; shift; fi
BIN="${1:-scalyc/build/scalyc}"
PKGS="scaly opensp dazzle scalyc scalyls tscaly scalygpu http json compress tls pg https h3 redis"
T="$(mktemp -d)"
# One job per package, side by side under --check (tscaly alone takes ~35 s and
# 6.5 GB; the others a few seconds each); the reports print in package order.
# Either way the interface is written into $T first, so a compile that fails
# leaves the committed interface as it was.
one() {
  local p=$1 root="packages/$1/0.1.0/$1.scaly" flags="" out="packages/$1/0.1.0/interface"
  [ "$p" = scaly ] && flags="--no-prelude"
  if ! ( ulimit -s 65520; SCALY_HOME="$PWD" "$BIN" $flags --emit-interface -o "$T/$p" "$root" ) > "$T/$p.out" 2>&1; then
    echo "interfaces: FAIL ($p)"; head -5 "$T/$p.out"; return 1
  fi
  if [ "$CHECK" = 1 ]; then
    if diff -r -q "$out" "$T/$p" > "$T/$p.diff" 2>&1; then
      echo "interfaces: $p current"
    else
      echo "interfaces: STALE $p — run tools/interfaces.sh"; head -5 "$T/$p.diff"; return 1
    fi
  else
    rm -rf "$out" && mv "$T/$p" "$out" && sed "s#$T/$p#$out#; s#^#interfaces: $p: #" "$T/$p.out"
  fi
}
# ★Side by side only under --check, which reads the committed interfaces and
# writes into $T. REWRITING goes one package at a time, in the order above: a
# dependent's facts are computed against its dependencies' interfaces, and a
# parallel rewrite handed scalyls and tscaly the scalyc interface while it was
# being deleted and written (2026-09-24: "FAIL (facts of scalyls)" on the first
# run, green on the second — and had it not failed, the facts would have been
# computed against the OLD dependency interface).
rc=0
if [ "$CHECK" = 1 ]; then
  pids=""
  for p in $PKGS; do
    one "$p" > "$T/$p.log" 2>&1 & pids="$pids $!"
  done
  set -- $pids
  for p in $PKGS; do
    wait "$1" || rc=1; shift
    cat "$T/$p.log"
  done
else
  for p in $PKGS; do
    one "$p" || rc=1
  done
fi
rm -rf "$T"
exit $rc
