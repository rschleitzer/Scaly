#!/bin/bash
# tools/version.sh <package> -- the version of a package of this tree that is
# being worked on: the highest version directory under packages/<package>.
# Every script asks here; none names a version itself. (A package's versions
# are directories, and the published ones leave the tree for its history when
# the directory is renamed for the next -- packages/<package>/published names
# those, tools/next-version.sh renames.)
cd "$(dirname "$0")/.." || exit 2
[ $# = 1 ] && [ -d "packages/$1" ] || { echo "usage: tools/version.sh <package>" >&2; exit 2; }
v="$(ls "packages/$1" | grep '^[0-9][0-9.]*$' | sort -t. -k1,1n -k2,2n -k3,3n | tail -1)"
[ -n "$v" ] || { echo "version.sh: packages/$1 holds no version directory" >&2; exit 1; }
echo "$v"
