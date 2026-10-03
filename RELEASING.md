# Releasing Scaly — the `.ll` seed distribution

Scaly ships as its own emitted LLVM IR. A **seed** is the self-hosted compiler
expressed as three textual `.ll` files — `main.ll` + `scalyc.ll` + `scaly.ll` —
plus the recipe to turn them back into a working `scalyc`. Textual IR is
diffable, auditable, and re-buildable with nothing but LLVM, so we distribute
that instead of opaque per-platform binaries.

This document is the per-release checklist: mint a seed on each target, verify
its fixed point, and publish the artifacts.

---

## 1. What a seed is (and is not)

- **The seed = the three `.ll` files.** They are the canonical, shippable
  artifact. `scalyc_seed` (a built binary) is reproducible from them and is
  *not* the deliverable.
- A seed is produced and verified by `tools/seed.sh`, which emits the trio with
  `--no-tests`, links it (no `-Wl,-undefined,dynamic_lookup` — a clean link
  proves zero undefined symbols), runs `hello` + the AOT corpus, and checks the
  **fixed point**: the seed re-emits all three `.ll` byte-identical to itself.

### One seed for all LP64 little-endian targets

The emitted `.ll` carries **no `target triple`**; it does carry a `target
datalayout` line (stamped from the TargetMachine — removing it miscompiles
DL-dependent sizeof-GEPs under `opt`), and that line is the one part of the seed
that changes with the LLVM major. The compiler computes every size and alignment
from **fixed LP64 constants** at
emission (union payload sizes, `align 8`, `ptrtoint`/`mul i64` sizeof
constexprs) — it never consults the host `DataLayout`. So the IR is
host-independent: a mint on any LP64-LE host emits the same `.ll`, `llc`
retargets it, and the compiler built from it reads its own triple via
`LLVMGetDefaultTargetTriple` at runtime. **A single seed therefore serves every
LP64 little-endian target** — the "fabulous four" below — and the repo commits
exactly one, under `seed/`, rebuilt with `tools/build-from-seed.sh`.

Not covered: LLP64 (Windows x64), 32-bit, and big-endian targets — those need
the size/align emission generalized off its hardcoded 64-bit constants.

**Verification is still per target.** Portable *emission* is proven; portable
*execution* is not automatic, because the compiler bakes some ABI choices
(struct-by-value / `sret`) into the IR that meet `libLLVM`/`libc` at a platform
ABI boundary. Each target is built from the seed and **verified** in two tiers:

- **Functional (every push, the three CI-reachable targets, free CI).** Build
  with `tools/build-from-seed.sh`, then `tools/verify-seed.sh`: hello + the AOT
  corpus (self-checked against each test's `; Expected:`). This proves the seed
  builds a *correct* compiler on the target. Automated in
  `.github/workflows/verify-seed.yml`.
- **Byte-identical fixed point (release time, high-RAM machine).**
  `VERIFY_FIXEDPOINT=1 tools/verify-seed.sh` additionally re-emits all three `.ll`
  and `cmp`s them against `seed/`. Re-emitting `scalyc.ll` peaks at **~15 GB**
  (main ~1 GB, scaly ~5.6 GB), so it does not fit free runners — run it on a box
  with enough RAM. This is the strongest proof of the single-seed model.

Current status: functionally verified in CI on `arm64-apple-darwin`,
`x86_64-linux-gnu`, and `aarch64-linux-gnu` (plus fixed-point on
`arm64-apple-darwin`). **`x86_64-apple-darwin` is best-effort, covered by
inference** — it is not in the CI matrix (GitHub's Intel macOS runners are being
retired). Its two halves are each verified above: the x86_64 System-V ABI via
`x86_64-linux-gnu`, and Mach-O/darwin object emission + linking via
`arm64-apple-darwin`; combined with the host-independent seed, the residual risk
is minimal. To verify it explicitly, run `tools/build-from-seed.sh` +
`tools/verify-seed.sh` on an Intel Mac.

(The C++ stage-0 stays frozen-but-buildable: it refreshes the seed, brings up
genuinely new data models, and is an independent lineage for diverse double
compiling.)

### The "fabulous four" supported targets

| Triple | Platform | Status |
|---|---|---|
| `arm64-apple-darwin`  | Apple Silicon macOS | verified in CI |
| `x86_64-linux-gnu`    | x86-64 Linux        | verified in CI |
| `aarch64-linux-gnu`   | arm64 Linux         | verified in CI |
| `x86_64-apple-darwin` | Intel macOS         | best-effort (covered by inference; no CI runner) |

All four are LP64 little-endian. 32-bit / big-endian targets are **not**
supported until the s179 union-sizing walker and the align/sizeof emission are
generalized off their hardcoded 64-bit constants.

### Pinned toolchain

- **LLVM 20.** IR text compatibility across LLVM major versions is not
  guaranteed; pin every seed to LLVM 20.
- **`libLLVM` is the binding half, not `llc`.** libLLVM prints the IR, so its
  major decides emission; measured across the 18→20 move, the whole seed diff
  was the `target datalayout` line plus `getelementptr inbounds` gaining `nuw`.
  `llc` only translates the seed TEXT and may be a different major (stage 7's
  Windows rung 12 builds with llc 20 against libLLVM 18). The final link is
  plain object linking, so any `clang`/`cc` works.

---

## 2. Mint the seed (once, on any LP64-LE host)

Emission is host-independent, so the seed is minted once on any LP64-LE machine
(per-target *verification* is section 5). Install the dependencies, clone the
repo at the release tag, then run one command. `tools/seed.sh` with no arguments
bootstraps a fresh stage-2 if needed; `tools/install-seed.sh` then copies the
result into the committed `seed/`.

| Target | Install dependencies |
|---|---|
| macOS (arm64 or Intel) | `brew install llvm@20` |
| Linux (x86-64 or arm64) | `apt install llvm-20-dev clang-20 zlib1g-dev libzstd-dev` |

Neither `cmake` nor `openjade` appears here any more: cmake went with the
retired C++ stage-0, and openjade became optional with stage 8 — `./mkp` drives
the DSSSL codegen with our own engine and skips it entirely when none is
available, since the generated files are committed.

```sh
git checkout v<version>          # the release tag
tools/seed.sh                    # bootstrap stage-2, emit + verify the seed
```

A successful run ends with:

```
SEED: OK — links clean, runs hello + AOT, reproduces itself byte-identical
```

and leaves the artifacts in `dist/seed/` (gitignored). If LLVM 20 lives in a
non-standard location, prefix with `LLVM20=/path/to/llvm-20`.

**Note:** if `build.sh` complains about a cmake generator mismatch (only happens
when re-running on an existing tree, never a fresh clone), `rm -rf scalyc/build`
and re-run. The `openjade: non SGML character` lines are benign warnings.

---

## 3. Package the artifact (one tarball for all LP64-LE targets)

Bundle the three `.ll` files plus a checksum manifest. One artifact covers every
LP64-LE target, so name the tarball by the release version only.

```sh
VERSION=<version>                                   # e.g. 0.1.0
DEST="scaly-seed-$VERSION"

mkdir -p "$DEST"
cp seed/main.ll seed/scalyc.ll seed/scaly.ll "$DEST/"
( cd "$DEST" && shasum -a 256 *.ll > SHA256SUMS )   # Linux: sha256sum
tar czf "$DEST.tar.gz" "$DEST"
```

The tarball is ~10–11 MB (mostly `scalyc.ll`). Record the LLVM version used
(`llc --version | head -1`) in the release notes — it is the seed's contract.

---

## 4. Rebuild + verify from a tarball (what a consumer does)

A consumer needs only LLVM 20 and a C compiler — no Scaly source:

```sh
shasum -a 256 -c SHA256SUMS                         # integrity
LLC=$(command -v llc-20 || echo "$(brew --prefix llvm@20)/bin/llc")
LIB="$(dirname "$(dirname "$LLC")")/lib"

for f in main scalyc scaly; do "$LLC" -relocation-model=pic -filetype=obj "$f.ll" -o "$f.o"; done
clang main.o scalyc.o scaly.o -L"$LIB" -lLLVM-20 -o scalyc

./scalyc -o hello <path>/tests/aot/hello.scaly && ./hello   # -> Hello, World!
```

**Fixed-point re-check** (optional, needs the matching source tree): re-emit and
compare against the shipped `.ll` — they must be byte-identical.

```sh
./scalyc -S --no-tests -o r_scaly.ll packages/scaly/0.1.0/scaly.scaly
cmp r_scaly.ll scaly.ll && echo "fixed point OK"
```

---

## 5. Publish the scaly.io installer

End users install with `curl -fsSL https://scaly.io/install.sh | sh`
(`docs/website/install.sh`). **We hand out programs with LLVM linked in
statically; the seed is the fallback.** Two kinds of archive lie under
`scaly.io/downloads/`:

- `scaly-<version>.tar.gz` (`tools/make-dist.sh`) — the same for every system:
  `seed/` with its `SHA256SUMS`, the sources of the stdlib and of the standard
  packages (scaly, http, https, tls, json, compress, pg, redis, h3), `LICENSE`.
  No sources of the compiler, the language server or the ports.
- `scaly-<version>-<system>-<machine>.tar.gz` (`tools/make-bindist.sh`, run ON
  that system) — `scalyc`, `scaly`, `scalyls` and `libscaly.a`. The minimum
  system is the one the LLVM libraries were built for: **macOS 26, Ubuntu
  26.04** (build the Linux archive on 26.04, not on something newer).

The installer unpacks the first, takes the second when there is one and its
programs start, and otherwise builds the three programs from the seed (LLVM 21
required then). It installs into `~/.scaly/toolchain.new` and swaps only after
its own check (`scaly run`, and `scaly build` where a C compiler exists), so a
failed run leaves the installed toolchain alone and a second run is the upgrade.

**Before publishing:**

```sh
tools/make-profile.sh         # ONCE, on the fast machine: dist/scalyc.profdata (the compiler trained on its own sources)
tests/install/run.sh          # both archives of THIS tree -> scratch prefix, both routes, 13 checks (with the profile ~2 min)
tools/make-bindist.sh <version>   # on EACH system we hand programs out for; leaves dist/scaly-<version>-<system>-<machine>.tar.gz
```

`make-bindist.sh` builds scalyc and scaly with `dist/scalyc.profdata` when it
is there (or `$SCALY_PGO_PROFILE`), about 30 % faster compilers; without it
the programs are the seed recipe's and the script says so. The profile is the
same file on every system — copy it to the other machines with the tree. Make
it again whenever the compiler's sources changed.

For Linux, in a clean container (`ubuntu:26.04` with `llvm-21 llvm-21-dev
lld-21 clang zlib1g-dev libzstd-dev libxml2-dev`); collect the archives of the
other systems into `dist/` before the next step.

Then, after refreshing `seed/`:

```sh
tools/publish-install.sh <version>      # e.g. 0.1.0
```

This runs `tools/make-dist.sh` and `aws s3 cp`s `docs/website/install.sh`,
that tarball and every program archive found in `dist/` to `s3://scaly.io/`,
then invalidates CloudFront. The archives live under the `/downloads/` prefix,
which `docs/deploy.sh` **excludes** from its whole-bucket `--delete` sync, so a
routine docs deploy never removes them. After publishing, confirm the endpoints
serve `200`:

```sh
curl -fsSL -o /dev/null -w '%{http_code}\n' https://scaly.io/install.sh
curl -fsSL -I https://scaly.io/downloads/scaly-<version>.tar.gz
curl -fsSL -I https://scaly.io/downloads/scaly-<version>-darwin-arm64.tar.gz
```

---

## 6. Release checklist

- [ ] Tag the release (`git tag v<version>`), push the tag.
- [ ] Refresh the single seed once (`tools/seed.sh && tools/install-seed.sh`) on
      any LP64-LE host; commit `seed/`.
- [ ] **Functional verify** on all four targets — the `verify-seed` CI matrix
      (build + hello + AOT corpus) must be green.
- [ ] **Fixed-point verify** on a high-RAM machine — `VERIFY_FIXEDPOINT=1
      tools/verify-seed.sh` re-emits all three `.ll` byte-identical to `seed/`
      (~15 GB peak; once per release is enough since the IR is host-independent).
- [ ] Package the one `.ll` trio + `SHA256SUMS` as `scaly-seed-<version>.tar.gz`
      (section 3) and attach it to the GitHub release.
- [ ] Run the installer's gate (`tests/install/run.sh`) and build the program
      archive on each system (`tools/make-bindist.sh`, section 5).
- [ ] Publish the scaly.io installer (`tools/publish-install.sh <version>`, section
      5) so `curl … | sh` ships the new seed; confirm `https://scaly.io/install.sh`
      and `/downloads/scaly-<version>.tar.gz` serve `200`.
- [ ] In the release notes, state the pinned **LLVM version** and list the four
      triples with their verification confirmation.
- [ ] (Independence) confirm the C++ stage-0 still builds on at least one
      platform — it remains the diverse-double-compiling anchor and the
      bootstrap root for new data models.

---

## 7. Notes

- The canonical seed is **committed** under `seed/` (one trio, ~10 MB, for all
  LP64-LE targets, LLVM-version-specific). `dist/` stays gitignored — it is the
  `tools/seed.sh` work area; `tools/install-seed.sh` promotes a verified mint
  from there into `seed/`.
- The seed lineage is produced entirely by the self-hosted compiler. Keeping the
  C++ stage-0 buildable preserves an independent lineage for trusting-trust /
  diverse double-compiling — do not delete it.
- Tooling reference: `tools/seed.sh` (mint + verify), `tools/install-seed.sh`
  (promote a mint into `seed/`), `tools/build-from-seed.sh` (rebuild scalyc from
  `seed/`, no C++), `tools/bootstrap.sh` (stage-0 → stage1 → stage2),
  `tools/llvm-env.sh` (LLVM-20 detection), `tools/make-dist.sh` (bundle the
  scaly.io installer tarball), `tools/publish-install.sh` (upload `install.sh` +
  tarball to scaly.io).
