#!/bin/bash
# Deploy website to S3 and invalidate CloudFront cache
aws s3 sync website s3://scaly.io/ --delete
aws cloudfront create-invalidation --distribution-id E3INKQI1B221G9 --paths "/*"
