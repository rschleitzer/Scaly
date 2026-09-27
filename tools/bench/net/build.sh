#!/bin/bash
# tools/bench/net/build.sh [scalyc] — build the HTTP comparison into
# BENCH_WORK/net (default /tmp/scaly-bench/net): the Scaly server
# (hello.scaly, -O2 -mcpu=native), the Go servers (goroutine per connection;
# net/http), the Rust/tokio server (cargo, fetches tokio from crates.io) and
# the load generator. A side that fails to build is reported; race.sh skips it.
cd "$(dirname "$0")/../../.." || exit 1
. tests/platform.sh || exit 1
X=$SCALY_EXE
SCALYC=${1:-scalyc/build/scalyc$X}
W=${BENCH_WORK:-/tmp/scaly-bench}/net
mkdir -p "$W"
ok() { echo "ok   $1"; }
bad() { echo "FAIL $1: $(grep -m2 -i error "$2")"; }

if "$SCALYC" -O2 -mcpu=native -o "$W/hello$X" tools/bench/net/hello.scaly > "$W/hello.log" 2>&1; then ok scaly; else bad scaly "$W/hello.log"; fi
for d in go-raw go-http load; do
  if (cd tools/bench/net/$d && go build -o "$W/$d$X" .) > "$W/$d.log" 2>&1; then ok "$d"; else bad "$d" "$W/$d.log"; fi
done
if command -v cargo >/dev/null 2>&1; then
  if (cd tools/bench/net/rust-tokio && CARGO_TARGET_DIR="$W/cargo" cargo build --release) > "$W/rust-tokio.log" 2>&1; then
    cp "$W/cargo/release/server$X" "$W/rust-tokio$X"; ok rust-tokio
  else
    bad rust-tokio "$W/rust-tokio.log"
  fi
else
  echo "skip rust-tokio: no cargo"
fi
