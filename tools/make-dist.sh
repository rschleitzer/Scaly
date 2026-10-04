#!/bin/bash
# Assemble the public Scaly distribution tarball that scaly.io/install.sh fetches.
#
# Payload — what the installer needs to make the three programs (scalyc, scaly,
# scalyls) and what they need afterwards, and NONE of the compiler's sources:
#   seed/                 the seven .ll roots (the compiler, the tool and the
#                         language server as IR) and their SHA256SUMS
#   packages/<p>/<v>/     the stdlib and the standard packages as SOURCES, each
#                         with its generated interface/ — the tool compiles a
#                         package into its build cache on first use, and the
#                         stdlib's C and assembly files are the runtime shims
#                         the installer compiles for the three links
#   LICENSE, THIRD-PARTY-LICENSES.txt, VERSION
# The standard packages are the list below (ROADMAP-public.md, decision 4:
# shipped with the toolchain). Not in it: scalyc and scalyls (the repository
# is private, decision 7 — their code travels as the seed), the ports (dazzle,
# opensp, tscaly) and scalygpu.
#
# Output: <outdir>/scaly-<version>.tar.gz, <outdir> default dist/ (gitignored).
# tools/publish-install.sh uploads it to https://scaly.io/downloads/ (kept out
# of the website --delete sync so a routine docs deploy can't remove it);
# tests/install/run.sh builds one into a scratch directory and installs from it.
#
# Usage: tools/make-dist.sh [version] [outdir]   (default 0.1.0 dist)
#
# Run tools/seed.sh + tools/install-seed.sh first so seed/ is current; this
# script packages whatever is committed under seed/.
set -e
cd "$(dirname "$0")/.."
VERSION="${1:-0.1.0}"
OUT="${2:-dist}"
TARBALL="$OUT/scaly-$VERSION.tar.gz"

SEED_FILES="main scaly_main scalyc scaly scalyls_main scalyls json"
PACKAGES="scaly http https tls json compress pg redis h3"

for f in $SEED_FILES; do
  [ -f "seed/$f.ll" ] || { echo "make-dist: FAIL — seed/$f.ll missing (run tools/seed.sh + install-seed.sh)"; exit 1; }
done
[ -f seed/SHA256SUMS ] || { echo "make-dist: FAIL — seed/SHA256SUMS missing"; exit 1; }
[ -f LICENSE ] || { echo "make-dist: FAIL — LICENSE missing"; exit 1; }
[ -d "packages/scaly/$VERSION" ] || { echo "make-dist: FAIL — packages/scaly/$VERSION missing"; exit 1; }

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

mkdir -p "$STAGE/seed"
for f in $SEED_FILES; do cp "seed/$f.ll" "$STAGE/seed/"; done
cp seed/SHA256SUMS "$STAGE/seed/"

# Every version directory a package has: a program names the version it wants.
for p in $PACKAGES; do
  [ -d "packages/$p" ] || { echo "make-dist: FAIL — packages/$p missing"; exit 1; }
  mkdir -p "$STAGE/packages/$p"
  for v in packages/"$p"/*/; do
    [ -f "$v/$p.scaly" ] || continue
    cp -R "$v" "$STAGE/packages/$p/$(basename "$v")"
  done
done

cp LICENSE "$STAGE/LICENSE"
# the licenses of what is linked into the programs (LLVM and others): they
# travel with every installation, as those licenses ask
cp THIRD-PARTY-LICENSES.txt "$STAGE/THIRD-PARTY-LICENSES.txt"
printf '%s\n' "$VERSION" > "$STAGE/VERSION"

# Drop editor/OS cruft so the tarball is reproducible-ish.
find "$STAGE" \( -name '.DS_Store' -o -name '*.o' -o -name '__pycache__' \) -prune -exec rm -rf {} + 2>/dev/null || true

mkdir -p "$OUT"
# COPYFILE_DISABLE: macOS tar otherwise adds an AppleDouble `._name` beside
# every file that carries an extended attribute.
COPYFILE_DISABLE=1 tar --no-xattrs -czf "$TARBALL" -C "$STAGE" . 2>/dev/null \
  || COPYFILE_DISABLE=1 tar -czf "$TARBALL" -C "$STAGE" .

SIZE=$(du -h "$TARBALL" | cut -f1)
echo "make-dist: OK -> $TARBALL ($SIZE)"
echo "  packages: $PACKAGES"
echo "  publish with: tools/publish-install.sh $VERSION"
