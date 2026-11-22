#!/bin/bash
# web/matmul/deploy.sh -- the matrix product as an animation, for Machine
# Learning with Scaly (its chapter on matrices links it), to
# https://scaly.io/play/matmul/. One page, nothing built: index.html of this
# directory. docs/deploy.sh leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
B=s3://scaly.io/play/matmul
DISTRIBUTION=E3INKQI1B221G9
aws s3 cp --only-show-errors "$HERE/index.html" "$B/index.html" --cache-control no-cache --content-type 'text/html; charset=utf-8'
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/matmul/*' --query Invalidation.Id --output text
echo "web/matmul/deploy.sh: https://scaly.io/play/matmul/"
