#!/bin/bash
# web/repl/deploy.sh -- the REPL and the tour to https://scaly.io/play/, from
# what web/repl/build.sh left in this directory. The compiler and the standard
# library go up compressed under their plain names, with Content-Encoding: br
# (CloudFront compresses only files up to 10 MB itself).
#
# The page needs COOP and COEP (SharedArrayBuffer). The distribution's
# behavior for /play/* adds them in a CloudFront Function on the viewer
# response, scaly-play-isolation: the free plan allows no custom response
# headers policy. docs/deploy.sh leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
B=s3://scaly.io/play
DISTRIBUTION=E3INKQI1B221G9
for f in scaly.wasm.br stdlib.json.br tour-steps.htm; do
  if [ ! -f "$HERE/$f" ]; then
    echo "web/repl/deploy.sh: no $f -- run web/repl/build.sh first" >&2
    exit 3
  fi
done
up() { aws s3 cp --only-show-errors "$HERE/$1" "$B/$2" --cache-control no-cache --content-type "$3" ${4:+--content-encoding "$4"}; }
for f in index.html tour.html tour-steps.htm; do up "$f" "$f" 'text/html; charset=utf-8'; done
for f in worker.mjs side.mjs load.mjs wasi.mjs; do up "$f" "$f" 'text/javascript; charset=utf-8'; done
up scaly.wasm.br scaly.wasm application/wasm br
up stdlib.json.br stdlib.json application/json br
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/*' --query Invalidation.Id --output text
echo "web/repl/deploy.sh: https://scaly.io/play/tour.html"
