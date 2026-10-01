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
# The Scaly side is one whole-program module (`scaly build --release`) for
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

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
# -mcpu=native reaches the IR since SIMD phase 6: a SIMD operation takes the
# form this CPU's features allow (pshufb on x86_64), as the Rust side's
# target-cpu=native does; the tool compiles the packages for it too.
# (Until 2026-10-01 three -S emissions and tools/link-lto.sh; measured equal
# on all four files, the binaries the same size to the byte.)
"$BIN" build tools/bench/json/bench.scaly --release -mcpu=native -o "$TMP/bench"
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
  echo "run.sh: SKIP simdjson/yyjson (the C++ side did not build):"
  grep -E "error|fatal" "$TMP/cpp.log" | head -5
fi

# STAGES=1: the json package's parse taken apart as well (stages.scaly)
if [ "${STAGES:-0}" = 1 ]; then
  "$BIN" build tools/bench/json/stages.scaly --release -mcpu=native -o "$TMP/stages"
fi

# JSONBENCH_KEEP=<dir>: the binaries stay there for a profiler afterwards
if [ -n "${JSONBENCH_KEEP:-}" ]; then
  mkdir -p "$JSONBENCH_KEEP"
  cp "$TMP/bench" "$JSONBENCH_KEEP/"
  [ -x "$TMP/stages" ] && cp "$TMP/stages" "$JSONBENCH_KEEP/"
fi

for f in canada citm_catalog twitter fhir_bundle; do
  echo "== $f.json ($(wc -c < "$DATA/$f.json" | tr -d ' ') bytes, $ITER iterations)"
  "$TMP/bench" "$DATA/$f.json" "$ITER"
  "$RUST" "$DATA/$f.json" "$ITER"
  if [ -n "$CPP" ]; then "$CPP" "$DATA/$f.json" "$ITER"; fi
  if [ "${STAGES:-0}" = 1 ]; then
    for m in copy classify take tokens parse; do printf '  scaly-json '; "$TMP/stages" "$DATA/$f.json" "$ITER" $m; done
  fi
done
