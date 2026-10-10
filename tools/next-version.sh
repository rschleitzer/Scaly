#!/bin/bash
# tools/next-version.sh <package> <version> -- begin the next version of a
# package of this tree.
#
# A published version does not change (packages/<package>/published names
# them; `scaly publish` writes it). So the first change after a publish begins
# a new version: the package's directory is RENAMED to the new number -- the
# tree holds one directory per package, the published one lives on at its
# commit --, and every file that names the old directory follows: the
# scripts, the suites and their expected outputs, the documents.
#
# What does NOT follow: a declaration. `package scaly 0.1.0` names a minimum,
# and the loader gives it the highest version of that line that is there
# (Modeler.version_at#), so this tree's roots, the fixtures and every
# published package go on saying what they said.
#
# The same renames a version that is not published yet -- 0.1.1 to 0.2.0,
# when `scaly publish --check` says the change takes the second number.
#
# After it for scaly, scalyc or scalyls: the seed names the old paths in its
# text until it is made again -- tools/bar.sh (or tools/bootstrap.sh and
# tools/seed.sh) before the commit.
set -eu
cd "$(dirname "$0")/.."
[ $# = 2 ] && [ -d "packages/$1" ] || { echo "usage: tools/next-version.sh <package> <version>" >&2; exit 2; }
pkg="$1"; new="$2"
old="$(tools/version.sh "$pkg")"
echo "$new" | grep -q '^[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*$' || { echo "next-version: $new is no version (three numbers)" >&2; exit 2; }
[ "$(printf '%s\n%s\n' "$old" "$new" | sort -t. -k1,1n -k2,2n -k3,3n | tail -1)" = "$new" ] && [ "$old" != "$new" ] \
  || { echo "next-version: $pkg is at $old, $new is not higher" >&2; exit 1; }
[ -z "$(git status --porcelain)" ] || { echo "next-version: the tree has uncommitted changes -- commit them first" >&2; exit 1; }

git mv "packages/$pkg/$old" "packages/$pkg/$new"
n=0
for f in $(git grep -lI -F "packages/$pkg/$old" -- . ':!seed' ':!packages/*/published' || true); do
  before="$(cksum < "$f")"
  perl -pi -e "s{packages/\\Q$pkg\\E/\\Q$old\\E(?![0-9.])}{packages/$pkg/$new}g" "$f"
  [ "$(cksum < "$f")" = "$before" ] || n=$((n+1))
done
echo "next-version: $pkg $old -> $new (packages/$pkg/$new; $n files named the old directory)"
if [ -f "packages/$pkg/published" ] && grep -q "^$old " "packages/$pkg/published"; then
  echo "  $old is published: it stays what it was, at its commit."
else
  echo "  $old was not published: this is a renumbering."
fi
echo "  Next: tools/bar.sh, commit."
