Scaly
=====

[_Scaly_](https://scaly.io) — the _self-scaling programming language_. A
self-hosted compiler with LLVM code generation, region-based memory management,
and a clean, semicolon-free syntax.

Self-scaling
------------

_Self-scaling_ means one program scales across execution substrates without
being rewritten for each: the same `for`-loop and the same region-based memory
model run unchanged from a single core to a whole cluster. Parallelism is
compiler inference — a `for` whose body is provably independent is dispatched
through an adaptive parallel driver, and there is no separate parallel syntax —
while a memory region is also the unit of distribution, shipped between nodes as
a self-describing wire image.

That ladder — **fibers → cores → GPU → cluster** — is now complete. The compiler
bootstraps from a committed LLVM-IR seed, reproduces itself byte-identically, and
passes native CI on arm64/x86_64 × darwin/linux. The finale is a real workload,
not a toy: [`demo/mann_dist.scaly`](demo/mann_dist.scaly) trains a 2-layer
transformer on Thomas Mann's _Der Tod in Venedig_ by synchronous data-parallel
SGD across N processes that all-reduce their gradient **regions** over TCP — pure
Scaly, no MPI or NCCL — then generates German text from the trained model. A
single-node baseline produces bit-for-bit identical loss, so distribution is
semantically transparent; a killed worker is detected and the run finishes on the
survivors.

```sh
demo/run.sh                 # stage 5: char transformer on one node
demo/run_mann_dist.sh 4     # stage 7: the same model, data-parallel over 4 nodes
```

See [ROADMAP.md](ROADMAP.md) for the full staged plan and what each milestone
proved.

Install
-------

Scaly ships as its own LLVM IR and is built into a native binary on your
machine, so you need **LLVM 20** and a C compiler first:

```sh
# macOS
brew install llvm@20
# Ubuntu / Debian
sudo apt update && sudo apt install llvm-20 lld-20 clang
```

On Ubuntu, `lld` is the linker that matters: stock GNU ld (BFD) fails to link
against libLLVM-20. The `apt update` is not decoration either — a stale package
list names a revision the mirror has already superseded, and the install then
dies with a wall of `404 Not Found` on the `.deb` URLs rather than with anything
resembling "not found". If your mirror still 404s afterwards, take LLVM's own
packages instead:

```sh
wget -qO- https://apt.llvm.org/llvm.sh | sudo bash -s -- 20
```

Note that a `libLLVM-20.so` already on the machine proves nothing — plenty of
distributions pull the runtime library in as some other package's dependency
while `llc`, `opt` and `llvm-link` (the `llvm-20` package) are absent. Version 20
is a hard requirement, not a floor — an older `llc` cannot even read the seed,
which is written in LLVM 19+ syntax and stops LLVM 18 at the first
`getelementptr inbounds nuw` with a bare `error: expected type`.

Then run the installer — it puts `scalyc` on your `PATH` under `~/.scaly`, no
`sudo`:

```sh
curl -fsSL https://scaly.io/install.sh | sh
```

Open a new terminal, and you are ready:

```sh
printf 'print("Hello, World!")\n' > hello.scaly
scalyc -o hello hello.scaly && ./hello
```

Uninstall with `rm -rf ~/.scaly` (and remove the `PATH` line the installer added
to your shell profile).

Learn Scaly
-----------

The [tutorial and language specification](https://scaly.io/lang/) cover the
language end to end, including the VS Code debugging setup.

Building from source
--------------------

With a source checkout you can build the compiler directly from the committed
seed (no C++ toolchain needed — just LLVM 20 + a C compiler):

```sh
tools/build-from-seed.sh        # -> scalyc/build/scalyc
tools/install.sh                # put it on your PATH (override BINDIR; tools/uninstall.sh to remove)
```

See [RELEASING.md](RELEASING.md) for how the seed is minted, verified, and
published, and [CLAUDE.md](CLAUDE.md) for the compiler architecture.

Repository layout
-----------------

- **scaly** — runtime support and standard library (`packages/scaly/`).
- **scalyc** — the self-hosted compiler (`packages/scalyc/`).
- **scaly.io** — the static website, the home page of Scaly (`docs/website/`).

Licensing
---------

Scaly is MIT (see [LICENSE](LICENSE)) — but licensing here is **per directory**,
not per repository, because two packages are ports of existing works and carry
the license they were derived under:

| path | license | origin |
|---|---|---|
| everything not listed below | MIT | — |
| `packages/opensp/`, `packages/dazzle/` | Clark / OpenJade (permissive) | James Clark's SP and the OpenJade project |
| `packages/tscaly/` | Apache 2.0 (+ `NOTICE.txt`) | [microsoft/typescript-go](https://github.com/microsoft/typescript-go) |

Each such package carries its own `LICENSE` next to its sources; that file
governs the directory it sits in. None of these licenses is copyleft, so they
neither infect one another nor the rest of the tree.

One rule follows from the mixture and is easy to break by accident:
**no code moves out of `packages/tscaly` into the compiler, the runtime or the
standard library.** Concepts and ideas yes, literal code no — a helper lifted
upward while porting would relicense a piece of the MIT stdlib to Apache 2.0
without anyone deciding to. `tools/license-boundary.sh` checks it on every push.
