#!/bin/bash
# web/mandelbrot/deploy.sh -- the Mandelbrot showcase to
# https://scaly.io/play/mandelbrot/, from what web/mandelbrot/build.sh left
# in web/mandelbrot/site/. The page needs COOP and COEP (its threads share
# memory), which the distribution's behavior for /play/* adds
# (web/repl/deploy.sh says how). docs/deploy.sh leaves play/ out of its sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
SITE="$HERE/site"
B=s3://scaly.io/play/mandelbrot
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$SITE/mandelbrot.wasm" ]; then
  echo "web/mandelbrot/deploy.sh: no $SITE/mandelbrot.wasm -- run web/mandelbrot/build.sh first" >&2
  exit 3
fi
up() { aws s3 cp --only-show-errors "$SITE/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in runner.mjs wasi.mjs threads.mjs thread.mjs gpu.mjs gpu-host.mjs gpu-worker.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up mandelbrot.scaly 'text/plain; charset=utf-8'
up mandelbrot.wasm application/wasm
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/mandelbrot/*' --query Invalidation.Id --output text
echo "web/mandelbrot/deploy.sh: https://scaly.io/play/mandelbrot/"
