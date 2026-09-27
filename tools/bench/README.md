# tools/bench — Scaly against the benchmarks-game C champions

The comparison of 2026-09-27 (ROADMAP-simd.md, phase 4) as a recipe that runs
on any machine that builds Scaly: the programs of `tests/selfscale/` that have
a benchmarks-game counterpart against the fastest C programs of that game.

```bash
tools/bench/fetch.sh              # the game's source archive (+ sse2neon on arm64)
tools/bench/build.sh              # Scaly (generic and -mcpu=native) and C into $BENCH_WORK/bin
tools/bench/race.sh               # all cores, median of 3 interleaved rounds
tools/bench/race.sh --one-core    # SCALY_WORKERS=1 / OMP_NUM_THREADS=1
tools/bench/race.sh --only mandelbrot,spectral --rounds 5
```

`BENCH_WORK` (default `/tmp/scaly-bench`) holds the download, the shims and the
binaries; nothing third-party is kept in the tree. `build.sh` takes the
compiler as its argument (default `scalyc/build/scalyc`) and finds clang
through `tools/llvm-env.sh`, `clang-20` or `clang` (override with `CC`).

## What is compared

| problem | size | Scaly (`tests/selfscale/`) | C (benchmarks game) |
|---|---|---|---|
| mandelbrot | 8000 | `mandelbrot`, `mandelbrot_simd` | `gcc-6` (SSE), `gcc-5` |
| spectral-norm | 10000 | `spectralnorm`, `spectralnorm_simd` | `gcc-5` (SSE), `gcc-4` |
| n-body | 50 000 000 | `nbody` | `gcc-4` (SSE), `gcc-6` |
| fannkuch-redux | 11 | `fannkuch` | `gcc-6` (SSE), `gcc-5` |
| binary-trees | 21 | `binarytrees` | `gcc-5` |

Every Scaly program is built twice: `<name>` for the generic CPU and
`<name>_native` with `-mcpu=native`. On x86-64 that pair is the AVX question
of phase 3 — generic is SSE2 (an `f64x4` is two `xmm` operations), a CPU with
AVX2 does it in one `ymm` operation. On arm64 both are 128-bit NEON and the
pair should time alike.

The C programs are built with `clang -O3` for the host CPU (`-march=native` on
x86-64; `-mcpu=native` plus sse2neon on arm64, which translates their SSE
intrinsics to NEON) and OpenMP through libomp (`brew install libomp` on
macOS). On Darwin two small shims in `shim/` supply what the Linux-written
programs expect: `memalign` and `sched_getaffinity`.

## Per host

- **macOS**: LLVM 20 and libomp from Homebrew (`llvm@20`, `libomp`). On arm64
  the C side goes through sse2neon; `fetch.sh` downloads it.
- **Linux** (x86-64, e.g. Ubuntu 24.04): the platform CI builds on, and the
  one where every C program builds as its authors wrote it — glibc has
  `memalign`, `sched_getaffinity` and pthreads, so no shim is involved. Needs
  LLVM 20 with clang, lld and libomp (apt.llvm.org: `clang-20`, `lld-20`,
  `libomp-20-dev`; CLAUDE-tooling.md has the rest of the list).
- **Windows** (the box of tests/win32/WINDOWS-BOX.md: Git Bash, the standalone
  LLVM 20 install, the MSVC runtime): the scripts take their environment from
  `tests/platform.sh`, binaries carry `.exe`, and a Scaly program links against
  `/tmp/libscaly.lib` (`tools/win-archive.sh`). The C programs get three small
  POSIX headers from `shim-win/` (`unistd.h` for mandelbrot #6's `write`,
  `malloc.h` for spectral-norm #5's 16-byte `memalign`, `sched.h` for
  spectral-norm #4's CPU count) and libomp from the LLVM install. fannkuch #6
  and binary-trees #5 are written on pthreads, which the MSVC runtime lacks —
  `build.sh` says so and `race.sh` runs without them. ★Measure the box before
  the programs: the antimalware scanner can make every process start cost
  seconds (WINDOWS-BOX.md §4b), and that lands in every row. The Windows path
  was written without a Windows machine at hand; its first run is its test.

## Reading the numbers

- The rounds are interleaved — every program once per round — so a machine
  that throttles as it warms up (a fanless laptop, one in the sun) slows all
  programs alike instead of whichever runs last. The table gives the median
  and the range; a wide range says the machine was not steady.
- `--one-core` skips, by name, the three C programs that choose their own
  thread count (spectral-norm #4 from the CPU affinity, fannkuch #6 and
  binary-trees #5 through pthreads): their number would not be a one-core
  number. n-body is sequential in every program.
- The outputs are printed at the end: the Scaly programs of a problem must
  agree with each other and the C programs with each other (the formats
  differ between the two sides; mandelbrot's C image is shown as an md5).

The measured results on arm64 are in ROADMAP-simd.md (phase 4) and in the
memory `benchmarks-game-c-race`.

## net/ — an HTTP server against Go and Rust

```bash
tools/bench/net/build.sh          # Scaly, Go (raw, net/http), Rust/tokio, the load generator
tools/bench/net/race.sh [threads] [seconds]
```

The same "Hello, World!" over HTTP/1.1 keep-alive from four servers, each
reading up to the end of a request header and writing a fixed response:
`hello.scaly` (`TcpListener.serve`, a scheduler per core), `go-raw` (a
goroutine per connection), `go-http` (the standard `net/http`) and
`rust-tokio` (a tokio task per connection). The load generator (`load/`, Go)
keeps 128 connections and pipelines 16 requests per round trip — the shape of
TechEmpower's plaintext test; without pipelining its own cost per request is
higher than any server's and every run measures the client. Each server runs
on one thread, then on half the CPUs, with the client on the other half.

★Socket options are part of the comparison: Go sets `TCP_NODELAY` by default,
Scaly and tokio leave Nagle on — with pipelining that alone halved Go's
throughput (every response its own packet). `go-raw` therefore switches
Nagle back on; `go-http` stays the standard library as shipped.
`NODELAY=1 tools/bench/net/race.sh` runs the Scaly, go-raw and tokio servers
with `TCP_NODELAY` instead (`TcpStream.set_nodelay`, the servers' second
argument `nodelay`).

A connection per request (`load -k=false`, not in `race.sh`) measures the
accept path. The client closes with `SO_LINGER 0`: without it its ~16 000
ephemeral ports sat in TIME_WAIT within seconds and every server showed
thousands of errors. Measured with it (5 server threads, 32 clients, 5 s):
Scaly 14 276, tokio 14 906, Go raw 15 040, Go net/http 15 049 req/s, no
errors — the kernel's connection setup is the limit for all four; Scaly's
5 % behind is where every poller wakes for a connection only one of them
gets (the shared listener), a plausible cause not yet measured.

Measured on arm64 (10 CPUs, M-series) 2026-09-27, requests per second:

| server | 1 thread | 5 threads | server CPU at 5 threads |
|---|---|---|---|
| Scaly | 221 882 | **485 426** | 26.7 s |
| Rust tokio | 221 394 | 459 774 | 28.9 s |
| Go, goroutine per connection | 219 846 | 446 568 | 29.3 s |
| Go net/http | 75 238 | 112 454 | 21.5 s |

With `TCP_NODELAY` (the same day, 6 s per run), every response leaves as its
own packet and all three fall to the same level — the option acts, and none
of the servers is the limit any more:

| server | 1 thread | 5 threads | server CPU at 5 threads |
|---|---|---|---|
| Scaly | 105 829 | **133 747** | 6.9 s |
| Rust tokio | 105 243 | 130 701 | 7.3 s |
| Go, goroutine per connection | 105 000 | 128 120 | 7.8 s |
| Go net/http (always) | 75 456 | 110 461 | 16.2 s |
