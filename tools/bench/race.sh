#!/bin/bash
# tools/bench/race.sh [race.py options] — race.py with the environment it needs
# on every host: on the Windows box tests/platform.sh (through tools/win-env.sh)
# puts a real python3 on PATH, and the work directory is handed over in the
# Windows spelling, since a native python does not know Git Bash's /tmp.
cd "$(dirname "$0")/../.." || exit 1
. tests/platform.sh || exit 1
W=${BENCH_WORK:-/tmp/scaly-bench}
if [ "$SCALY_COFF" = 1 ]; then
  W=$(cygpath -w "$W")
fi
BENCH_WORK=$W exec python3 tools/bench/race.py "$@"
