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

export function activate(context: vscode.ExtensionContext): void {
  void startClient();

  context.subscriptions.push(
    vscode.commands.registerCommand("scaly.restartServer", async () => {
      await stopClient();
      await startClient();
      void vscode.window.showInformationMessage("Scaly language server restarted.");
    })
  );
}

export function deactivate(): Thenable<void> | undefined {
  if (!client) {
    return undefined;
  }
  return client.stop();
}
