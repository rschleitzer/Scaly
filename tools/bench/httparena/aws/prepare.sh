#!/bin/bash
# prepare.sh <httparena-commit> [frameworks...] -- from here, after launch.sh:
# copies the entry and the host scripts, runs setup.sh, and starts measure.sh
# under nohup so that a dropped connection does not end the run.
# Progress: ssh in and read ~/results/progress.txt (or ./status.sh).
# PROFILE="<name> ..." measures those profiles only (e.g. PROFILE=async).
# VARIANTS="<name>=<patch>[+<patch>...] ..." adds the entries scaly-<name>
# (variant.sh): the packages of this tree at HEAD with the patches applied
# (git diff output; none for HEAD itself), built by the released toolchain
# beside the released packages -- an A/B of a runtime change without a
# release. Name them among the frameworks to measure them.
# MEASURE=0 prepares the host and starts nothing (profile.sh by hand).
# VALIDATE="<entry> ..." validates those entries first (default scaly).
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
# shellcheck disable=SC1090
. "$HOME/.scaly-bench"
COMMIT="$1"; shift
SSH=(ssh -i "$PEM" -o StrictHostKeyChecking=accept-new "ubuntu@$HOST")
"${SSH[@]}" 'rm -rf ~/entry && mkdir -p ~/entry'
scp -i "$PEM" -q "$HERE"/../scaly/* "ubuntu@$HOST:entry/"
"${SSH[@]}" 'rm -rf ~/scaly_nginx ~/scaly_caddy'
scp -i "$PEM" -q -r "$HERE/../scaly_nginx" "$HERE/../scaly_caddy" "ubuntu@$HOST:"
scp -i "$PEM" -q "$HERE/setup.sh" "$HERE/measure.sh" "$HERE/variant.sh" "$HERE/profile.sh" "$HERE/cpusets.py" "ubuntu@$HOST:"
"${SSH[@]}" "bash setup.sh $COMMIT"
TREE="$(cd "$HERE/../../../.." && pwd)"
for v in ${VARIANTS:-}; do
  name="${v%%=*}"
  patches="${v#*=}"
  stage="$(mktemp -d)"
  for p in scaly http json compress tls https h3 pg; do
    git -C "$TREE" archive HEAD "packages/$p/0.2.0" | tar x -C "$stage"
  done
  for patch in ${patches//+/ }; do
    patch="$(cd "$(dirname "$patch")" && pwd)/$(basename "$patch")"
    (cd "$stage" && git apply "$patch")
  done
  mkdir -p "$stage/v/packages"
  for p in scaly http json compress tls https h3 pg; do
    mv "$stage/packages/$p/0.2.0" "$stage/v/packages/$p"
  done
  "${SSH[@]}" "rm -rf ~/variants/$name && mkdir -p ~/variants/$name"
  scp -i "$PEM" -q -r "$stage/v/packages" "ubuntu@$HOST:variants/$name/"
  rm -rf "$stage"
  "${SSH[@]}" "bash variant.sh $name"
done
[ "${MEASURE:-1}" = 0 ] && { echo "prepare.sh: prepared, nothing started"; exit 0; }
"${SSH[@]}" "nohup env PROFILE='${PROFILE:-}' VALIDATE='${VALIDATE:-}' bash measure.sh $* > measure.log 2>&1 < /dev/null &"
echo "prepare.sh: measuring; ./status.sh shows the progress"
