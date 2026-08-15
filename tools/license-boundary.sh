#!/bin/sh
# license-boundary.sh — enforce the tscaly license boundary.
#
# packages/tscaly is Apache-2.0 (a port of microsoft/typescript-go). The rest of
# this repository is MIT, except packages/dazzle and packages/opensp, which carry
# the permissive Clark/OpenJade license. Apache-2.0 is not copyleft, so the
# mixture is fine — the hazard is the other direction: while porting, a helper in
# tscaly looks exactly like a stdlib candidate, and lifting it upward relicenses
# a piece of the MIT stdlib without anyone deciding to.
#
# THE CHECK: no tracked file outside packages/tscaly may carry the SPDX marker
# that every ported file carries.
#
# ★ SCOPE, stated because an instrument whose scope is narrower than its claim is
# the trap it exists to prevent: this catches a lifted FILE, because a ported
# file carries the marker on its first line and the marker travels with a copy.
# It does NOT catch a lifted FUNCTION pasted into an existing stdlib file — that
# copy carries no marker, and no grep can see it. That case is caught by review,
# by the rule in the root CLAUDE.md, and by writing helpers again from the
# specification instead of copying them. Do not read a green run here as proof
# that nothing was lifted.
#
# Prose MENTIONS of the license are not flagged: the search is for the SPDX
# marker, not for the string "Apache-2.0" (which legitimately appears in the root
# CLAUDE.md and in editors/vscode/package-lock.json).
#
# Only git-tracked files are scanned, which excludes the submodule's contents and
# anything gitignored (editors/vscode/node_modules) for free.

set -eu

cd "$(dirname "$0")/.."

# ★ The pattern is a REGEX that matches the marker without this file containing
# it literally — the bracket expression [.] matches a dot and is not one. Written
# plainly, this script would be its own first finding, because it is tracked and
# would scan itself. Do NOT "clean this up" into a plain string, and do NOT fix
# it by excluding this path instead: the regex costs nothing and keeps the check
# free of exemptions, while every exemption is a hole someone can park a file in.
MARKER='SPDX-License-Identifier: Apache-2[.]0'
PKG='packages/tscaly'

fail=0

# --- 1. the marker must not appear outside packages/tscaly --------------------
outside=$(git ls-files \
  | grep -v "^$PKG/" \
  | tr '\n' '\0' \
  | xargs -0 grep -l "$MARKER" 2>/dev/null || true)

if [ -n "$outside" ]; then
  echo "license-boundary: FAIL — Apache-2.0 marker outside $PKG:" >&2
  echo "$outside" | sed 's/^/    /' >&2
  echo >&2
  echo "  Code from $PKG has been moved into an MIT-licensed part of this" >&2
  echo "  repository. Revert it. If the helper belongs in the stdlib, write it" >&2
  echo "  again from the specification with the Apache file closed, and say so" >&2
  echo "  in the commit message." >&2
  fail=1
fi

# --- 2. the package's own license paperwork must be present ------------------
# A missing LICENSE turns check 1 into a check of nothing in particular, and
# Apache-2.0 section 4 requires both files to travel with the work.
for f in "$PKG/LICENSE" "$PKG/NOTICE.txt"; do
  if [ ! -f "$f" ]; then
    echo "license-boundary: FAIL — missing $f" >&2
    fail=1
  fi
done

if [ "$fail" -ne 0 ]; then
  exit 1
fi

echo "license-boundary: OK (marker confined to $PKG; LICENSE + NOTICE.txt present)"
