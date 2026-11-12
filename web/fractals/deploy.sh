#!/bin/bash
# web/fractals/deploy.sh -- the fractal showcase to
# https://scaly.io/play/fractals/, from what web/fractals/build.sh left in
# web/fractals/site/. The page needs COOP and COEP (its threads share
# memory), which the distribution's behavior for /play/* adds
# (web/repl/deploy.sh says how). docs/deploy.sh leaves play/ out of its sync.
# The editor builds with the compiler of /play/ (web/repl/deploy.sh): that
# goes up first, built from the same tree. The showcase's first address,
# /play/mandelbrot/, gets a page that sends a reader on (moved.html).
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
SITE="$HERE/site"
B=s3://scaly.io/play/fractals
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$SITE/fractals.wasm" ]; then
  echo "web/fractals/deploy.sh: no $SITE/fractals.wasm -- run web/fractals/build.sh first" >&2
  exit 3
fi
up() { aws s3 cp --only-show-errors "$SITE/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in runner.mjs wasi.mjs threads.mjs thread.mjs gpu.mjs gpu-host.mjs gpu-worker.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up fractals.scaly 'text/plain; charset=utf-8'
up fractals.wasm application/wasm
aws s3 cp --only-show-errors "$HERE/moved.html" s3://scaly.io/play/mandelbrot/index.html --cache-control no-cache --content-type 'text/html; charset=utf-8'
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/fractals/*' '/play/mandelbrot/*' --query Invalidation.Id --output text
echo "web/fractals/deploy.sh: https://scaly.io/play/fractals/"
