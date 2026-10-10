#!/bin/bash
# tools/published.sh -- a published version does not change: every version
# directory of this tree that its package's `published` file names is, in the
# WORKING TREE, what it was at the commit recorded there -- uncommitted edits
# and new files included. Without the network; `scaly publish --check` asks
# the remote as well and judges the new versions' numbers.
#
# A change to such a directory belongs into the next version:
#   tools/next-version.sh <package> <version>
cd "$(dirname "$0")/.." || exit 2
rc=0; n=0
for rec in packages/*/published; do
  [ -f "$rec" ] || continue
  pkg="$(basename "$(dirname "$rec")")"
  while read -r v commit tree; do
    case "$v" in ''|\;*) continue ;; esac
    [ -d "packages/$pkg/$v" ] || continue
    n=$((n+1))
    if ! git diff --quiet "$commit" -- "packages/$pkg/$v" 2>/dev/null \
       || [ -n "$(git ls-files --others --exclude-standard "packages/$pkg/$v")" ]; then
      echo "published: $pkg $v is published and differs here:"
      { git diff --stat "$commit" -- "packages/$pkg/$v"; git ls-files --others --exclude-standard "packages/$pkg/$v"; } | sed 's/^/  /' | head -8
      echo "  put the change into the next version: tools/next-version.sh $pkg <version>"
      rc=1
    fi
  done < "$rec"
done
[ $rc = 0 ] && echo "published: $n published version directories unchanged"
exit $rc
