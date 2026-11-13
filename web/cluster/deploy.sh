#!/bin/bash
# web/cluster/deploy.sh -- the cluster showcase, a render farm, to
# https://scaly.io/play/cluster/, from what web/cluster/build.sh left in
# web/cluster/site/. The page needs COOP and COEP (its nodes share memory
# with their threads), which the distribution's behavior for /play/* adds
# (web/repl/deploy.sh says how). docs/deploy.sh leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
SITE="$HERE/site"
B=s3://scaly.io/play/cluster
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$SITE/render.wasm" ]; then
  echo "web/cluster/deploy.sh: no $SITE/render.wasm -- run web/cluster/build.sh first" >&2
  exit 3
fi
up() { aws s3 cp --only-show-errors "$SITE/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in node.mjs wasi.mjs threads.mjs thread.mjs net.mjs gpu.mjs gpu-host.mjs gpu-worker.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up render.scaly 'text/plain; charset=utf-8'
up render.wasm application/wasm
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/cluster/*' --query Invalidation.Id --output text
echo "web/cluster/deploy.sh: https://scaly.io/play/cluster/"
