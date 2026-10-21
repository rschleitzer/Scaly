#!/bin/bash
# status.sh -- the run's progress on the host, and how long it has left
# before it ends itself.
set -eu
# shellcheck disable=SC1090
. "$HOME/.scaly-bench"
ssh -i "$PEM" "ubuntu@$HOST" 'cat ~/results/progress.txt 2>/dev/null; echo; uptime; ls /run/systemd/shutdown/scheduled > /dev/null 2>&1 && cat /run/systemd/shutdown/scheduled'
