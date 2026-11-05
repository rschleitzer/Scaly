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
  native debugger; breakpoints bind in the `.scaly` source. Ctrl+F5 builds it
  and runs it without the debugger.
- **Launch settings**: "Scaly Launch Settings..." on a `.scaly` file (Solution
  Explorer or the editor's context menu) sets the program's arguments, working
  directory and environment variables for F5 and Ctrl+F5. They are kept in
  `.vs\scaly\launch.txt` of the folder, one section per file. An entry for the
  file in `.vs\launch.vs.json`, where one was written, takes precedence for each
  key it sets (`args`, `currentDir`, `env`).
- **Test**: the Test Explorer lists the tests of every `.scaly` file of the
  folder (a function or procedure named `test` or `test_<name>`, taking
  nothing, answering an int) as `scaly test file --list` names them, a test of
  a module file and of a namespace (`Suite.test_inside`) included, and runs
  them with `scaly test`; a double-click goes to the declaration. A file that
  another loads with `module` is no entry of its own: its tests appear under
  the program, each under its folder and file (`prog` › `modules\helpers` ›
  `inner.scaly` › `test_inner`). A test that
  traps fails with the trap's message, and the tests after it in its file are
  shown as not run. Where a file does not compile, its tests are read from the
  source until it does.

The programs are found on `PATH`, else where the scaly.io installer puts them
(`%USERPROFILE%\.scaly\toolchain\libexec`); Tools > Options > Scaly sets them
explicitly, and `SCALY_HOME`; they are read once a `.scaly` file has been
opened in the editor. The Target setting builds for another triple
(`--target`): `x86_64-pc-windows-msvc` on an Arm PC gives an x64 program that
Windows runs, and Visual Studio debugs, under emulation. The Build pane names
the tool, the target and `SCALY_HOME` of every build. The test adapter runs
apart from Visual Studio, in the test host; the extension hands it the same
tool and `SCALY_HOME` through the run settings (`<ScalyAdapter>`).

## Installing

Download [scaly-visualstudio.vsix](https://scaly.io/downloads/scaly-visualstudio.vsix),
close Visual Studio and double-click the file; the VSIX Installer does the
rest. The tutorial's chapter "Using Scaly with Visual Studio" walks through
it, from installing Visual Studio to the Test Explorer.

## Limits

- Visual Studio does not ask a language server for code lenses or the call
  hierarchy, so neither appears, though `scalyls` offers both.
- Completion comes from `scalyls`, which answers from the last analysis that
  succeeded, so a half-typed line still completes. A file that has not parsed
  once since it was opened gets an empty list.
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

`publish.sh` builds the Release VSIX and uploads it as
`scaly.io/downloads/scaly-visualstudio.vsix` (the aws CLI, signed in);
`publish.sh <file.vsix>` uploads one built on another machine,
`--dry-run` says what it would upload.

## Layout

- `Scaly.VisualStudio/`: the extension. VisualStudio.Extensibility hosted in
  process (`VssdkCompatibleExtension`), because the grammar is registered by a
  `.pkgdef` and Open Folder build, debug and test are MEF parts of the
  `Microsoft.VisualStudio.Workspace` and Test Explorer APIs; the out-of-process
  model offers neither. One VSIX serves x64 and arm64.
- `Scaly.TestAdapter/`: the test adapter, which runs in the test host and is
  carried in the VSIX as a `UnitTestExtension`.
