#!/bin/bash
# tools/interfaces.sh — the generated package interfaces of this tree.
#
#   tools/interfaces.sh [scalyc-binary]
#
# A library package is read through its INTERFACE: its module tree with every
# non-generic body replaced by `linked` and the facts a caller needs that the
# compiler derives from bodies written out (`scalyc --emit-interface <root>`,
# compiler/Interface.scaly has the list). The loader reads a package through
# its interface whenever there is one, so a dependent root sees declarations
# only — the bodies are in the package's archive, which is what AOT
# compilation used from the sources anyway.
#
# THE INTERFACES LIE IN THE CACHE, NOT IN THE TREE ($SCALY_CACHE or
# ~/.scaly/cache, beside the compiled packages): each under a key over the
# compiling compiler, every file of the package and the versions of what it
# declares, so none can be stale and none is committed. The tool writes one
# whenever it compiles a package; this script does the same for the packages
# of this tree, which the suites compile with `scalyc` and no tool —
# ./build.sh, the bar and tools/seed.sh run it. Without it everything
# compiles all the same, each package read from its sources; only a seed
# root asks for them (SCALY_REQUIRE_INTERFACES in tools/seed.sh), because a
# root that uses a library package comes out differently without.
#
# Packages go in DEPENDENCY order: the facts of a package are computed with
# its dependencies loaded through THEIR interfaces, and its interface's key
# holds their interfaces' sums (Modeler.interface_sum#) -- written before
# one of them, it lies under a key no later reader asks for. A package
# declared by one that comes before it in the list is refused here (scalyls
# stood before json until 2026-10-15). One whose interface is there already
# costs a compile and writes nothing.
set -u
cd "$(dirname "$0")/.."
BIN="${1:-scalyc/build/scalyc}"
PKGS="scaly scalyc json scalyls scalygpu http compress tls pg https h3 redis sqlite tds"
T="$(mktemp -d)"
trap 'rm -rf "$T"' EXIT
rc=0
done_pkgs=" "
for p in $PKGS; do
  root="packages/$p/$(tools/version.sh "$p")/$p.scaly"; flags=""
  for d in $(sed -n 's/^package \([a-z0-9_]*\) .*/\1/p' "$root"); do
    case "$done_pkgs" in *" $d "*) ;; *) echo "interfaces: FAIL ($p declares $d, which comes after it in PKGS)"; exit 1 ;; esac
  done
  done_pkgs="$done_pkgs$p "
  [ "$p" = scaly ] && flags="--no-prelude"
  if ( ulimit -s 65520 2>/dev/null; SCALY_HOME="$PWD" "$BIN" $flags --emit-interface "$root" ) > "$T/$p.out" 2>&1; then
    sed "s#^#interfaces: $p: #" "$T/$p.out" | tail -1
  else
    echo "interfaces: FAIL ($p)"; head -5 "$T/$p.out"; rc=1
  fi
done
exit $rc
