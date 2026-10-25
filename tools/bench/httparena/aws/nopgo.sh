#!/bin/bash
# nopgo.sh -- runs ON the host after setup.sh (prepare.sh calls it with
# NOPGO=1): the entry scaly-nopgo beside frameworks/scaly, the same entry
# built without the profile -- build.sh with SCALY_PGO=0, so the Dockerfile
# finds no pgo/arena.profdata and builds `--release` alone. Measured beside
# scaly, it says how much of a result the profile-guided build is.
# validate.sh is not run for it.
set -eu
HA="${HA:-$HOME/HttpArena}"
DIR="$HA/frameworks/scaly-nopgo"
rm -rf "$DIR"
cp -R "$HA/frameworks/scaly" "$DIR"
grep -qF 'if [ "${SCALY_PGO:-1}" != 0 ]; then' "$DIR/build.sh"
sed -i 's|if \[ "${SCALY_PGO:-1}" != 0 \]; then|if [ "${SCALY_PGO:-0}" != 0 ]; then|' "$DIR/build.sh"
grep -qF 'if [ "${SCALY_PGO:-0}" != 0 ]; then' "$DIR/build.sh"
# the image its own name (the board's scripts do not export IMAGE_NAME to a
# framework's build.sh)
grep -qF 'IMAGE="${IMAGE_NAME:-httparena-scaly}"' "$DIR/build.sh"
sed -i 's|IMAGE="${IMAGE_NAME:-httparena-scaly}"|IMAGE="${IMAGE_NAME:-httparena-scaly-nopgo}"|' "$DIR/build.sh"
grep -qF 'IMAGE="${IMAGE_NAME:-httparena-scaly-nopgo}"' "$DIR/build.sh"
sed -i 's|"display_name": "scaly"|"display_name": "scaly-nopgo"|' "$DIR/meta.json"
grep -qF '"display_name": "scaly-nopgo"' "$DIR/meta.json"
echo "nopgo.sh: frameworks/scaly-nopgo, built without the profile"
