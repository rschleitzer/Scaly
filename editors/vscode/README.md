# Scaly for VS Code

VS Code language support for [Scaly](https://scaly.io), backed by
[scalyls](../../packages/scalyls/0.1.0) — the Scaly language server, itself
written in Scaly. The extension registers the `.scaly` language, provides
TextMate syntax highlighting, launches the server over stdio, and ships a
debugger backed by `lldb-dap`.

What you get:

- **Syntax highlighting** — TextMate grammar for keywords, literals, comments,
  operators (plus richer, server-computed **semantic tokens** on top).
- **Diagnostics** — parse / model / plan errors as you type, in the editor and
  the Problems panel. A package member is analysed through its package ROOT, so
  the errors of every module of that root are reported at once: a mistake in a
  file you do not have open still appears in the Problems panel, at its own
  line. It costs nothing extra — that analysis was already running.
- **Hover** — signatures for functions, types, and bindings, plus the `;`
  comment block written above the declaration, its line structure intact.
- **Go to Definition** / **Find All References** / **Document Highlight** —
  cross-file and cross-package.
- **Completion** — names in scope, members, keywords.
- **Rename** — shadow-aware, with prepare support (F2).
- **Signature Help** — parameter hints while typing a call.
- **Document symbols / outline** — functions, structs/unions (with methods and
  variants), namespaces, module-level mutables; **workspace symbols** across
  the project (Cmd+T).
- **Inlay hints** — inferred types inline, plus **where each construction is
  allocated**: `stack`, `region` (the function's own region), `region #` (the
  caller's page, so it outlives the call) or `region ^name`. Lifetimes are
  inferred in Scaly — the `$` sigil is gone and `#`/`^` survive only where a page
  is genuinely pinned — so this is the only place the allocation decision is
  visible. It is the emitter's own rule, checked against emitted IR in the test
  suite. These hints describe the file **as saved**: they come from a plan the
  compiler builds by reading from disk, so they disappear while a buffer has
  unsaved changes and come back on save (which is also what keeps a full plan off
  every scroll — the answer is cached per saved file).
- **Code lenses** — five families, all Scaly-specific:
  - **Run** — `Run (scalyc --jit)` at the top of any file that has top-level
    statements, i.e. a program rather than a library. Runs it in a terminal
    through the in-process JIT; nothing is written to disk.
  - **Generated file** — `generated from codegen/parser-scaly.scm - edit the
    generator, then ./mkp` at the top of every file `./mkp` writes, with a click
    that opens the generator. The list mirrors `mkp` itself.
  - **Package / module resolution** — the file a `package NAME VERSION` or
    `module NAME` declaration resolves to, clickable — and, the useful half,
    `missing module file: …` when it resolves to nothing (the compiler
    substitutes an empty module for that, silently).
  - **Definitions emitted** — above a generic `function`/`procedure`, how many
    definitions the compiler emits for it (one per distinct mangled name, so a
    generic shows what it costs in copies) and which concrete types it was
    instantiated with; above a routine the demand-driven planner never reached,
    `not emitted in this root` — and for a whole file that the package root does
    not include as a module, that fact once, at the top.
  - **The self-scaling verdict** — above every `for` loop: whether the compiler
    drives it through the adaptive parallel engine (`runs in parallel`) or runs
    it sequentially, and in that case the first construct that blocked it
    (`runs sequentially: writes loop-external t`). Scaly has no parallel syntax
    and emits no diagnostic either way, so this is the only place the decision
    is visible outside `scalyc --task-plan`. These verdicts describe the file
    **as last saved** — a loop's classification needs its whole package planned,
    which is read from disk — so while an unsaved edit adds or removes a `for`,
    they disappear rather than sit on the wrong lines.

  Turn all of them off per language with
  `"[scaly]": { "editor.codeLens": false }`.
- **Folding ranges**.
- **Debugging** — gutter breakpoints, stepping, call stack, and a Variables
  view with arguments, `let` bindings and readable containers. See below.

## Install (users)

1. Install the Scaly toolchain — this puts both `scalyc` and the `scalyls`
   language server on your `PATH`:

   ```bash
   curl -fsSL https://scaly.io/install.sh | sh
   ```

2. Install the extension from the packaged `.vsix`:

   ```bash
   curl -fsSLO https://scaly.io/downloads/scaly-vscode.vsix
   code --install-extension scaly-vscode.vsix
   ```

   (Or in VS Code: Extensions view > `···` menu > "Install from VSIX…".)

3. Open any folder and create a `.scaly` file. The extension finds `scalyls`
   on `PATH`; the installer's wrapper sets `SCALY_HOME` itself, so the server
   resolves the prelude and standard library from `~/.scaly` no matter where
   your sources live.

## Settings

| Setting | Default | Meaning |
|---------|---------|---------|
| `scaly.server.path` | `scalyls` | Path to the `scalyls` binary. The default resolves it on `PATH` (the scaly.io installer's wrapper); falls back to `~/.scaly/bin/scalyls`. |
| `scaly.compiler.path` | `scalyc` | Path to `scalyc`, used by the **Run** code lens. Resolved like the server: setting, then `PATH`, then `~/.scaly/bin/scalyc`. |
| `scaly.home` | `""` | `SCALY_HOME` for the server (prelude/packages search root). If empty and a workspace folder is itself a Scaly checkout (contains `packages/scaly`), that folder is used; otherwise the server wrapper's own `SCALY_HOME` applies. |

## Restart the server

Command Palette > **"Scaly: Restart Language Server"** (`scaly.restartServer`),
e.g. after rebuilding `scalyls`.

## Development (working on the extension or the server)

Build the language server from a repo checkout (`tools/install.sh` installs a
`scalyls` wrapper alongside `scalyc`; or build by hand):

```bash
./build.sh                      # builds scalyc + scalyls from the seed
tools/install.sh                # puts scalyc/scalyls wrappers on PATH
```

Build and run the extension from source:

```bash
cd editors/vscode
npm install
npm run compile
```

Then open `editors/vscode` in VS Code and press **F5** ("Run Scaly
Extension") — an Extension Development Host window opens with the extension
loaded. Open the Scaly repo root as the folder in that window so `SCALY_HOME`
resolves to the checkout, and open any `.scaly` file.

Package a `.vsix` for distribution:

```bash
npm run package                 # runs vsce package
```

## Debugging

The extension contributes a `scaly` debug type backed by **`lldb-dap`**, the DAP
server that ships with LLVM and with Xcode — so there is no separate debugger
extension to install and nothing to build. Compile with `-g` and press F5:

```bash
scalyc -g -o build/program src/main.scaly
```

`.vscode/launch.json` (the extension offers this as the initial configuration,
and as a "Scaly: Launch" snippet):

```json
{
  "type": "scaly",
  "request": "launch",
  "name": "Debug Scaly program",
  "program": "${workspaceFolder}/build/program",
  "cwd": "${workspaceFolder}"
}
```

**Without `-g` there are no line tables, so breakpoints cannot bind** — that is
the first thing to check when a breakpoint stays hollow.

What the Variables view shows:

| | |
|---|---|
| Function **arguments** | by name and type, including by-value structs |
| **`let`** bindings | including a program's top-level statements |
| `var` bindings | as before |
| `String` | as text — `name = "probe"` |
| `Vector` / `Array` / `List` | element count plus the elements as children |
| tagged unions | `tag = Chosen` — the variant NAME, not a number |

The last two rows come from two different places, which is worth knowing when
something looks wrong. The union tag is in the **DWARF itself** (its tag is an
enumeration over the variants), so it works in any debugger. `String`, `Vector`,
`Array` and `List` need the **data formatters** in
[`tools/lldb/scaly.py`](../../tools/lldb/scaly.py), which the extension loads for
you: `resolveDebugConfiguration` injects a
`command script import …/tools/lldb/scaly.py` into `initCommands` when your
launch config does not set that key itself. Set `"initCommands": []` to opt out.

Settings:

| Setting | Default | Meaning |
|---|---|---|
| `scaly.debugAdapter.path` | *(auto)* | Path to `lldb-dap`. Auto-detection tries `PATH`, then the LLVM 20 prefix, then Xcode's copy. |
| `scaly.formatters.path` | *(auto)* | Path to `tools/lldb/scaly.py`. Derived from `scaly.home`, then `SCALY_HOME`, then the first workspace folder. |

Outside the editor the same thing works from a terminal, which is often the
faster loop:

```bash
lldb -o "command script import <scaly>/tools/lldb/scaly.py" \
     -o "b main.scaly:42" -o run -o "frame variable" ./build/program
```

The whole path is gated by [`tests/debuginfo/run.sh`](../../tests/debuginfo),
which drives `lldb-dap` over the protocol with the same launch attributes and the
same `initCommands` this extension sends — so a wrong key here fails in CI rather
than in your editor.
