#!/bin/bash
# Publish the public Scaly installer to scaly.io.
#
# Uploads two things:
#   - install.sh           -> s3://scaly.io/install.sh       (the curl|sh bootstrap)
#   - scaly-<ver>.tar.gz   -> s3://scaly.io/downloads/...     (seed + stdlib + LICENSE)
# then invalidates the CloudFront cache for both. The tarball lives under the
# /downloads/ prefix, which docs/deploy.sh excludes from its --delete sync, so a
# routine docs deploy never removes it.
#
# Run AFTER the seed is current (tools/seed.sh + tools/install-seed.sh) so the
# published compiler matches the repo.
#
# Usage: tools/publish-install.sh [version]   (default 0.1.0)
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-0.1.0}"
TARBALL="dist/scaly-$VERSION.tar.gz"
DIST_ID=E3INKQI1B221G9   # same CloudFront distribution as docs/deploy.sh

command -v aws >/dev/null 2>&1 || { echo "publish-install: FAIL — aws CLI not found"; exit 1; }

# Build the tarball fresh from the committed seed + stdlib.
tools/make-dist.sh "$VERSION"

echo "publish-install: uploading install.sh + $TARBALL"
aws s3 cp docs/website/install.sh "s3://scaly.io/install.sh" \
    --content-type 'text/x-shellscript'
aws s3 cp "$TARBALL" "s3://scaly.io/downloads/scaly-$VERSION.tar.gz" \
    --content-type 'application/gzip'

aws cloudfront create-invalidation --distribution-id "$DIST_ID" \
    --paths "/install.sh" "/downloads/scaly-$VERSION.tar.gz"

echo "publish-install: OK"
echo "  users install with:  curl -fsSL https://scaly.io/install.sh | sh"
