#!/bin/sh
# Publishes the Visual Studio extension as
# https://scaly.io/downloads/scaly-visualstudio.vsix, the stable name the
# tutorial and the download page link to, and invalidates that path in the
# CloudFront cache. Needs the aws CLI, signed in for the scaly.io bucket.
#
#   ./publish.sh                 build the Release VSIX here (Visual Studio,
#                                see build.sh), then upload it
#   ./publish.sh <file.vsix>     upload a VSIX built elsewhere, as from a
#                                machine without Visual Studio
#   ./publish.sh --dry-run ...   say what would be uploaded, upload nothing
set -eu

dry=false
if [ "${1:-}" = "--dry-run" ]; then
    dry=true
    shift
fi

here=$(cd "$(dirname "$0")" && pwd)
if [ $# -gt 0 ]; then
    vsix=$1
else
    vsix=$("$here/build.sh" Release | tail -n 1)
fi
[ -f "$vsix" ] || { echo "publish.sh: no VSIX at $vsix" >&2; exit 1; }

# The manifest's version, so the log says which one went up.
version=$(unzip -p "$vsix" extension.vsixmanifest | sed -n 's/.*<Identity [^>]*Version="\([^"]*\)".*/\1/p')
key=downloads/scaly-visualstudio.vsix
dist=E3INKQI1B221G9   # the distribution of docs/deploy.sh and tools/publish-install.sh

if $dry; then
    echo "publish.sh: would upload $vsix (version $version, $(wc -c < "$vsix" | tr -d ' ') bytes) -> s3://scaly.io/$key"
    exit 0
fi
command -v aws >/dev/null 2>&1 || { echo "publish.sh: no aws CLI" >&2; exit 1; }
aws s3 cp "$vsix" "s3://scaly.io/$key" --content-type application/octet-stream
aws cloudfront create-invalidation --distribution-id "$dist" --paths "/$key" > /dev/null
echo "publish.sh: version $version is https://scaly.io/$key"
