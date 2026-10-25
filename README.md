Scaly
=====

[_Scaly_](https://scaly.io) is a compiled programming language whose memory
is managed in regions the compiler infers: no garbage collector, no borrow
checker, and no lifetime annotations in ordinary code. It compiles through
LLVM to native code, checks that functions are pure, and runs a `for` loop in
parallel when its body allows it — with no parallel syntax at all.

The compiler is written in Scaly and builds itself; so are its language
server and the tool around it.

Install
-------

On macOS and Linux:

```sh
curl -fsSL https://scaly.io/install.sh | sh
```

On Windows (PowerShell):

```powershell
irm https://scaly.io/install.ps1 | iex
```

The installer puts three ready-made programs under `~/.scaly` — no `sudo`, no
LLVM to install: `scaly` (the tool: REPL, `run`, `build`, `test`, `new`,
`install`), `scalyc` (the compiler) and `scalyls` (the language server). It
needs a C compiler for linking (clang; on Windows the Build Tools for Visual
Studio, which it offers to install). Ready-made programs exist for macOS 26
on Apple silicon, macOS 15 on Intel, Ubuntu 26.04 on x86_64 and arm64, and
Windows 10/11 on x86_64 and arm64; elsewhere the installer builds them from
source. More on [scaly.io/download](https://scaly.io/download/).

First steps
-----------

```sh
printf 'print("Hello, World!")\n' > hello.scaly
scaly run hello.scaly                  # compiled and run in process
scaly build hello.scaly -o hello       # a native program
scaly new myapp                        # a project with a test
scaly                                  # the REPL
```

A taste
-------

```scaly
define Shape union
(
    Rect: (w: f64, h: f64)
    Triangle: (base: f64, height: f64)
)

function area(s: Shape) returns f64
    choose s
        when r: Rect: r.w * r.h
        when t: Triangle: t.base * t.height / 2.0

function total(shapes: Vector[Shape]) returns f64
{
    var sum 0.0
    for s in shapes                     ; a reduction: runs on every core
        sum := sum + area(s)
    sum
}

var shapes Vector[Shape](3)
shapes.put(0, Shape.Rect((w: 2.5, h: 4.0)))
shapes.put(1, Shape.Triangle((base: 3.0, height: 1.5)))
shapes.put(2, Shape.Rect((w: 0.5, h: 0.5)))
print "total area: `total(shapes)`"    ; total area: 12.5
```

No semicolons, no `return` for the last value, and no annotations about where
memory lives: the compiler decides whether a value goes on the stack, on the
calling function's region or on a page of its own, and an escape checker
proves that no reference outlives its memory.

An HTTP server
--------------

The standard packages include an HTTP/1.1 and HTTP/2 server; this one answers
on port 8080 on every core:

```scaly
package http 0.2.0

use http.Request.Request
use http.Response.Response
use http.Router.Router

procedure hello(req: ref[Request], mutable res: ref[Response])
    res.send_text("Hello, World!")

procedure greet(req: ref[Request], mutable res: ref[Response])
{
    let name req.param("name")
    let text "Hello, `name`!"
    res.send_text(text.as_slice())
}

var app Router.create(Page.allocate_page())
app.get("/", &hello)
app.get("/hello/{name}", &greet)
serve(app, 8080)
```

```sh
scaly build server.scaly --release -o server && ./server
```

Each request is handled in a region of its own, freed when the handler
returns. The packages beside the standard library: `http`, `https` (TLS 1.3),
`h3` (HTTP/3), `tls`, `json`, `compress`, and clients for PostgreSQL, Redis,
SQLite and SQL Server.

Built with Scaly
----------------

- **[tscaly](https://github.com/rschleitzer/tscaly)** — the TypeScript
  compiler (typescript-go), ported from Go. On all of VS Code it reports the
  same 359 errors as tsgo, in 4.9 s where tsgo takes 6.9 s; its 326 000 lines
  build in 38 s.
- **[fire](https://github.com/rschleitzer/fire)** — a FHIR R5 server generated
  from HL7's definitions, a drop-in for its C# counterpart on the same
  database: 1.4 to 1.9 times fewer instructions for a read, 35 to 49 MB of
  memory where the C# server needs 450 to 470.
- **[dazzle](https://github.com/rschleitzer/dazzle)** — OpenSP and OpenJade,
  the SGML parser and DSSSL engine, ported from C++, with a PDF backend of its
  own; both run in a web page as WebAssembly.
- **The compiler, `scalyls` and the tool** — every line of the toolchain.

Tools
-----

- A [VS Code extension](https://scaly.io/downloads/scaly-vscode.vsix) on the
  language server: diagnostics as you type, hover, go to definition,
  references, rename, completion.
- Source-level debugging through DWARF with lldb.
- Packages are directories with a version in their path, fetched from git by
  the tool (`package json 0.1.3 "github.com/someone/json"`); no manifest, no
  lock file, no registry.
- `--target wasm32-wasip1` for WebAssembly.

Self-scaling
------------

One program scales across execution substrates without being rewritten: the
same `for` loop and the same region model run from a single core to a
cluster. A `for` whose body is provably independent is dispatched through an
adaptive parallel driver, and a memory region is also the unit of
distribution, shipped between nodes as a self-describing image. The ladder —
fibers, cores, GPU, cluster — is complete;
[`demo/mann_dist.scaly`](demo/mann_dist.scaly) trains a small transformer by
data-parallel SGD across processes that exchange their gradient regions over
TCP.

Learn Scaly
-----------

The [tutorial and language specification](https://scaly.io/lang/) cover the
language end to end, including the VS Code debugging setup; its examples are
executed by the test suite.

Building from source
--------------------

A source checkout builds the toolchain from the committed seed, the compiler
as LLVM IR. It needs **LLVM 21** with its headers (Homebrew's `llvm@21`,
Ubuntu's `llvm-21-dev lld-21 clang`) and a C compiler:

```sh
tools/build-from-seed.sh        # -> scalyc/build/scalyc, scaly, scalyls
tools/install.sh                # put it on your PATH (tools/uninstall.sh to remove)
```

Version 21 is a hard requirement, not a floor: the compiler links against
LLVM's C API, which renames functions between major versions. See
[RELEASING.md](RELEASING.md) for how the seed is made, verified and
published.

Issues
------

Bug reports and questions are welcome as
[issues](https://github.com/rschleitzer/Scaly/issues). Scaly is developed by
one person; for a change larger than a fix, please open an issue first. A
security problem goes privately, as [SECURITY.md](SECURITY.md) says.

Repository layout
-----------------

- **scaly** — runtime support and standard library (`packages/scaly/`).
- **scalyc** — the self-hosted compiler and the tool (`packages/scalyc/`).
- **scalyls** — the language server (`packages/scalyls/`).
- the standard packages beside them (`packages/http/`, `packages/json/`, …).
- **scaly.io** — the website and the documentation (`docs/`).

Licensing
---------

Scaly is MIT (see [LICENSE](LICENSE)).

Two ports written in Scaly are repositories of their own and carry the license
of the work they were derived from. The SGML parser and the DSSSL engine —
ports of James Clark's SP and of OpenJade, under their permissive license —
are [github.com/rschleitzer/dazzle](https://github.com/rschleitzer/dazzle);
this tree's generated sources are produced with that engine.

The TypeScript port, tscaly, is Apache 2.0 and a repository of its own,
[github.com/rschleitzer/tscaly](https://github.com/rschleitzer/tscaly). One
rule follows from that and is easy to break by accident: **no code moves out
of tscaly into the compiler, the runtime or the standard library.** Concepts
and ideas yes, literal code no — a helper lifted over while porting would
relicense a piece of the MIT stdlib to Apache 2.0 without anyone deciding to.
