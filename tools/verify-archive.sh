#!/bin/bash
# tools/verify-archive.sh <version> [system] [--from <dir>] -- rebuild an
# archive that was handed out and compare it with the one handed out.
#
#   1. the archive, its .buildinfo and the profile it names, from
#      https://scaly.io/downloads (or <dir>); the profile checked against
#      the SHA-256 in the buildinfo
#   2. the commit the buildinfo names, in a worktree of this checkout
#   3. this machine's tools against the buildinfo's: a difference is said,
#      and the rebuild goes on (an archive is the same bytes only where the
#      tools are the same; where they are not, it says how far it got)
#   4. the build script of the archive's system there, with that profile
#      and the commit's time
#   5. the bytes: the same archive -- or, file by file, what differs
#
# <system> is this machine's by default (darwin-arm64, linux-x86_64, ...);
# a Linux archive is rebuilt in the container tools/make-bindist-linux.sh
# makes it in, a Windows one on Windows. The seed travels with the commit:
# nothing of a compiler on this machine takes part. Exit 0 where the bytes
# are the same, 1 where they differ, 2 where it could not rebuild.
set -u
cd "$(dirname "$0")/.."
. tools/repro.sh
ROOT="$PWD"

VERSION=""; SYSTEM=""; FROM=""
while [ $# -gt 0 ]; do
  case "$1" in
    --from) FROM="$2"; shift 2 ;;
    *) if [ -z "$VERSION" ]; then VERSION="$1"; else SYSTEM="$1"; fi; shift ;;
  esac
done
[ -n "$VERSION" ] || { echo "usage: tools/verify-archive.sh <version> [system] [--from <dir>]"; exit 2; }
if [ -z "$SYSTEM" ]; then
  case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) SYSTEM="windows-$( [ "$(uname -m)" = aarch64 ] || [ "${PROCESSOR_ARCHITECTURE:-}" = ARM64 ] && echo arm64 || echo x86_64)" ;;
    *) SYSTEM="$(uname -s | tr '[:upper:]' '[:lower:]')-$(uname -m)" ;;
  esac
fi
case "$SYSTEM" in
  windows-*) ARCHIVE="scaly-$VERSION-$SYSTEM.zip"; SCRIPT=tools/make-bindist-windows.sh ;;
  linux-*)   ARCHIVE="scaly-$VERSION-$SYSTEM.tar.gz"; SCRIPT=tools/make-bindist-linux.sh ;;
  *)         ARCHIVE="scaly-$VERSION-$SYSTEM.tar.gz"; SCRIPT=tools/make-bindist.sh ;;
esac

WORK="$(mktemp -d)"
TREE="$WORK/tree"
cleanup() {
  git -C "$ROOT" worktree remove --force "$TREE" > /dev/null 2>&1
  rm -rf "$WORK"
}
trap cleanup EXIT

# 1. what was handed out
fetch() {
  if [ -n "$FROM" ]; then cp "$FROM/$1" "$WORK/got/$1" 2>/dev/null
  else curl -fsSL "https://scaly.io/downloads/$1" -o "$WORK/got/$1"; fi
}
mkdir -p "$WORK/got" "$WORK/made"
fetch "$ARCHIVE" || { echo "verify: no $ARCHIVE"; exit 2; }
fetch "$ARCHIVE.buildinfo" || { echo "verify: no $ARCHIVE.buildinfo -- an archive from before 0.2.36 says nothing about how it was made"; exit 2; }
INFO="$WORK/got/$ARCHIVE.buildinfo"
field() { sed -n "s/^$1 //p" "$INFO" | head -1; }
COMMIT="$(field commit)"; EPOCH="$(field epoch)"; PROF_SUM="$(field profile)"
echo "verify: $ARCHIVE, commit $COMMIT"
PROFILE=none
if [ "$PROF_SUM" != none ]; then
  fetch "scaly-$VERSION-$SYSTEM.profdata" || { echo "verify: no scaly-$VERSION-$SYSTEM.profdata"; exit 2; }
  PROFILE="$WORK/got/scaly-$VERSION-$SYSTEM.profdata"
  [ "$(repro_sha256 "$PROFILE")" = "$PROF_SUM" ] || { echo "verify: the profile is not the one the buildinfo names"; exit 2; }
fi

# 2. the commit
git -C "$ROOT" cat-file -e "$COMMIT^{commit}" 2>/dev/null || git -C "$ROOT" fetch -q origin "$COMMIT" 2>/dev/null
git -C "$ROOT" worktree add -q --detach "$TREE" "$COMMIT" || { echo "verify: commit $COMMIT is not in this checkout"; exit 2; }

# 3. the tools: what the rebuild will be made with, against what made it
( cd "$TREE" && . tools/repro.sh && source tools/llvm-env.sh > /dev/null 2>&1
  repro_buildinfo "$WORK/here.buildinfo" "$VERSION" "$SYSTEM" "$PROFILE" "$TREE" LICENSE )
same_tools=1
for k in llvm clang ld sdk brew rpm os msvc; do
  a="$(sed -n "s/^$k //p" "$INFO" | head -1)"
  b="$(sed -n "s/^$k //p" "$WORK/here.buildinfo" | head -1)"
  [ -z "$a" ] && continue
  if [ "$a" != "$b" ]; then
    echo "verify: NOTE -- $k differs: made with '$a', here '$b'"
    same_tools=0
  fi
done
[ "$same_tools" = 1 ] && echo "verify: the tools are the ones it was made with"

# 4. the rebuild
echo "verify: rebuilding (tools/$(basename "$SCRIPT"))"
( cd "$TREE" && SOURCE_DATE_EPOCH="$EPOCH" SCALY_PGO_PROFILE="$PROFILE" "$SCRIPT" "$VERSION" "$WORK/made" ) > "$WORK/build.log" 2>&1 \
  || { tail -15 "$WORK/build.log"; echo "verify: the rebuild failed"; exit 2; }

# 5. the bytes
if cmp -s "$WORK/got/$ARCHIVE" "$WORK/made/$ARCHIVE"; then
  echo "verify: SAME -- $ARCHIVE is the archive its commit and profile make ($(repro_sha256 "$WORK/made/$ARCHIVE"))"
  exit 0
fi
echo "verify: DIFFERENT -- $ARCHIVE is not the bytes the rebuild made"
diff <(grep '^file ' "$INFO") <(grep '^file ' "$WORK/made/$ARCHIVE.buildinfo") | sed -n 's/^[<>] file [0-9a-f]* /  differs: /p' | sort -u
[ "$same_tools" = 1 ] || echo "verify: (the tools differ -- see the notes above)"
exit 1
