#!/bin/bash
# The tool beside a compiler: prints the path of `scaly` for a given `scalyc`.
#
#   tools/scaly-of.sh /tmp/scalyc_stage2        -> /tmp/scaly_stage2
#   tools/scaly-of.sh scalyc/build/scalyc       -> scalyc/build/scaly
#   tools/scaly-of.sh dist/seed/scalyc_seed     -> dist/seed/scaly_seed
#
# scalyc is the compiler (a C compiler's command line), scaly the tool (REPL,
# run, build, test). Every build that makes one makes the other beside it, the
# file names alike but for the `c`; a suite takes the compiler as its argument
# and asks here for the tool. A name that does not begin with `scalyc` has no
# sibling by this rule: the path is printed as given, and the caller's first
# `scaly build` says what is missing.
d=$(dirname "$1")
b=$(basename "$1")
case "$b" in
  scalyc*) b="scaly${b#scalyc}" ;;
esac
if [ "$d" = "." ] && [ "${1#./}" = "$1" ]; then echo "$b"; else echo "$d/$b"; fi
