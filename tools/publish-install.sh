#!/bin/bash
# Publish the public Scaly installers and what they fetch to scaly.io.
#
# Uploads:
#   - install.sh, install.ps1   -> s3://scaly.io/        (curl | sh, irm | iex)
#   - LICENSE, THIRD-PARTY-LICENSES.txt, SHA256SUMS -> s3://scaly.io/downloads/
#     (ours, those of what is linked into the programs, and the checksums of
#     the seven files)
#   - scaly-<ver>.tar.gz        -> s3://scaly.io/downloads/   (seed + packages + LICENSE),
#                                  made here by tools/make-dist.sh
#   - the SIX binary archives, every one found in dist/:
#       scaly-<ver>-darwin-{arm64,x86_64}.tar.gz
#       scaly-<ver>-linux-{aarch64,x86_64}.tar.gz
#       scaly-<ver>-windows-{arm64,x86_64}.zip
#     They are built by the `release` workflow (gh workflow run release);
#     --run <id> fetches that run's artifacts into dist/ first. A POSIX system
#     without its archive is built from the seed by install.sh; Windows has
#     no such route, so a missing zip means no Windows installation.
# then invalidates the CloudFront cache for them. Everything lies under
# /downloads/ or beside the website's files, and docs/deploy.sh excludes
# /downloads/ from its --delete sync, so a routine docs deploy removes none.
#
# Run AFTER the seed is current (tools/seed.sh + tools/install-seed.sh) so the
# published compiler matches the repo. ★The tarball is made of THIS tree and
# the archives of the run's commit: the script says so when the two differ.
#
# Usage: tools/publish-install.sh [version] [--run <id>] [--dry-run] [--partial]
#   version     default: the tree's VERSION file
#   --overwrite replace a version that is published already (a published
#               version does not change: for a botched upload only)
#   --run <id>  fetch the six archives of that release run into dist/ (gh)
#   --dry-run   say what would be uploaded, upload and invalidate nothing
#   --partial   go on although one of the six archives is missing
set -e
cd "$(dirname "$0")/.."
VERSION="$(cat VERSION)"
OVERWRITE=0
RUN=""
DRY=0
PARTIAL=0
while [ $# -gt 0 ]; do
  case "$1" in
    --run) RUN="$2"; shift ;;
    --dry-run) DRY=1 ;;
    --partial) PARTIAL=1 ;;
    --overwrite) OVERWRITE=1 ;;
    -*) echo "publish-install: unknown option $1"; exit 2 ;;
    *) VERSION="$1" ;;
  esac
  shift
done
TARBALL="dist/scaly-$VERSION.tar.gz"
DIST_ID=E3INKQI1B221G9   # same CloudFront distribution as docs/deploy.sh

command -v aws >/dev/null 2>&1 || { echo "publish-install: FAIL — aws CLI not found"; exit 1; }

# The archives of a release run: each artifact is a directory holding one file.
if [ -n "$RUN" ]; then
  command -v gh >/dev/null 2>&1 || { echo "publish-install: FAIL — gh not found (--run)"; exit 1; }
  FETCH="$(mktemp -d)"
  trap 'rm -rf "$FETCH"' EXIT
  echo "publish-install: fetching the archives of release run $RUN"
  gh run download "$RUN" -D "$FETCH" || { echo "publish-install: FAIL — gh run download $RUN"; exit 1; }
  mkdir -p dist
  find "$FETCH" -type f \( -name "scaly-$VERSION-*.tar.gz" -o -name "scaly-$VERSION-*.zip" \) -exec cp {} dist/ \;
  RUN_SHA="$(gh run view "$RUN" --json headSha --jq .headSha 2>/dev/null || true)"
  if [ -n "$RUN_SHA" ] && [ "$RUN_SHA" != "$(git rev-parse HEAD)" ]; then
    echo "publish-install: NOTE — the run built commit ${RUN_SHA:0:9}, this tree is $(git rev-parse --short=9 HEAD):"
    echo "                 the tarball (packages, seed) is made of this tree, the programs of that commit"
    if ! git diff --quiet "$RUN_SHA" HEAD -- packages seed 2>/dev/null; then
      echo "publish-install: NOTE — packages/ or seed/ differ between the two"
    fi
  fi
fi

# The license page shows LICENSE's text: the two must be the same words.
python3 - <<'PY' || { echo "publish-install: FAIL — docs/website/license/index.html does not show the text of LICENSE"; exit 1; }
import html, re, sys
page = open('docs/website/license/index.html', encoding='utf-8').read()
m = re.search(r'<pre class="license" id="license-text">(.*?)</pre>', page, re.S)
sys.exit(0 if m and html.unescape(m.group(1)).strip() == open('LICENSE', encoding='utf-8').read().strip() else 1)
PY

# Build the tarball fresh from the committed seed + stdlib.
tools/make-dist.sh "$VERSION"

# The programs, per system: what the release run (or tools/make-bindist.sh on
# that system) left in dist/.
BINARIES=()
MISSING=""
for sys in darwin-arm64 darwin-x86_64 linux-aarch64 linux-x86_64; do
  if [ -f "dist/scaly-$VERSION-$sys.tar.gz" ]; then BINARIES+=("dist/scaly-$VERSION-$sys.tar.gz"); else MISSING="$MISSING $sys"; fi
done
for sys in windows-arm64 windows-x86_64; do
  if [ -f "dist/scaly-$VERSION-$sys.zip" ]; then BINARIES+=("dist/scaly-$VERSION-$sys.zip"); else MISSING="$MISSING $sys"; fi
done
if [ -n "$MISSING" ]; then
  echo "publish-install: missing in dist/:$MISSING"
  if [ "$PARTIAL" != 1 ]; then
    echo "publish-install: FAIL — all six archives are published together (--run <id> fetches them; --partial goes on without)"
    exit 1
  fi
fi

# A published version does not change: its archives are out, installed and
# cached under their names. What is there already is not replaced -- the next
# upload is the next version (the VERSION file).
if [ "$OVERWRITE" = 0 ] && [ "$DRY" = 0 ] && curl -fsSI "https://scaly.io/downloads/scaly-$VERSION.tar.gz?t=$(date +%s)" > /dev/null 2>&1; then
  echo "publish-install: FAIL — version $VERSION is published: a published version does not change. Raise the VERSION file (or --overwrite for a botched upload)."
  exit 1
fi

# What a careful reader checks: SHA256SUMS of the seven files, in the form
# `shasum -a 256 -c` reads.
( cd dist && shasum -a 256 "scaly-$VERSION.tar.gz" $(for b in "${BINARIES[@]}"; do basename "$b"; done) | sed 's/ \*/  /' > SHA256SUMS )
# upload <file> <key> <content-type>
PATHS=()
upload() {
  PATHS+=("/$2")
  if [ "$DRY" = 1 ]; then
    printf 'publish-install: would upload %-44s -> s3://scaly.io/%s (%s bytes)\n' "$1" "$2" "$(wc -c < "$1" | tr -d ' ')"
  else
    echo "publish-install: uploading $1"
    aws s3 cp "$1" "s3://scaly.io/$2" --content-type "$3"
  fi
}
# under downloads/: docs/deploy.sh syncs the website with --delete and spares
# only that prefix (the page docs/website/license/ links to both)
upload LICENSE downloads/LICENSE 'text/plain; charset=utf-8'
upload THIRD-PARTY-LICENSES.txt downloads/THIRD-PARTY-LICENSES.txt 'text/plain; charset=utf-8'
upload dist/SHA256SUMS downloads/SHA256SUMS 'text/plain; charset=utf-8'
# ... and the same under the version's name: an older version stays what it
# was and stays there
cp dist/SHA256SUMS "dist/SHA256SUMS-$VERSION"
upload "dist/SHA256SUMS-$VERSION" "downloads/SHA256SUMS-$VERSION" 'text/plain; charset=utf-8'
upload "$TARBALL" "downloads/scaly-$VERSION.tar.gz" 'application/gzip'
for b in "${BINARIES[@]}"; do
  case "$b" in
    *.zip) upload "$b" "downloads/$(basename "$b")" 'application/zip' ;;
    *)     upload "$b" "downloads/$(basename "$b")" 'application/gzip' ;;
  esac
done
# the pointer after the archives it names, and the installers LAST: they read
# downloads/latest and fetch what it names, so all of it is there before them
printf '%s\n' "$VERSION" > dist/latest
upload dist/latest downloads/latest 'text/plain; charset=utf-8'
upload docs/website/install.sh  install.sh  'text/x-shellscript'
upload docs/website/install.ps1 install.ps1 'text/plain; charset=utf-8'

# Publish the VS Code extension under a STABLE name so the tutorial's install
# command never goes stale. Picks the newest committed .vsix; rebuild it with
# `cd editors/vscode && npm run package` before publishing a new version.
VSIX=$(ls -t editors/vscode/scaly-*.vsix 2>/dev/null | head -1)
if [ -n "$VSIX" ]; then
  upload "$VSIX" downloads/scaly-vscode.vsix 'application/octet-stream'
fi

if [ "$DRY" = 1 ]; then
  echo "publish-install: would invalidate ${#PATHS[@]} paths of distribution $DIST_ID"
  echo "publish-install: DRY RUN — nothing was uploaded"
  exit 0
fi
aws cloudfront create-invalidation --distribution-id "$DIST_ID" --paths "${PATHS[@]}"

echo "publish-install: OK"
echo "  users install with:  curl -fsSL https://scaly.io/install.sh | sh"
echo "  on Windows:          irm https://scaly.io/install.ps1 | iex"
echo "  VS Code extension:   https://scaly.io/downloads/scaly-vscode.vsix"
