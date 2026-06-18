import * as vscode from "vscode";
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
  TransportKind,
} from "vscode-languageclient/node";

let client: LanguageClient | undefined;

function resolveScalyHome(): string {
  const configured = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("home", "")
    .trim();
  if (configured.length > 0) {
    return configured;
  }
  const folders = vscode.workspace.workspaceFolders;
  if (folders && folders.length > 0) {
    return folders[0].uri.fsPath;
  }
  return "";
}

function buildClient(): LanguageClient {
  const serverPath = vscode.workspace
    .getConfiguration("scaly")
    .get<string>("server.path", "/tmp/scalyls");

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
