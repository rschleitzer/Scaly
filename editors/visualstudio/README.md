# Scaly for Visual Studio

Visual Studio 2026 support for [Scaly](https://scaly.io), on the Open Folder
model: open a folder of `.scaly` files (File > Open > Folder), no project file.
The extension adds nothing of its own to the language; it wires what exists:

- **Editor**: the content type `scaly` for `*.scaly`, with the Scaly icon in
  Solution Explorer, coloured by the TextMate
  grammar of the VS Code extension (`../vscode/syntaxes/`), with its comments
  and brackets (`../vscode/language-configuration.json`). Both are taken from
  there at build time, so the VS Code extension stays their one source.
- **Language server**: `scalyls` over stdio: diagnostics in the editor and the
  Error List, hover, Go to Definition (F12), Find All References
  (Shift+F12), rename (Ctrl+R, Ctrl+R), Go to All (Ctrl+T), signature help,
  folding, semantic tokens, completion.
- **Build**: Build on a `.scaly` file in Solution Explorer runs
  `scaly build -g file.scaly -o file.exe`; the output goes to the Build pane,
  errors with file and line to the Error List.
- **Debug**: F5 on a `.scaly` file builds it and starts the program under the
  native debugger; breakpoints bind in the `.scaly` source.
- **Test**: the Test Explorer lists the `test*` functions of every `.scaly`
  file of the folder and runs them with `scaly test`.

The programs are found on `PATH`, else where the scaly.io installer puts them
(`%USERPROFILE%\.scaly\toolchain\libexec`); Tools > Options > Scaly sets them
explicitly, and `SCALY_HOME`. The test adapter runs apart from Visual Studio
and does not read those settings.

## Limits

- Visual Studio does not ask a language server for code lenses or the call
  hierarchy, so neither appears, though `scalyls` offers both.
- Completion comes from `scalyls`, which answers only while the file parses:
  on a half-typed line the list is empty. VS Code falls back to word
  completion there; Visual Studio shows nothing.
- Hover on a call shows what the call yields; hover on the definition shows
  the signature.

## Building

Visual Studio 2026 with the workload "Visual Studio extension development".
`dotnet build` does not find the VSSDK targets; `build.sh` uses Visual
Studio's MSBuild:

```sh
./build.sh                 # Scaly.VisualStudio/bin/Debug/net472/scaly-visualstudio.vsix
./build.sh Release
./build.sh --exp           # also deploy into the Experimental instance (close it first),
                           # then: devenv /rootSuffix Exp <folder>
```

From Git Bash, start devenv with `MSYS_NO_PATHCONV=1`, or `/rootSuffix`
becomes a path.

## Layout

- `Scaly.VisualStudio/`: the extension. VisualStudio.Extensibility hosted in
  process (`VssdkCompatibleExtension`), because the grammar is registered by a
  `.pkgdef` and Open Folder build, debug and test are MEF parts of the
  `Microsoft.VisualStudio.Workspace` and Test Explorer APIs; the out-of-process
  model offers neither. One VSIX serves x64 and arm64.
- `Scaly.TestAdapter/`: the test adapter, which runs in the test host and is
  carried in the VSIX as a `UnitTestExtension`.
