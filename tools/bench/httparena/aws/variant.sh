#!/bin/bash
# variant.sh <name> -- runs ON the host after setup.sh (prepare.sh calls it
# for every VARIANTS entry): the entry scaly-<name> beside frameworks/scaly,
# the same entry built by the same released toolchain, but with the packages
# in ~/variants/<name>/packages (prepare.sh stages them from the local tree)
# in place of the toolchain's own. benchmark.sh then measures it like any
# entry; validate.sh is not run for it.
set -eu
NAME="$1"
HA="$HOME/HttpArena"
SRC="$HOME/variants/$NAME/packages"
DIR="$HA/frameworks/scaly-$NAME"
[ -d "$SRC" ] || { echo "variant.sh: no packages at $SRC"; exit 1; }
rm -rf "$DIR"
cp -R "$HA/frameworks/scaly" "$DIR"
cp -R "$SRC" "$DIR/packages"
# the build context takes the packages along, the image its own name (the
# board's scripts do not export IMAGE_NAME to a framework's build.sh)
grep -qF 'cp "$HERE/Dockerfile" "$HERE"/*.scaly "$CTX/"' "$DIR/build.sh"
sed -i 's|cp "$HERE/Dockerfile" "$HERE"/\*.scaly "$CTX/"|cp "$HERE/Dockerfile" "$HERE"/*.scaly "$CTX/"; cp -R "$HERE/packages" "$CTX/"|' "$DIR/build.sh"
grep -qF 'IMAGE="${IMAGE_NAME:-httparena-scaly}"' "$DIR/build.sh"
sed -i "s|IMAGE=\"\${IMAGE_NAME:-httparena-scaly}\"|IMAGE=\"\${IMAGE_NAME:-httparena-scaly-$NAME}\"|" "$DIR/build.sh"
# the packages replace the toolchain's right after its installation
grep -q '^ENV PATH=/opt/scaly/bin:\$PATH$' "$DIR/Dockerfile"
P=/opt/scaly/toolchain/packages
sed -i "s|^ENV PATH=/opt/scaly/bin:\$PATH\$|&\nCOPY packages/ /tmp/packages/\nRUN for p in /tmp/packages/*; do n=\$(basename \$p); rm -rf $P/\$n/0.2.0; cp -R \$p $P/\$n/0.2.0; done; rm -rf /tmp/packages|" "$DIR/Dockerfile"
grep -qxF 'COPY packages/ /tmp/packages/' "$DIR/Dockerfile"
sed -i "s|\"display_name\": \"scaly\"|\"display_name\": \"scaly-$NAME\"|" "$DIR/meta.json"
echo "variant.sh: frameworks/scaly-$NAME with $(ls "$SRC" | tr '\n' ' ')"
