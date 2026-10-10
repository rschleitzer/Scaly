# Releasing Scaly

What goes out, what its numbers mean, and the steps in order.

## What is released

- **The toolchain**: the three programs `scalyc` (the compiler), `scaly` (the
  tool: REPL, `run`, `build`, `test`, `install`, `next`, `publish`) and `scalyls` (the
  language server), as one archive per system with LLVM linked in, and one
  source archive that every system takes: the packages of this tree and the
  **seed** — the three programs as LLVM IR, from which the installer builds
  them where no ready-made archive fits.
- **The packages** of this tree, each under `packages/<name>/<version>/`.
- **The documentation** on scaly.io.

| archive | system |
|---|---|
| `scaly-<v>-darwin-arm64.tar.gz` | macOS 26 and newer, Apple silicon |
| `scaly-<v>-darwin-x86_64.tar.gz` | macOS 15 and newer, Intel |
| `scaly-<v>-linux-x86_64.tar.gz` | Ubuntu 26.04 and newer |
| `scaly-<v>-linux-aarch64.tar.gz` | Ubuntu 26.04 and newer |
| `scaly-<v>-windows-x86_64.zip` | Windows 10 and newer |
| `scaly-<v>-windows-arm64.zip` | Windows 11 on arm64 |
| `scaly-<v>.tar.gz` | every system: packages and seed |

## The numbers

**The toolchain has a version of its own**, the file `VERSION` at the root.
It says what a user of the programs gets. The distribution scripts, the
release workflow and the installer gate read it; the installers ask for none
and take what `downloads/latest` names (`SCALY_VERSION` picks another).

**A published toolchain version does not change.** Its archives are out,
installed and cached under their names. `tools/publish-install.sh` refuses a
version whose archive is already at scaly.io, so every release begins with
raising `VERSION`. (`--overwrite` exists for an upload that went wrong, not
for a change.)

**Each package has its own version**, by the rules `scaly publish` checks:

- A version is a directory. Published is what `packages/<name>/published`
  names — version, commit, git tree of the directory.
- A published version does not change. `tools/published.sh`, a step of the
  bar, refuses an edit to a published directory, uncommitted ones included.
- The number says what changed, compared by declarations: below 1.0 a
  declaration that is gone or reads differently takes the second number,
  anything else the third; from 1.0 on the first and the second.
- The tree holds ONE directory per package. The first change after a publish
  begins the next version:

  ```sh
  scaly next <package> <version>
  ```

  (`tools/next-version.sh` is that with this tree's own tool.) It renames
  the directory and rewrites every tracked file that names it.
  What it cannot see is a path built from a variable or a glob, and an
  expectation in a test that speaks of something else with the same
  spelling: run the bar after it and read the diff of the tests.
- Declarations do not follow. `package scaly 0.1.0` names a minimum, and the
  loader gives it the highest version of that line that is there.

The toolchain's number and the standard library's are different numbers on
purpose: every package declares `package scaly <version>`, the line is part
of a package's identity, and so the library's line moves only when its
declarations break.

## The steps

1. **The bar.** `tools/bar.sh` — bootstrap, seed refresh, every suite, the
   published directories unchanged. Commit what it refreshed (`seed/`).
2. **The packages.** `scaly publish --check` at the root: every published
   version is what its record names, every new version's number says its
   change. `scaly publish` when a package's new version is done — it writes
   the record, commits and pushes. Not every release publishes packages.
3. **The version.** Raise `VERSION`, commit, push.
4. **The installer gate.** `tests/install/run.sh` — archives of this tree
   into a scratch prefix by both routes (ready-made programs, the seed).
   `tests/install/run-windows.sh` on a Windows machine.
5. **The programs.** `gh workflow run release` builds the six archives on
   GitHub's runners from the pushed commit (the version is the `VERSION`
   file's unless one is given). Wait for all six.
6. **The upload.** `tools/publish-install.sh --run <run id>` fetches the
   run's archives, makes the source archive of THIS tree, and uploads: the
   seven files, `SHA256SUMS` and `SHA256SUMS-<version>`, then
   `downloads/latest`, the two installers last. `--dry-run` shows the
   uploads and sends nothing. It says so when the run's commit and this
   tree's differ.
7. **The documentation.** `docs/html.sh` regenerates the pages from
   `docs/scaly/`; commit them; `docs/deploy.sh` puts the website up.

A push proves nothing by itself: no workflow verifies a commit. The bar on a
machine is the gate, and the release workflow only builds.

## The seed

The committed `seed/` is the compiler, the tool and the language server as
textual LLVM IR, with checksums. It is what the repository bootstraps from
(`tools/bootstrap.sh`, `./build.sh`) and what the installer builds from on a
system without a ready-made archive. One seed serves every target.

`tools/seed.sh` emits it with a freshly bootstrapped compiler and verifies
it: a clean link, `hello`, the AOT corpus, and that the compiler built from
the seed emits the seed again byte for byte. The bar runs it and refreshes
`seed/` when the emission moved; a change to the compiler, the standard
library or the language server is not done before that is committed.

LLVM 21 is required to build from the seed (`llc`, `libLLVM`) and any clang
to link.
