#!/bin/bash
# tools/bench/json/run.sh [iterations] — the json package against the best
# JSON parsers (ROADMAP-http.md stage B): Rust's serde_json (Value, the
# realistic bar), simd-json and sonic-rs, and simdjson (C++, its DOM) and
# yyjson (C) — the SIMD ceiling. One core, the same method on every side
# (bench.scaly, rust/src/main.rs, cpp/bench.cpp): best of five rounds, the
# file's size per second, parse into a DOM and write it back. simdjson and
# yyjson come from the system (brew install simdjson yyjson; apt: libsimdjson-dev
# libyyjson-dev); without them their rows are skipped by name.
#
# The files are serde-rs/json-benchmark's canada.json (numbers), citm_catalog
# (objects, short strings) and twitter (Unicode, escapes), plus one synthetic
# FHIR bundle (Synthea, smart-on-fhir/generated-sample-data), downloaded into
# a cache outside the tree: $JSONBENCH_DATA, default $TMPDIR/scaly-jsonbench.
#
# The Scaly side is one whole-program module (tools/link-lto.sh, opt -O2) for
# the host CPU, the Rust side a fat-LTO release for the host CPU: both as the
# HttpArena image builds its entry.
set -eu
HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../../.." && pwd)"
ITER=${1:-20}
DATA=${JSONBENCH_DATA:-${TMPDIR:-/tmp}/scaly-jsonbench}
BIN="$ROOT/scalyc/build/scalyc"
cd "$ROOT"

mkdir -p "$DATA"
fetch() { [ -s "$DATA/$1" ] || curl -sSfL -o "$DATA/$1" "$2"; }
for f in canada citm_catalog twitter; do
  fetch "$f.json" "https://raw.githubusercontent.com/serde-rs/json-benchmark/master/data/$f.json"
done
fetch fhir_bundle.json "https://raw.githubusercontent.com/smart-on-fhir/generated-sample-data/master/R4/SYNTHEA/Addie_Tremblay_4c875c3c-b4d6-4f6d-aabe-5ddc24892adc.json"

source tools/llvm-env.sh > /dev/null 2>&1
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
# the IR carries no triple (one seed for every target): name the host's
TRIPLE=$(${CLANG:-clang} -print-target-triple)
# -mcpu=native reaches the IR since SIMD phase 6: a SIMD operation takes the
# form this CPU's features allow (pshufb on x86_64), as the Rust side's
# target-cpu=native does
"$BIN" -S -mcpu=native --no-prelude --no-tests -o "$TMP/rt.ll" packages/scaly/0.1.0/scaly.scaly
"$BIN" -S -mcpu=native --no-prelude -o "$TMP/json.ll" packages/json/0.1.0/json.scaly
"$BIN" -S -mcpu=native -o "$TMP/bench.ll" tools/bench/json/bench.scaly
LTO_OPT_FLAGS="-mtriple=$TRIPLE -mcpu=native" LTO_LLC_FLAGS="-mcpu=native" \
  tools/link-lto.sh "$TMP/bench" "$TMP/bench.ll" "$TMP/json.ll" "$TMP/rt.ll"
( cd tools/bench/json/rust && RUSTFLAGS="-C target-cpu=native" cargo build --release -q )
RUST="$ROOT/tools/bench/json/rust/target/release/jsonbench"
CPP=""
INC=(); LIBS=()
for p in simdjson yyjson; do
  if command -v brew > /dev/null 2>&1 && d=$(brew --prefix "$p" 2> /dev/null) && [ -d "$d/include" ]; then
    INC+=(-I"$d/include"); LIBS+=(-L"$d/lib" -Wl,-rpath,"$d/lib")
  fi
done
# the host CPU, spelled per architecture (clang takes -march on x86)
case "$(uname -m)" in x86_64|amd64) NATIVE=-march=native ;; *) NATIVE=-mcpu=native ;; esac
if ${CXX:-clang++} -std=c++17 -O3 $NATIVE "${INC[@]+"${INC[@]}"}" tools/bench/json/cpp/bench.cpp \
     "${LIBS[@]+"${LIBS[@]}"}" -lsimdjson -lyyjson -o "$TMP/cppbench" 2> "$TMP/cpp.log"; then
  CPP="$TMP/cppbench"
else
  echo "run.sh: SKIP simdjson/yyjson (not installed: $(head -1 "$TMP/cpp.log"))"
fi

for f in canada citm_catalog twitter fhir_bundle; do
  echo "== $f.json ($(wc -c < "$DATA/$f.json" | tr -d ' ') bytes, $ITER iterations)"
  "$TMP/bench" "$DATA/$f.json" "$ITER"
  "$RUST" "$DATA/$f.json" "$ITER"
  if [ -n "$CPP" ]; then "$CPP" "$DATA/$f.json" "$ITER"; fi
done
