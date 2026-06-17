Scaly
=====

[_Scaly_](https://scaly.io) — the _self-scaling programming language_. A
self-hosted compiler with LLVM code generation, region-based memory management,
and a clean, semicolon-free syntax.

Install
-------

Scaly ships as its own LLVM IR and is built into a native binary on your
machine, so you need **LLVM 18** and a C compiler first:

```sh
# macOS
brew install llvm@18
# Ubuntu / Debian
sudo apt install llvm-18 clang-18
```

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
seed (no C++ toolchain needed — just LLVM 18 + a C compiler):

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
