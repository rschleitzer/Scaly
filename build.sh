#!/bin/bash
set -e

# Build the Scaly compiler from the committed .ll seed.
#
# The C++ stage-0 compiler is retired (sources frozen under retired/scalyc0/);
# the bootstrap root is the self-hosted seed in seed/ (see tools/seed.sh and
# tools/build-from-seed.sh). This script regenerates the grammar-derived
# sources (parser.scaly, Syntax.scaly, literate tests, docs) and builds the
# seed compiler at the canonical path scalyc/build/scalyc.
#
#   ./build.sh          regenerate + build seed compiler
#   ./build.sh test     ... then run the regression + literate suites on it

# Regenerate from spec
if [ -f ./mkp ]; then
    ./mkp
fi

tools/build-from-seed.sh scalyc/build/scalyc
# the packages' interfaces are a build product of the compiler just built
tools/interfaces.sh scalyc/build/scalyc

if [ "$1" = "test" ]; then
    tests/regress/run.sh scalyc/build/scalyc
    tests/selfhosted/run.sh scalyc/build/scalyc
    tests/target/run.sh scalyc/build/scalyc
    tests/fiber/run.sh scalyc/build/scalyc
fi
