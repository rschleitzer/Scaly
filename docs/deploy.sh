#!/bin/bash
# Deploy website to S3 and invalidate CloudFront cache.
# --exclude 'downloads/*': the install tarball lives under s3://scaly.io/downloads/
# and is published out-of-band by tools/publish-install.sh; excluding it here
# stops this whole-bucket --delete sync from removing it on a routine docs deploy.
# --exclude 'play/*': the REPL and the tour, published by web/repl/deploy.sh.
# --exclude 'install.*': the installers go up with tools/publish-install.sh,
# with their content types (a sync would give install.sh application/x-sh).
#
# The tour's steps (play/tour-steps.htm) are set from the tutorial's "A Tour
# of Scaly" as web/repl/build.sh sets them, and go up here too when they
# differ from what is published -- a change to the tour in the tutorial
# reaches the interactive tour with the next docs deploy.
cd "$(dirname "$0")" || exit 1
aws s3 sync website s3://scaly.io/ --delete --exclude 'downloads/*' --exclude 'play/*' --exclude 'install.sh' --exclude 'install.ps1'

DAZZLE="${DAZZLE:-../scalyc/build/dazzle}"
if [ -x "$DAZZLE" ]; then
  T="$(mktemp -d)"
  rm -f tour-steps.htm
  if SP_ENCODING=utf-8 "$DAZZLE" -t sgml -d dsssl/scaly-tour.dsl scaly-spec.xml && [ -s tour-steps.htm ]; then
    mv tour-steps.htm "$T/new.htm"
    aws s3 cp --only-show-errors s3://scaly.io/play/tour-steps.htm "$T/live.htm" 2>/dev/null
    if cmp -s "$T/new.htm" "$T/live.htm"; then
      echo "deploy.sh: the tour's steps are published as they stand"
    else
      aws s3 cp --only-show-errors "$T/new.htm" s3://scaly.io/play/tour-steps.htm \
        --cache-control no-cache --content-type 'text/html; charset=utf-8'
      echo "deploy.sh: the tour's steps published anew"
    fi
  else
    echo "deploy.sh: setting the tour's steps failed -- the interactive tour is left as it was" >&2
  fi
  rm -f tour-steps.htm
  rm -rf "$T"
else
  echo "deploy.sh: no engine at $DAZZLE -- the interactive tour is left as it was (./mkp builds it)" >&2
fi

aws cloudfront create-invalidation --distribution-id E3INKQI1B221G9 --paths "/*"
