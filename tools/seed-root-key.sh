#!/bin/bash
# tools/seed-root-key.sh — the key of the bootstrap ROOT (/tmp/scalyc_seed_root).
#
# The ROOT is a function of the committed seed and of the script that builds it,
# so a compiler built from seed/ by tools/build-from-seed.sh under this key IS
# the ROOT. tools/bootstrap.sh reuses the cached ROOT while the key matches, and
# tools/build-from-seed.sh refreshes the cache after every build from seed/ — a
# seed refresh used to cost the NEXT bootstrap a second build of the same
# compiler (~40 s).
cd "$(dirname "$0")/.."
# shasum is Perl's; a fresh Ubuntu has only coreutils' sha256sum (same digest).
if command -v shasum >/dev/null 2>&1; then SHA256="shasum -a 256"; else SHA256="sha256sum"; fi
cat seed/main.ll seed/scalyc.ll seed/scaly.ll tools/build-from-seed.sh 2>/dev/null | $SHA256 | cut -c1-64
