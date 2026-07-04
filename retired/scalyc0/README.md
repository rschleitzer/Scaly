# scalyc0 — the retired C++ stage-0 compiler

Here lies the C++ bootstrap compiler (2025 – 2026-07-04). It carried the
project from the first LLVM-emitting lexer through full self-hosting: every
pass (Lexer → Parser → Modeler → Planner → Emitter) was first written here,
then ported to Scaly, until the self-hosted compiler reached its fixed point
(s177) and finally shipped as the textual `.ll` seed (`seed/`), which is the
sole bootstrap root today.

At the time of burial it could no longer parse the living source (no page
sigils, `repeat`, lambdas, labeled loops) and had no remaining build wiring:
`build.sh` builds from the seed, `tools/bootstrap.sh`/`tools/cycle.sh` have
no C++ fallback, and `codegen/rules.scm` no longer emits `Parser.cpp` /
`Syntax.h` / `SyntaxDump.h` (the generated copies here are the last ones).

The sources remain frozen as a **porting reference**: CLAUDE.md's
load-bearing notes cite `Planner.cpp` / `Emitter.cpp` line numbers, and
mirroring stage-0 behavior is still the proven method when closing planner
or emitter gaps in the self-hosted compiler.

It was a faithful crutch, and nobody will miss writing it.
