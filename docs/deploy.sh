#!/bin/bash
# Deploy website to S3 and invalidate CloudFront cache.
# --exclude 'downloads/*': the install tarball lives under s3://scaly.io/downloads/
# and is published out-of-band by tools/publish-install.sh; excluding it here
# stops this whole-bucket --delete sync from removing it on a routine docs deploy.
aws s3 sync website s3://scaly.io/ --delete --exclude 'downloads/*'
aws cloudfront create-invalidation --distribution-id E3INKQI1B221G9 --paths "/*"
