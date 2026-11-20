#!/bin/bash
# web/field/deploy.sh -- the field of Machine Learning with Scaly, a small
# network learning in the page, to https://scaly.io/play/field/, from what
# web/field/build.sh left in web/field/site/. The page needs COOP and COEP
# (its program shares memory with its threads), which the distribution's
# behavior for /play/* adds (web/repl/deploy.sh says how). docs/deploy.sh
# leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
SITE="$HERE/site"
B=s3://scaly.io/play/field
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$SITE/field.wasm" ]; then
  echo "web/field/deploy.sh: no $SITE/field.wasm -- run web/field/build.sh first" >&2
  exit 3
fi
up() { aws s3 cp --only-show-errors "$SITE/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in runner.mjs wasi.mjs threads.mjs thread.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up field.scaly 'text/plain; charset=utf-8'
up field.wasm application/wasm
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/field/*' --query Invalidation.Id --output text
echo "web/field/deploy.sh: https://scaly.io/play/field/"
