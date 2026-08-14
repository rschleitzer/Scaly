import * as vscode from "vscode";
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

// Where the scaly.io installer drops the server wrapper.
const INSTALLED_SERVER = path.join(os.homedir(), ".scaly", "bin", "scalyls");

// True if `name` resolves on PATH (cheap: scan PATH for an executable file).
function onPath(name: string): boolean {
  if (name.includes("/")) {
    return fileIsExecutable(name);
  }
  const dirs = (process.env.PATH || "").split(path.delimiter);
  return dirs.some((d) => d && fileIsExecutable(path.join(d, name)));
}

function fileIsExecutable(p: string): boolean {
  try {
    fs.accessSync(p, fs.constants.X_OK);
    return true;
  } catch {
    return false;
  }
}

// Resolve the scalyls binary: explicit setting wins; otherwise prefer `scalyls`
// on PATH (the installer's wrapper, which sets SCALY_HOME itself), then fall
// back to the default install location ~/.scaly/bin/scalyls.
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
  if (fileIsExecutable(INSTALLED_SERVER)) {
    return INSTALLED_SERVER;
  }
  return configured.length > 0 ? configured : "scalyls";
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
        `'curl -fsSL https://scaly.io/install.sh | sh', or set 'scaly.server.path'.`
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
    "lldb-dap-20",
    "/opt/homebrew/opt/llvm@20/bin/lldb-dap",
    "/usr/lib/llvm-20/bin/lldb-dap",
    "/Applications/Xcode.app/Contents/Developer/usr/bin/lldb-dap",
  ];
}

function resolveDebugAdapter(): string | undefined {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("debugAdapter.path");
  if (configured && configured.length > 0) {
    return configured;
  }
  return lldbDapCandidates().find((c) => onPath(c));
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
  return undefined;
}

class ScalyDebugAdapterFactory
  implements vscode.DebugAdapterDescriptorFactory
{
  createDebugAdapterDescriptor(): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
    const adapter = resolveDebugAdapter();
    if (!adapter) {
      void vscode.window.showErrorMessage(
        "Scaly: lldb-dap not found. Install LLVM 20 (or Xcode), or set scaly.debugAdapter.path."
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
    // An EXPLICIT initCommands is honoured as given, including an empty array —
    // that is how a user opts out of the formatters.
    if (config.initCommands === undefined) {
      const script = resolveFormatterScript();
      if (script) {
        config.initCommands = [`command script import ${script}`];
      }
    }
    if (config.cwd === undefined) {
      config.cwd = "${workspaceFolder}";
    }
    return config;
  }
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
  const installed = path.join(os.homedir(), ".scaly", "bin", "scalyc");
  if (fileIsExecutable(installed)) {
    return installed;
  }
  return "scalyc";
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
    )
  );
}

export function deactivate(): Thenable<void> | undefined {
  if (!client) {
    return undefined;
  }
  return client.stop();
}
