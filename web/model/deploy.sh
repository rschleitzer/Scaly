#!/bin/bash
# web/model/deploy.sh -- the model page of Machine Learning with Scaly (the
# chapter's boxes open it), to https://scaly.io/play/model/, from what
# web/model/build.sh left in web/model/site/. The page needs COOP and COEP
# (its program shares memory with its threads), which the distribution's
# behavior for /play/* adds (web/repl/deploy.sh says how). docs/deploy.sh
# leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
SITE="$HERE/site"
B=s3://scaly.io/play/model
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$SITE/model.wasm" ]; then
  echo "web/model/deploy.sh: no $SITE/model.wasm -- run web/model/build.sh first" >&2
  exit 3
fi
up() { aws s3 cp --only-show-errors "$SITE/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in runner.mjs wasi.mjs threads.mjs thread.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up model.scaly 'text/plain; charset=utf-8'
up model.wasm application/wasm
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/model/*' --query Invalidation.Id --output text
echo "web/model/deploy.sh: https://scaly.io/play/model/"
