#!/bin/bash
# web/repl/deploy-dazzle.sh -- dazzle's page (web/dazzle/ of the dazzle
# checkout beside this one, or DAZZLE_REPO) to https://scaly.io/play/dazzle/.
# It builds dazzle.wasm with this tree's compiler (scalyc/build/scalyc) through
# that checkout's web/build.sh, then uploads the page, pages.mjs, the WASI
# host and the program. The page needs no cross-origin isolation (it runs
# dazzle on its own thread); under /play/ it gets it anyway, which costs
# nothing for files of one origin. docs/deploy.sh leaves play/ out of its
# sync.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
TREE="$(cd "$HERE/../.." && pwd)"
DAZZLE_REPO="${DAZZLE_REPO:-$TREE/../dazzle}"
B=s3://scaly.io/play/dazzle
DISTRIBUTION=E3INKQI1B221G9
if [ ! -f "$DAZZLE_REPO/web/dazzle/index.html" ]; then
  echo "web/repl/deploy-dazzle.sh: no dazzle checkout at $DAZZLE_REPO (set DAZZLE_REPO)" >&2
  exit 3
fi
OUT="$(mktemp -d)"
trap 'rm -rf "$OUT"' EXIT
SCALY_HOME="$TREE" "$DAZZLE_REPO/web/build.sh" dazzle "$OUT" "$TREE/scalyc/build/scalyc"
cp "$DAZZLE_REPO/web/dazzle/index.html" "$DAZZLE_REPO/web/dazzle/pages.mjs" "$OUT/"
up() { aws s3 cp --only-show-errors "$OUT/$1" "$B/$1" --cache-control no-cache --content-type "$2"; }
up index.html 'text/html; charset=utf-8'
for f in pages.mjs wasi.mjs; do up "$f" 'text/javascript; charset=utf-8'; done
up dazzle.wasm application/wasm
aws cloudfront create-invalidation --distribution-id "$DISTRIBUTION" --paths '/play/dazzle/*' --query Invalidation.Id --output text
echo "web/repl/deploy-dazzle.sh: https://scaly.io/play/dazzle/"
