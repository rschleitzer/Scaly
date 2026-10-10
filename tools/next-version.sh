#!/bin/bash
# tools/next-version.sh <package> <version> -- begin the next version of a
# package of this tree: `scaly next`, with this tree's own tool.
#
# A published version does not change (packages/<package>/published names
# them; `scaly publish` writes it). So the first change after a publish begins
# a new version: the package's directory is RENAMED to the new number -- the
# tree holds one directory per package, the published one lives on at its
# commit --, and every tracked file that names the old directory follows: the
# scripts, the suites and their expected outputs, the documents.
#
# What does NOT follow: a declaration. `package scaly 0.1.0` names a minimum,
# and the loader gives it the highest version of that line that is there, so
# this tree's roots, the fixtures and every published package go on saying
# what they said.
#
# The same renames a version that is not published yet -- 0.1.1 to 0.2.0,
# when `scaly publish --check` says the change takes the second number.
#
# The tool is the one ./build.sh made, else the bootstrap's.
set -eu
cd "$(dirname "$0")/.."
[ $# = 2 ] || { echo "usage: tools/next-version.sh <package> <version>" >&2; exit 2; }
scaly=scalyc/build/scaly
[ -x "$scaly" ] || scaly=/tmp/scaly_stage2
[ -x "$scaly" ] || { echo "next-version: no tool -- ./build.sh or tools/bootstrap.sh first" >&2; exit 2; }
SCALY_HOME="$PWD" "$scaly" next "$1" "$2"
echo "  Next: tools/bar.sh, commit."
