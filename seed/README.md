# The Scaly seed

These `.ll` files **are** the Scaly compiler — the self-hosted compiler
emitted as its own LLVM IR:

- `main.ll` — entry point
- `scalyc.ll` — the compiler (lexer → parser → modeler → planner → emitter)
- `scaly.ll` — the standard library / runtime
- `scalyls.ll` + `scalyls_main.ll` — the language server, a **separate
  program** with its own two roots. It depends on the scalyc + scaly packages,
  whose bodies come from the compiler seed objects above, so it links as
  `scalyls_main.o + scalyls.o + scalyc.o + scaly.o`. It is emitted
  **self-hosted** (by the seed compiler) but is **not** part of the compiler
  fixed point. `install.sh` and `tools/build-from-seed.sh` link it into
  `<prefix>/bin/scalyls` / `scalyc/build/scalyls`.
- `SHA256SUMS` — integrity manifest

## One seed for all 64-bit little-endian targets

The IR carries **no `target triple` and no `datalayout`**, and the compiler
computes every size/alignment from fixed LP64 constants (never the host's
`DataLayout`). So a single seed serves every LP64 little-endian target — the
"fabulous four" being `arm64-apple-darwin`, `x86_64-apple-darwin`,
`x86_64-linux-gnu`, and `aarch64-linux-gnu`. `llc` retargets the IR, and the
built compiler reads its own host triple at runtime.

Not covered: LLP64 (e.g. Windows x64), 32-bit, and big-endian targets — those
would need the size/align emission generalized off its hardcoded 64-bit
constants.

## Rebuilding the compiler from the seed

With LLVM 18 and a C compiler, from the repository root:

```sh
tools/build-from-seed.sh        # -> scalyc/build/scalyc, no C++ toolchain
```

## Refreshing the seed

The seed is minted by `tools/seed.sh` (which bootstraps a stage-2 compiler
from the committed seed, emits the compiler trio with `--no-tests` and
verifies the fixed point, then emits the two scalyls roots self-hosted and
link-checks + LSP-smoke-tests them), then installed here with
`tools/install-seed.sh`. Because emission is host-independent, you may
mint on any LP64-LE host — but **verify on each target** (run hello + the AOT
corpus + a fixed-point re-emit) before trusting it there. See `RELEASING.md`.

## Verification status

CI (`.github/workflows/seed.yml`) builds from this seed and runs
`tools/verify-seed.sh` (hello + AOT corpus) on every push — since 2026-08-04 on
ONE target, because this repo is private and GitHub bills macOS minutes at 10×
the Linux rate:

- ✅ `x86_64-linux-gnu` — verified in CI on every push (incl. byte-identical
  fixed point)
- ✅ `arm64-apple-darwin` — verified by the full local bar on the dev box before
  every push (was a CI leg until 2026-08-04)
- 🔹 `aarch64-linux-gnu` — best-effort (was a CI leg until 2026-08-04). Its two
  halves are each verified above: the AArch64 codegen via `arm64-apple-darwin`,
  the ELF/glibc side via `x86_64-linux-gnu`.
- 🔹 `x86_64-apple-darwin` — best-effort, never had a runner. Its halves: the
  x86_64 System-V codegen via `x86_64-linux-gnu`, Mach-O/darwin via
  `arm64-apple-darwin`.

Object EMISSION for all four is checked in CI by `tests/target/run.sh`
(`--target` cross-emit). To verify a best-effort target explicitly, run
`tools/build-from-seed.sh` + `tools/verify-seed.sh` on a host of that target —
a container is enough for `aarch64-linux-gnu` on an arm64 Mac.
