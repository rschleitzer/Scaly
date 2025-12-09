#!/bin/bash
# Deploy website to S3
aws s3 sync website s3://scaly.io/ --delete
