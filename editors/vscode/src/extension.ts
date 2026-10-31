import * as vscode from "vscode";
import * as cp from "child_process";
import * as fs from "fs";
import * as os from "os";
import * as path from "path";
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
  TransportKind,
} from "vscode-languageclient/node";

let client: LanguageClient | undefined;

// The extension's own directory: the formatters it carries (lldb/scaly.py).
let extensionPath = "";

// Where the scaly.io installer puts a program, for a VS Code that does not see
// the PATH the installer extended: on macOS and Linux the commands in
// ~/.scaly/bin; on Windows the programs themselves in
// %USERPROFILE%\.scaly\toolchain\libexec (install.ps1). A VS Code started by
// its own setup inherits the environment of the browser the setup came from,
// which was running before Scaly was installed.
function installedProgram(name: string): string | undefined {
  const home = path.join(os.homedir(), ".scaly");
  const candidates =
    process.platform === "win32"
      ? [path.join(home, "toolchain", "libexec", name + ".exe"), path.join(home, "bin", name + ".exe")]
      : [path.join(home, "bin", name)];
  return candidates.find(fileIsExecutable);
}

const INSTALL_COMMAND =
  process.platform === "win32"
    ? "'irm https://scaly.io/install.ps1 | iex' in PowerShell"
    : "'curl -fsSL https://scaly.io/install.sh | sh'";

// True if `name` resolves on PATH (cheap: scan PATH for an executable file).
// On Windows a program is found by its name with an extension of PATHEXT
// (scalyls is scalyls.exe), as the system finds it.
function onPath(name: string): boolean {
  if (name.includes("/") || name.includes("\\")) {
    return fileIsExecutable(name);
  }
  const exts =
    process.platform === "win32"
      ? ["", ...(process.env.PATHEXT || ".COM;.EXE;.BAT;.CMD").split(";").filter((e) => e.length > 0)]
      : [""];
  const dirs = (process.env.PATH || "").split(path.delimiter);
  return dirs.some((d) => d && exts.some((e) => fileIsExecutable(path.join(d, name + e))));
}

function fileIsExecutable(p: string): boolean {
  try {
    fs.accessSync(p, fs.constants.X_OK);
    return fs.statSync(p).isFile();
  } catch {
    return false;
  }
}

// Resolve the scalyls binary: explicit setting wins; otherwise prefer `scalyls`
// on PATH (the installer's wrapper, which sets SCALY_HOME itself), then fall
// back to the installer's location (installedProgram#).
function resolveServerPath(): string {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("server.path", "scalyls")
    .trim();
  if (configured.length > 0 && configured !== "scalyls") {
    return configured;
  }
  if (onPath("scalyls")) {
    return "scalyls";
  }
  return installedProgram("scalyls") ?? (configured.length > 0 ? configured : "scalyls");
}

// SCALY_HOME for the server (prelude/stdlib root). Explicit setting wins. Then,
// if a workspace folder is itself a Scaly checkout (has packages/scaly), use it
// — this is the in-repo developer case. Otherwise leave it unset and let the
// installed `scalyls` wrapper set its own SCALY_HOME (=~/.scaly).
function resolveScalyHome(): string {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("home", "")
    .trim();
  if (configured.length > 0) {
    return configured;
  }
  const folders = vscode.workspace.workspaceFolders || [];
  for (const f of folders) {
    if (fs.existsSync(path.join(f.uri.fsPath, "packages", "scaly"))) {
      return f.uri.fsPath;
    }
  }
  return "";
}

function buildClient(): LanguageClient {
  const serverPath = resolveServerPath();

  if (!onPath(serverPath)) {
    void vscode.window.showErrorMessage(
      `Scaly: language server '${serverPath}' not found. Install it with ` +
        `${INSTALL_COMMAND}, or set 'scaly.server.path'.`
    );
  }

  const env = { ...process.env };
  const scalyHome = resolveScalyHome();
  if (scalyHome.length > 0) {
    env.SCALY_HOME = scalyHome;
  }

  const serverOptions: ServerOptions = {
    command: serverPath,
    transport: TransportKind.stdio,
    options: { env },
  };

  const clientOptions: LanguageClientOptions = {
    documentSelector: [{ scheme: "file", language: "scaly" }],
  };

  return new LanguageClient(
    "scaly",
    "Scaly Language Server",
    serverOptions,
    clientOptions
  );
}

async function startClient(): Promise<void> {
  client = buildClient();
  await client.start();
}

async function stopClient(): Promise<void> {
  if (client) {
    await client.stop();
    client = undefined;
  }
}

// ---------------------------------------------------------------- debugging
//
// Scaly programs are debugged through lldb-dap, the DAP server that ships with
// LLVM and with Xcode — so there is no adapter to write and nothing extra to
// install. The compiler's `-g` emits the line tables, the variables and the
// container types; this side only has to find the adapter and load the data
// formatters.

// Where lldb-dap tends to live, in the order we prefer it. The configured
// setting wins over all of them.
function lldbDapCandidates(): string[] {
  return [
    "lldb-dap",
    "lldb-dap-21",
    "/opt/homebrew/opt/llvm@21/bin/lldb-dap",
    "/usr/lib/llvm-21/bin/lldb-dap",
    // the Command Line Tools, which the scaly.io installer fetches on a Mac
    // without them: no shim in /usr/bin, so not on PATH
    "/Library/Developer/CommandLineTools/usr/bin/lldb-dap",
    "/Applications/Xcode.app/Contents/Developer/usr/bin/lldb-dap",
  ];
}

// macOS: whichever developer directory is selected (xcode-select) knows it
function lldbDapFromXcrun(): string | undefined {
  if (process.platform !== "darwin") {
    return undefined;
  }
  try {
    const found = cp
      .execFileSync("xcrun", ["--find", "lldb-dap"], { encoding: "utf8", stdio: ["ignore", "pipe", "ignore"] })
      .trim();
    return found.length > 0 && fileIsExecutable(found) ? found : undefined;
  } catch {
    return undefined;
  }
}

function resolveDebugAdapter(): string | undefined {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("debugAdapter.path");
  if (configured && configured.length > 0) {
    return configured;
  }
  return lldbDapCandidates().find((c) => onPath(c)) ?? lldbDapFromXcrun();
}

// The formatters turn a String from a raw pointer into text and give
// Vector/Array/List their elements as children. They live in the Scaly repo, so
// resolve them the way the language server's SCALY_HOME is resolved.
function resolveFormatterScript(): string | undefined {
  const cfg = vscode.workspace.getConfiguration("scaly");
  const configured = cfg.get<string>("formatters.path");
  if (configured && configured.length > 0) {
    return fs.existsSync(configured) ? configured : undefined;
  }
  const roots = [
    cfg.get<string>("home"),
    process.env.SCALY_HOME,
    vscode.workspace.workspaceFolders?.[0]?.uri.fsPath,
  ];
  for (const root of roots) {
    if (!root) {
      continue;
    }
    const p = path.join(root, "tools", "lldb", "scaly.py");
    if (fs.existsSync(p)) {
      return p;
    }
  }
  // the copy the extension carries: an installation has no tools/
  const bundled = path.join(extensionPath, "lldb", "scaly.py");
  return fs.existsSync(bundled) ? bundled : undefined;
}

class ScalyDebugAdapterFactory
  implements vscode.DebugAdapterDescriptorFactory
{
  createDebugAdapterDescriptor(): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
    const adapter = resolveDebugAdapter();
    if (!adapter) {
      void vscode.window.showErrorMessage(
        "Scaly: lldb-dap not found. Install LLVM 21 (or Xcode), or set scaly.debugAdapter.path."
      );
      return undefined;
    }
    return new vscode.DebugAdapterExecutable(adapter, []);
  }
}

class ScalyDebugConfigurationProvider
  implements vscode.DebugConfigurationProvider
{
  resolveDebugConfiguration(
    _folder: vscode.WorkspaceFolder | undefined,
    config: vscode.DebugConfiguration
  ): vscode.ProviderResult<vscode.DebugConfiguration> {
    // F5 without a launch.json: debug the Scaly file in the active editor
    if (!config.type && !config.request && !config.name) {
      const file = activeScalyFile();
      if (!file) {
        void vscode.window.showInformationMessage("Scaly: open a .scaly file to debug it.");
        return undefined;
      }
      config.type = "scaly";
      config.request = "launch";
      config.name = "Debug the open Scaly file";
      config.source = file;
    }
    if (config.cwd === undefined) {
      config.cwd = config.source !== undefined ? "${fileDirname}" : "${workspaceFolder}";
    }
    // Windows: the Visual Studio debugger, not lldb-dap (visualStudioLaunch#)
    if (process.platform === "win32") {
      return visualStudioLaunch(config);
    }
    // An EXPLICIT initCommands is honoured as given, including an empty array —
    // that is how a user opts out of the formatters.
    if (config.initCommands === undefined) {
      const script = resolveFormatterScript();
      if (script) {
        config.initCommands = [`command script import ${script}`];
      }
    }
    return config;
  }

  // A configuration that names a `source` builds it first, with debug
  // information, into `program` (beside the source unless given); a failed
  // build stops the launch, its errors in the Problems panel.
  resolveDebugConfigurationWithSubstitutedVariables(
    folder: vscode.WorkspaceFolder | undefined,
    config: vscode.DebugConfiguration
  ): Promise<vscode.DebugConfiguration | undefined> {
    return buildBeforeLaunch(folder, config, "source");
  }
}

async function buildBeforeLaunch(
  folder: vscode.WorkspaceFolder | undefined,
  config: vscode.DebugConfiguration,
  key: string
): Promise<vscode.DebugConfiguration | undefined> {
  if (config.request !== "launch" || typeof config[key] !== "string") {
    return config;
  }
  const source: string = config[key];
  delete config[key];
  if (typeof config.program !== "string" || config.program.length === 0) {
    config.program = outputOf(source);
  }
  const task = scalyTask({ type: "scaly", command: "build", file: source }, source, folder, config.program);
  const exit = await runToEnd(task);
  if (exit !== 0) {
    void vscode.window.showErrorMessage(
      `Scaly: building ${path.basename(source)} failed — the Problems panel has the errors.`
    );
    return undefined;
  }
  return config;
}

// ---------------------------------------------------------------- Windows
//
// On Windows `-g` writes CodeView into a .pdb beside the program (the Build
// Tools' link.exe), and the debugger that reads it is Visual Studio's, which
// VS Code has through Microsoft's C/C++ extension as the debug type
// `cppvsdbg`. A Scaly configuration is turned into one of those before it is
// resolved further: VS Code then asks the providers of the NEW type, and a
// provider of this extension for `cppvsdbg` builds the source in the second
// phase (VS Code does not change the type between the phases, so the build
// must be registered under the type the launch has by then). The data views
// for String and the containers need no setting: the link puts the standard
// library's scaly/win32/scaly.natvis into the .pdb.

const CPPTOOLS = "ms-vscode.cpptools";

function visualStudioLaunch(config: vscode.DebugConfiguration): vscode.DebugConfiguration | undefined {
  if (!vscode.extensions.getExtension(CPPTOOLS)) {
    void vscode.window
      .showErrorMessage(
        "Scaly: on Windows, Scaly programs are debugged with the Visual Studio debugger, which comes with Microsoft's C/C++ extension.",
        "Install C/C++"
      )
      .then((choice) => {
        if (choice) {
          void vscode.commands.executeCommand("workbench.extensions.installExtension", CPPTOOLS);
        }
      });
    return undefined;
  }
  const out: vscode.DebugConfiguration = {
    type: "cppvsdbg",
    request: config.request,
    name: config.name,
  };
  if (config.request === "attach") {
    out.processId = config.pid !== undefined ? String(config.pid) : "${command:pickProcess}";
  } else {
    if (typeof config.source === "string") {
      out.scalySource = config.source;
    }
    if (config.program !== undefined) {
      out.program = config.program;
    }
    out.args = config.args ?? [];
    out.cwd = config.cwd;
    out.stopAtEntry = config.stopOnEntry === true;
    if (config.env && typeof config.env === "object") {
      out.environment = Object.entries(config.env).map(([name, value]) => ({ name, value: String(value) }));
    }
  }
  return out;
}

class ScalyVisualStudioBuild implements vscode.DebugConfigurationProvider {
  resolveDebugConfigurationWithSubstitutedVariables(
    folder: vscode.WorkspaceFolder | undefined,
    config: vscode.DebugConfiguration
  ): Promise<vscode.DebugConfiguration | undefined> {
    return buildBeforeLaunch(folder, config, "scalySource");
  }
}

// ---------------------------------------------------------------- tasks
//
// The tasks a Scaly file offers without a tasks.json -- run, build (with debug
// information) and test --, for the file in the active editor or the one a
// tasks.json entry names. Terminal > Configure Task writes one into
// tasks.json to change it.

interface ScalyTaskDefinition extends vscode.TaskDefinition {
  command: "run" | "build" | "test";
  file?: string;
}

function activeScalyFile(): string | undefined {
  const doc = vscode.window.activeTextEditor?.document;
  if (!doc || doc.uri.scheme !== "file" || !doc.fileName.endsWith(".scaly")) {
    return undefined;
  }
  return doc.fileName;
}

// the program a build of `file` writes: beside it, without the extension
function outputOf(file: string): string {
  const base = path.join(path.dirname(file), path.basename(file, ".scaly"));
  return process.platform === "win32" ? base + ".exe" : base;
}

// Resolve the tool the way the compiler is resolved: setting, PATH, the
// default install location.
function resolveToolPath(): string {
  const configured = vscode.workspace.getConfiguration("scaly").get<string>("tool.path", "scaly").trim();
  if (configured.length > 0 && configured !== "scaly") {
    return configured;
  }
  if (onPath("scaly")) {
    return "scaly";
  }
  return installedProgram("scaly") ?? "scaly";
}

function scalyTask(
  def: ScalyTaskDefinition,
  file: string,
  scope: vscode.WorkspaceFolder | vscode.TaskScope | undefined,
  output?: string
): vscode.Task {
  let args: string[];
  switch (def.command) {
    case "build":
      args = ["build", "-g", file, "-o", output ?? outputOf(file)];
      break;
    case "test":
      args = ["test", file];
      break;
    default:
      args = ["run", file];
  }
  const task = new vscode.Task(
    def,
    scope ?? vscode.TaskScope.Workspace,
    def.command,
    "scaly",
    new vscode.ShellExecution(resolveToolPath(), args, { cwd: path.dirname(file) }),
    ["$scaly"]
  );
  task.detail = `scaly ${args.map((a) => (a === file ? path.basename(file) : a === output ? path.basename(a) : a)).join(" ")}`;
  if (def.command === "build") {
    task.group = vscode.TaskGroup.Build;
  } else if (def.command === "test") {
    task.group = vscode.TaskGroup.Test;
  }
  return task;
}

// a task's file: the one its definition names, else the active editor's
function fileOf(def: ScalyTaskDefinition, scope: vscode.WorkspaceFolder | vscode.TaskScope | undefined): string | undefined {
  if (!def.file || def.file === "${file}") {
    return activeScalyFile();
  }
  if (path.isAbsolute(def.file)) {
    return def.file;
  }
  const base = typeof scope === "object" ? scope.uri.fsPath : vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
  return base ? path.join(base, def.file) : undefined;
}

class ScalyTaskProvider implements vscode.TaskProvider {
  provideTasks(): vscode.Task[] {
    const file = activeScalyFile();
    if (!file) {
      return [];
    }
    const folder = vscode.workspace.getWorkspaceFolder(vscode.Uri.file(file));
    return (["run", "build", "test"] as const).map((command) =>
      scalyTask({ type: "scaly", command }, file, folder)
    );
  }

  resolveTask(task: vscode.Task): vscode.Task | undefined {
    const def = task.definition as ScalyTaskDefinition;
    const file = fileOf(def, task.scope);
    if (!file) {
      void vscode.window.showInformationMessage("Scaly: open a .scaly file to run this task on.");
      return undefined;
    }
    const resolved = scalyTask(def, file, task.scope);
    // keep the name and the group a tasks.json entry gave it
    return new vscode.Task(def, task.scope ?? vscode.TaskScope.Workspace, task.name, "scaly", resolved.execution, task.problemMatchers.length > 0 ? task.problemMatchers : ["$scaly"]);
  }
}

// run a task and answer its exit code
function runToEnd(task: vscode.Task): Promise<number | undefined> {
  return new Promise((resolve) => {
    let execution: vscode.TaskExecution | undefined;
    const ended: vscode.TaskProcessEndEvent[] = [];
    const listener = vscode.tasks.onDidEndTaskProcess((e) => {
      if (execution === undefined) {
        ended.push(e);
      } else if (e.execution === execution) {
        listener.dispose();
        resolve(e.exitCode);
      }
    });
    vscode.tasks.executeTask(task).then(
      (started) => {
        execution = started;
        const early = ended.find((e) => e.execution === started);
        if (early) {
          listener.dispose();
          resolve(early.exitCode);
        }
      },
      () => {
        listener.dispose();
        resolve(undefined);
      }
    );
  });
}

// ---------------------------------------------------------------- code lenses
//
// The server emits lenses whose command is one of the two below. They are
// registered here and DELIBERATELY not contributed in package.json: a lens
// invokes them programmatically with a path argument, and a palette entry
// would offer the user a command that cannot work without one.
//
// The server cannot name a built-in instead. `vscode.open` takes a Uri OBJECT
// and rejects the string a JSON-RPC argument can carry, so opening a file from
// a lens needs this thin conversion either way.

async function openPath(target: unknown): Promise<void> {
  if (typeof target !== "string" || target.length === 0) {
    return;
  }
  const doc = await vscode.workspace.openTextDocument(vscode.Uri.file(target));
  await vscode.window.showTextDocument(doc, { preview: false });
}

// Resolve the compiler the way the server is resolved: explicit setting, then
// PATH (the installer's wrapper, which sets SCALY_HOME itself), then the
// default install location.
function resolveCompilerPath(): string {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("compiler.path", "scalyc")
    .trim();
  if (configured.length > 0 && configured !== "scalyc") {
    return configured;
  }
  if (onPath("scalyc")) {
    return "scalyc";
  }
  return installedProgram("scalyc") ?? "scalyc";
}

// One reused terminal, so running a file repeatedly does not pile up panels.
let runTerminal: vscode.Terminal | undefined;

function shellQuote(s: string): string {
  return `'` + s.replace(/'/g, `'\\''`) + `'`;
}

function runFile(target: unknown): void {
  if (typeof target !== "string" || target.length === 0) {
    return;
  }
  if (!runTerminal || runTerminal.exitStatus !== undefined) {
    runTerminal = vscode.window.createTerminal({ name: "Scaly" });
  }
  runTerminal.show(true);
  // --jit runs the program in the compiler's own process; nothing is written to
  // disk, which is what makes this safe to offer on any program file.
  runTerminal.sendText(
    `${shellQuote(resolveCompilerPath())} --jit ${shellQuote(target)}`
  );
}

export function activate(context: vscode.ExtensionContext): void {
  extensionPath = context.extensionPath;
  void startClient();

  context.subscriptions.push(
    vscode.commands.registerCommand("scaly.openPath", openPath),
    vscode.commands.registerCommand("scaly.runFile", runFile),
    vscode.commands.registerCommand("scaly.restartServer", async () => {
      await stopClient();
      await startClient();
      void vscode.window.showInformationMessage("Scaly language server restarted.");
    }),
    vscode.debug.registerDebugAdapterDescriptorFactory(
      "scaly",
      new ScalyDebugAdapterFactory()
    ),
    vscode.debug.registerDebugConfigurationProvider(
      "scaly",
      new ScalyDebugConfigurationProvider()
    ),
    vscode.debug.registerDebugConfigurationProvider(
      "cppvsdbg",
      new ScalyVisualStudioBuild()
    ),
    vscode.tasks.registerTaskProvider("scaly", new ScalyTaskProvider())
  );
}

export function deactivate(): Thenable<void> | undefined {
  if (!client) {
    return undefined;
  }
  return client.stop();
}
