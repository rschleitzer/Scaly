#!/bin/bash
# The 10k demo (milestone 1.6): compile and run the 10,000-connection
# fiber-per-connection HTTP benchmark. Kept out of tests/fiber/run.sh's
# glob (bench/ subdirectory) because it needs ~20,050 file descriptors —
# this script raises the soft limit itself and reports the wall time.
#
# Usage: tests/fiber/bench/run.sh [stage-binary]   (default /tmp/scalyc_stage2)
cd "$(dirname "$0")/../../.." || exit 1
STAGE=${1:-/tmp/scalyc_stage2}

NEED=20500
hard=$(ulimit -Hn)
if [ "$hard" != "unlimited" ] && [ "$hard" -lt "$NEED" ]; then
  echo "http10k: SKIP (fd hard limit $hard < $NEED)"
  exit 0
fi
# On darwin, setrlimit happily accepts values ABOVE kern.maxfilesperproc but
# the kernel still caps there (accept then fails with EMFILE mid-benchmark) —
# so the sysctl, not ulimit, is the real gate. CI raises it with sudo.
if [ "$(uname -s)" = "Darwin" ]; then
  mfp=$(sysctl -n kern.maxfilesperproc 2>/dev/null || echo 0)
  if [ "$mfp" -lt "$NEED" ]; then
    echo "http10k: SKIP (kern.maxfilesperproc $mfp < $NEED)"
    exit 0
  fi
fi
if ! ulimit -n $NEED 2>/dev/null; then
  echo "http10k: SKIP (cannot raise fd soft limit to $NEED)"
  exit 0
fi

# Top up an older archive (see tests/fiber/run.sh).
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^fcontext\.o$'; then
  tools/fcontext.sh /tmp/fcontext.o && ar rcs /tmp/libscaly.a /tmp/fcontext.o
fi
if [ -f /tmp/libscaly.a ] && ! ar t /tmp/libscaly.a 2>/dev/null | grep -q '^eio\.o$'; then
  tools/eio.sh /tmp/eio.o && ar rcs /tmp/libscaly.a /tmp/eio.o
fi

f=tests/fiber/bench/http10k.scaly
expected=$(sed -n 's/^; Expected: //p' "$f")
bin=/tmp/fiber_http10k; rm -f "$bin"
if ! "$STAGE" -o "$bin" "$f" >/dev/null 2>&1; then
  echo "http10k: FAIL (compile)"
  exit 1
fi
SECONDS=0
out=$("$bin" 2>/dev/null); rc=$?
elapsed=$SECONDS
if [ "$rc" = "0" ] && [ "$out" = "$expected" ]; then
  echo "http10k: PASS (10000 connections in ${elapsed}s)"
else
  echo "http10k: FAIL rc=$rc '$out'"
  exit 1
fi
