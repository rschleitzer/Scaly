#!/bin/bash
# finish.sh <dir> -- fetch ~/results from the host into <dir>, then
# terminate the host (it costs while it runs). Nothing else is deleted: the
# key pair and the security group stay for the next run.
set -eu
DIR="$1"
# shellcheck disable=SC1090
. "$HOME/.scaly-bench"
mkdir -p "$DIR"
scp -i "$PEM" -q -r "ubuntu@$HOST:results/." "$DIR/"
echo "finish.sh: results in $DIR"
aws ec2 terminate-instances --region "$REGION" --instance-ids "$ID" > /dev/null
aws ec2 wait instance-terminated --region "$REGION" --instance-ids "$ID"
echo "finish.sh: $ID terminated"
