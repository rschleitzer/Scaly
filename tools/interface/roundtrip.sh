#!/bin/bash
# tools/interface/roundtrip.sh <package> <dependent-root> [scalyc-binary]
#
# The interface's proof: emit <dependent-root> once with <package> loaded from
# its SOURCES and once from its generated interface, and compare the IR byte
# for byte. Anything a caller depends on that the interface failed to carry
# (a caller page, a page-set membership, a residency, a read) shows up as a
# difference. The sources side is a SCALY_HOME in which the package has no
# interface directory, so the loader falls back to the sources.
set -u
cd "$(dirname "$0")/../.."
PKG="$1"; DEP="$2"; BIN="${3:-scalyc/build/scalyc}"
case "$BIN" in /*) ;; *) BIN="$PWD/$BIN";; esac
T="$(mktemp -d)"; H="$T/home"
mkdir -p "$H/packages/$PKG/0.1.0"
for d in packages/*; do n=$(basename "$d"); [ "$n" = "$PKG" ] || ln -s "$PWD/$d" "$H/packages/$n"; done
# the package WITHOUT its interface directory: sources only
for f in packages/$PKG/0.1.0/*; do
  [ "$(basename "$f")" = interface ] || ln -s "$PWD/$f" "$H/packages/$PKG/0.1.0/$(basename "$f")"
done
DEPH="$DEP"; case "$DEP" in packages/*) ;; *) DEPH="$PWD/$DEP";; esac
b=$(basename "$DEP" .scaly)
( ulimit -s 65520; "$BIN" -S -o "$T/if.ll" "$DEP" > "$T/if.log" 2>&1 )
( ulimit -s 65520; cd "$H" && SCALY_HOME="$H" "$BIN" -S -o "$T/src.ll" "$DEPH" > "$T/src.log" 2>&1 )
if cmp -s "$T/src.ll" "$T/if.ll"; then echo "roundtrip: $PKG -> $b IDENTICAL"; rc=0
else echo "roundtrip: $PKG -> $b DIFFERS (logs in $T)"; head -3 "$T/if.log" "$T/src.log"; rc=1; fi
[ $rc = 0 ] && rm -rf "$T"
exit $rc
