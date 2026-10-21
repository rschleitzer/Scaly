#!/bin/bash
# finish.sh <dir> -- fetch ~/results from the host into <dir>, then
# terminate the host (it costs while it runs). Nothing else is deleted: the
# key pair and the security group stay for the next run.
set -eu
DIR="$1"
# shellcheck disable=SC1090
. "$HOME/.scaly-bench"
mkdir -p "$DIR"
# a host without results (a run of its own, a failed setup) still ends
if scp -i "$PEM" -q -r "ubuntu@$HOST:results/." "$DIR/"; then
  echo "finish.sh: results in $DIR"
else
  echo "finish.sh: no results fetched"
fi
aws ec2 terminate-instances --region "$REGION" --instance-ids "$ID" > /dev/null
aws ec2 wait instance-terminated --region "$REGION" --instance-ids "$ID"
echo "finish.sh: $ID terminated"
