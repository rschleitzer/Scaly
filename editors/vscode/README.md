# Scaly for VS Code

VS Code language support for [Scaly](https://scaly.io), backed by
[scalyls](../../packages/scalyls/0.1.0) — the Scaly language server, itself
written in Scaly. The extension registers the `.scaly` language, provides
TextMate syntax highlighting, launches the server over stdio, and declares
breakpoint support so the CodeLLDB debugger can stop in Scaly sources.

What you get:

- **Syntax highlighting** — TextMate grammar for keywords, literals, comments,
  operators (plus richer, server-computed **semantic tokens** on top).
- **Diagnostics** — parse / model / plan errors as you type, in the editor and
  the Problems panel.
- **Hover** — signatures and doc info for functions, types, and bindings.
- **Go to Definition** / **Find All References** / **Document Highlight** —
  cross-file and cross-package.
- **Completion** — names in scope, members, keywords.
- **Rename** — shadow-aware, with prepare support (F2).
- **Signature Help** — parameter hints while typing a call.
- **Document symbols / outline** — functions, structs/unions (with methods and
  variants), namespaces, module-level mutables; **workspace symbols** across
  the project (Cmd+T).
- **Inlay hints** — inferred types inline.
- **Folding ranges**.
- **Breakpoints in `.scaly` files** — the extension declares the language for
  debugging, so the editor accepts gutter breakpoints without any settings
  workaround (debugging itself is provided by the
  [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb)
  extension and the compiler's `-g` DWARF output; see the tutorial chapter
  "Using Scaly with VS Code" on scaly.io).

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
