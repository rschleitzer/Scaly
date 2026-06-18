# Scaly for VS Code

A minimal VS Code language client for [scalyls](../../packages/scalyls/0.1.0), the
Scaly language server (itself written in Scaly). It registers the `.scaly`
language, launches the server over stdio, and surfaces what the server provides:

- **Diagnostics** — parse / model / plan errors, pushed by the server.
- **Document symbols / outline** — the Outline view, breadcrumbs, and
  "Go to Symbol in File" (`Cmd+Shift+O`), showing functions, structs/unions
  (with their methods and variants), namespaces, and module-level mutables.

No hover/completion/definition yet — those are planned server-side work. The
client itself is generic `vscode-languageclient`; it picks up each feature
automatically as the server advertises the capability.

## 1. Build the language server

The client launches a prebuilt `scalyls` binary; it does not build it. From the
repo root:

```bash
cd /Users/r.schleitzer/repos/Scaly

# Build the compiler if it is missing:
#   ./build.sh

# Build the runtime archive if /tmp/libscaly.a is missing:
#   ./scalyc/build/scalyc -c --no-prelude -o /tmp/libscaly.o packages/scaly/0.1.0/scaly.scaly
#   ar rcs /tmp/libscaly.a /tmp/libscaly.o

./scalyc/build/scalyc -o /tmp/scalyls packages/scalyls/0.1.0/main.scaly \
    -L/opt/homebrew/opt/llvm@18/lib -lLLVM-18
```

This produces `/tmp/scalyls`, the default `scaly.server.path`.

## 2. Build the extension

```bash
cd editors/vscode
npm install
npm run compile
```

## 3. Settings

| Setting | Default | Meaning |
|---------|---------|---------|
| `scaly.server.path` | `/tmp/scalyls` | Path to the `scalyls` binary. |
| `scaly.home` | `""` | `SCALY_HOME` for the server. **If empty, the first workspace folder is used.** |

`SCALY_HOME` (or, when unset, the workspace folder) **must** point at a tree that
contains `packages/` — the server loads the scaly + scalyc prelude packages from
`$SCALY_HOME/packages/` to run the diagnostics pipeline. With it unset and no
workspace folder, diagnostics come back empty because the prelude never loads.

The simplest setup: open the Scaly repo root
(`/Users/r.schleitzer/repos/Scaly`) as your workspace folder and leave
`scaly.home` empty.

## 4. Run it (Extension Development Host)

1. Open the `editors/vscode` folder in VS Code.
2. Run `npm install` once (step 2).
3. Press **F5** (or Run → "Run Scaly Extension"). This compiles and opens a new
   **Extension Development Host** window with the extension loaded.
4. In that window, open the Scaly repo as a folder (so `SCALY_HOME` resolves to
   it), then open any `.scaly` file.

### Verify diagnostics

1. Create a file `bad.scaly` containing a call to an undefined function, e.g.:

   ```scaly
   function main() returns int { return frobnicate(3) }
   ```

   A red squiggle appears under the call and a `function not found: frobnicate`
   entry shows in the **Problems** panel.
2. Fix it (e.g. replace `frobnicate(3)` with `3`) and save — the squiggle and the
   Problems entry clear.
3. Close the file — any remaining diagnostics for it clear.
4. Open a clean, valid `.scaly` file — no diagnostics appear.

### Verify the outline

Open any `.scaly` file (e.g. one under `packages/`) and open the **Outline**
view (Explorer sidebar) or press `Cmd+Shift+O`. The tree lists top-level
functions, `define` structs/unions/namespaces (with their methods and union
variants nested underneath), and module-level `mutable` globals. The outline is
read from the file **on disk**, so save the file to refresh it after edits (an
in-memory document store is a planned follow-up alongside incremental sync).

### Restart the server

Command Palette → **"Scaly: Restart Language Server"** (`scaly.restartServer`),
e.g. after rebuilding `/tmp/scalyls`.
