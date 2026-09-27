# tools/bench — Scaly against the benchmarks-game C champions

The comparison of 2026-09-27 (ROADMAP-simd.md, phase 4) as a recipe that runs
on any machine that builds Scaly: the programs of `tests/selfscale/` that have
a benchmarks-game counterpart against the fastest C programs of that game.

```bash
tools/bench/fetch.sh              # the game's source archive (+ sse2neon on arm64)
tools/bench/build.sh              # Scaly (generic and -mcpu=native) and C into $BENCH_WORK/bin
tools/bench/race.py               # all cores, median of 3 interleaved rounds
tools/bench/race.py --one-core    # SCALY_WORKERS=1 / OMP_NUM_THREADS=1
tools/bench/race.py --only mandelbrot,spectral --rounds 5
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
