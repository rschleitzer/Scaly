#!/bin/bash
# tools/interfaces.sh — the generated package interfaces.
#
#   tools/interfaces.sh [--check] [scalyc-binary]
#
# A library package of this tree is read through packages/<p>/<v>/interface/:
# its module tree with every non-generic body replaced by `linked` and the
# facts a caller needs that the compiler derives from bodies written out. The
# compiler writes it (`scalyc --emit-interface [-o dir] <root>`,
# compiler/Interface.scaly has the list); this script only runs it over the
# packages. The loader reads a package through its interface whenever it has
# one, so a dependent root sees declarations only — the bodies are in the
# package's archive, which is what AOT compilation used from the sources
# anyway.
#
# THE INTERFACES ARE A BUILD PRODUCT, NOT SOURCES: git ignores them,
# ./build.sh and the bar write them with the compiler they just built, and
# tools/make-dist.sh puts them into the distribution after checking them. A
# tree without them compiles all the same, each package read from its
# sources. (A package from elsewhere -- fetched, or a project's own -- gets
# its interface in the tool's cache instead, written when the package is
# compiled: tool.interface_wanted#.) AFTER CHANGING A LIBRARY PACKAGE RUN
# THIS, as after changing the compiler one builds it: a stale interface lets
# every dependent compile against a package that does not exist.
#
# Without --check the interfaces are (re)written; with it they are generated
# into a scratch directory and compared, and a difference fails. Packages go
# in DEPENDENCY order, because the facts of a package are computed with its
# dependencies loaded through THEIR interfaces.
set -u
cd "$(dirname "$0")/.."
CHECK=0
if [ "${1:-}" = "--check" ]; then CHECK=1; shift; fi
BIN="${1:-scalyc/build/scalyc}"
PKGS="scaly scalyc scalyls scalygpu http json compress tls pg https h3 redis"
T="$(mktemp -d)"
# One job per package, side by side under --check; the reports print in
# package order. (tscaly, which ran alone after them for its 6.5 GB, is a
# repository of its own since 2026-10-05 and checks its interface itself:
# packages/tscaly/tools/interface.sh there, a step of the bar's tscaly lane.)
# Either way the interface is written into $T first, so a compile that fails
# leaves the interface that lies there as it was.
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
# ★Side by side only under --check, which reads the interfaces that lie there and
# writes into $T. REWRITING goes one package at a time, in the order above: a
# dependent's facts are computed against its dependencies' interfaces, and a
# parallel rewrite handed scalyls the scalyc interface while it was
# being deleted and written (2026-09-24: "FAIL (facts of scalyls)" on the first
# run, green on the second — and had it not failed, the facts would have been
# computed against the OLD dependency interface).
rc=0
if [ "$CHECK" = 1 ]; then
  for p in $PKGS; do
    one "$p" > "$T/$p.log" 2>&1 &
    eval "pid_$p=$!"
  done
  for p in $PKGS; do
    eval "wait \$pid_$p" || rc=1
  done
  for p in $PKGS; do cat "$T/$p.log"; done
else
  for p in $PKGS; do
    one "$p" || rc=1
  done
fi
rm -rf "$T"
exit $rc
