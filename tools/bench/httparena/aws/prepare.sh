#!/bin/bash
# prepare.sh <httparena-commit> [frameworks...] -- from here, after launch.sh:
# copies the entry and the host scripts, runs setup.sh, and starts measure.sh
# under nohup so that a dropped connection does not end the run.
# Progress: ssh in and read ~/results/progress.txt (or ./status.sh).
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
# shellcheck disable=SC1090
. "$HOME/.scaly-bench"
COMMIT="$1"; shift
SSH=(ssh -i "$PEM" -o StrictHostKeyChecking=accept-new "ubuntu@$HOST")
"${SSH[@]}" 'rm -rf ~/entry && mkdir -p ~/entry'
scp -i "$PEM" -q "$HERE"/../scaly/* "ubuntu@$HOST:entry/"
scp -i "$PEM" -q "$HERE/setup.sh" "$HERE/measure.sh" "ubuntu@$HOST:"
"${SSH[@]}" "bash setup.sh $COMMIT"
"${SSH[@]}" "nohup bash measure.sh $* > measure.log 2>&1 < /dev/null &"
echo "prepare.sh: measuring; ./status.sh shows the progress"
