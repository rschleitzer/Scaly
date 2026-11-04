namespace Scaly.VisualStudio.Workspace;

using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Workspace;
using Microsoft.VisualStudio.Workspace.Build;
using Microsoft.VisualStudio.Workspace.Debug;
using Microsoft.VisualStudio.Workspace.Extensions.VS;
using Microsoft.VisualStudio.Workspace.Indexing;
using Task = System.Threading.Tasks.Task;

/// <summary>
/// Builds one .scaly file the way the VS Code extension's build task does:
/// <c>scaly build -g file.scaly -o file.exe</c>, the program and its .pdb
/// beside the source. Every line goes to the Build pane of the Output window,
/// every <c>file:line:col: error: text</c> also to the Error List.
/// </summary>
internal static class ScalyBuild
{
    public const string Extension = ".scaly";

    private static readonly Regex Diagnostic = new(@"^(.*?):(\d+):(\d+):\s+(error|warning):\s+(.*)$");

    public static bool IsScaly(string path) => path.EndsWith(Extension, StringComparison.OrdinalIgnoreCase);

    public static string ProgramOf(string source) => Path.ChangeExtension(source, ".exe");

    public static async Task<bool> BuildAsync(IWorkspace workspace, string source, CancellationToken cancellationToken)
    {
        var messages = workspace.GetBuildMessageService();

        // Reporting is asynchronous and the compiler's lines arrive on reader
        // threads: chain the reports so they keep their order, and wait for the
        // chain before the build counts as done.
        var gate = new object();
        Task reported = Task.CompletedTask;
        void Report(string text, BuildMessage.TaskType type = BuildMessage.TaskType.None, string? file = null, int line = 0, int column = 0, string? task = null)
        {
            if (messages is null)
            {
                return;
            }
            var message = new BuildMessage
            {
                Type = type,
                LogMessage = text + "\n",
                TaskText = task,
                File = file,
                ProjectFile = source,
                LineNumber = line,
                ColumnNumber = column,
            };
            lock (gate)
            {
                reported = reported.ContinueWith(
                    _ => messages.ReportBuildMessages(new[] { message }),
                    CancellationToken.None,
                    TaskContinuationOptions.None,
                    TaskScheduler.Default).Unwrap();
            }
        }
#pragma warning disable VSTHRD003 // The chain is started here, on the thread pool, never on the UI thread.
        Task AllReportedAsync()
        {
            lock (gate)
            {
                return reported;
            }
        }
#pragma warning restore VSTHRD003

        ScalyPrograms.SettingsError = null;
        var tool = await ScalyPrograms.FindAsync(ScalySettings.ToolPath, "scaly", cancellationToken);
        if (tool is null)
        {
            var text = $"scaly was not found. Install Scaly with '{ScalyPrograms.InstallCommand}' in PowerShell, or set its path under Tools > Options > Scaly.";
            Report(text, BuildMessage.TaskType.Error, task: text);
            await AllReportedAsync();
            return false;
        }

        var directory = Path.GetDirectoryName(source)!;
        var output = ProgramOf(source);
        var info = new ProcessStartInfo(tool, $"build -g \"{source}\" -o \"{output}\"")
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true,
            WorkingDirectory = directory,
        };
        var home = await ScalyPrograms.HomeAsync(workspace.Location, cancellationToken);
        if (home is not null)
        {
            info.EnvironmentVariables["SCALY_HOME"] = home;
        }
        if (ScalyPrograms.SettingsError is { } settingsError)
        {
            Report("Warning: " + settingsError + "; the defaults apply.");
        }
        Report($"{tool} build -g {Path.GetFileName(source)} -o {Path.GetFileName(output)}");
        if (home is not null)
        {
            Report($"SCALY_HOME={home}");
        }

        using var process = new Process { StartInfo = info, EnableRaisingEvents = true };
        var exited = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
        void OnLine(object sender, DataReceivedEventArgs e)
        {
            if (e.Data is null)
            {
                return;
            }
            var m = Diagnostic.Match(e.Data);
            if (!m.Success)
            {
                Report(e.Data);
                return;
            }
            var file = m.Groups[1].Value;
            if (!Path.IsPathRooted(file))
            {
                file = Path.Combine(directory, file);
            }
            Report(
                e.Data,
                m.Groups[4].Value == "error" ? BuildMessage.TaskType.Error : BuildMessage.TaskType.Warning,
                file,
                int.Parse(m.Groups[2].Value),
                int.Parse(m.Groups[3].Value),
                m.Groups[5].Value);
        }
        process.OutputDataReceived += OnLine;
        process.ErrorDataReceived += OnLine;
        process.Exited += (_, _) => exited.TrySetResult(process.ExitCode);
        process.Start();
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();

        using (cancellationToken.Register(() =>
        {
            try
            {
                process.Kill();
            }
            catch (InvalidOperationException)
            {
                // Already exited.
            }
            exited.TrySetCanceled();
        }))
        {
            var rc = await exited.Task.ConfigureAwait(false);
            process.WaitForExit(); // let the asynchronous readers drain
            Report(rc == 0 ? "Build succeeded." : $"Build failed (exit {rc}).");
            await AllReportedAsync();
            return rc == 0;
        }
    }
}

/// <summary>
/// Indexes every .scaly file as buildable (no configuration) and as a debug
/// target, so it is offered in Solution Explorer and as a startup item.
/// </summary>
[ExportFileScanner(
    "B0C6D0E5-6A0F-4E61-9B3D-2C1E7F4A8D21", "Scaly",
    new[] { ScalyBuild.Extension },
    new[] { typeof(IReadOnlyCollection<FileDataValue>) },
    ProviderPriority.Normal)]
internal sealed class ScalyFileScannerFactory : IWorkspaceProviderFactory<IFileScanner>
{
    public IFileScanner CreateProvider(IWorkspace workspaceContext) => new Scanner();

    private sealed class Scanner : IFileScanner
    {
        public Task<T> ScanContentAsync<T>(string filePath, CancellationToken cancellationToken)
            where T : class
        {
            if (typeof(T) != FileScannerTypeConstants.FileDataValuesType)
            {
                throw new NotImplementedException();
            }

            var values = new List<FileDataValue>
            {
                new(BuildConfigurationContext.ContextTypeGuid, BuildConfigurationContext.DataValueName, value: null),
                new(DebugLaunchActionContext.ContextTypeGuid, DebugLaunchActionContext.IsDefaultStartupProjectEntry, value: null,
                    target: ScalyBuild.ProgramOf(filePath)),
            };
            return Task.FromResult((T)(IReadOnlyCollection<FileDataValue>)values);
        }
    }
}

/// <summary>The build and rebuild contexts of a .scaly file.</summary>
[ExportFileContextProvider(
    ProviderTypeString, ProviderPriority.Normal,
    new[] { typeof(string) },
    BuildContextTypes.BuildContextType, BuildContextTypes.RebuildContextType)]
internal sealed class ScalyBuildContextProviderFactory : IWorkspaceProviderFactory<IFileContextProvider>
{
    private const string ProviderTypeString = "4E2B9A61-3C7D-4F08-A1E5-6D9B2C8F0A37";
    private static readonly Guid ProviderType = new(ProviderTypeString);

    public IFileContextProvider CreateProvider(IWorkspace workspaceContext) => new Provider();

    private sealed class Provider : IFileContextProvider
    {
        public Task<IReadOnlyCollection<FileContext>> GetContextsForFileAsync(string filePath, CancellationToken cancellationToken)
        {
            if (!ScalyBuild.IsScaly(filePath))
            {
                return Task.FromResult<IReadOnlyCollection<FileContext>>(Array.Empty<FileContext>());
            }

            var inputs = new[] { filePath };
            return Task.FromResult<IReadOnlyCollection<FileContext>>(new[]
            {
                new FileContext(ProviderType, BuildContextTypes.BuildContextTypeGuid, new BuildConfigurationContext(null), inputs),
                new FileContext(ProviderType, BuildContextTypes.RebuildContextTypeGuid, new BuildConfigurationContext(null), inputs),
            });
        }
    }
}

/// <summary>Build and Rebuild on a .scaly file (Solution Explorer, Ctrl+B).</summary>
[ExportFileContextActionProvider(
    (FileContextActionProviderOptions)VsCommandActionProviderOptions.SupportVsCommands,
    "8A1F3C5E-9B2D-4E7A-B6C0-1D4F8E2A5B93", ProviderPriority.Normal,
    BuildContextTypes.BuildContextType, BuildContextTypes.RebuildContextType)]
internal sealed class ScalyBuildActionProviderFactory : IWorkspaceProviderFactory<IFileContextActionProvider>, IVsCommandActionProvider
{
    // The command group and identifiers the Open Folder build commands use.
    private static readonly Guid BuildCommandGroup = new("16537f6e-cb14-44da-b087-d1387ce3bf57");
    private const uint BuildCommandId = 0x1000;
    private const uint RebuildCommandId = 0x1010;

    public IFileContextActionProvider CreateProvider(IWorkspace workspaceContext) => new Provider(workspaceContext);

    public IReadOnlyCollection<CommandID> GetSupportedVsCommands() => new[]
    {
        new CommandID(BuildCommandGroup, (int)BuildCommandId),
        new CommandID(BuildCommandGroup, (int)RebuildCommandId),
    };

    private sealed class Provider : IFileContextActionProvider
    {
        private readonly IWorkspace workspace;

        public Provider(IWorkspace workspace) => this.workspace = workspace;

        public Task<IReadOnlyList<IFileContextAction>> GetActionsAsync(string filePath, FileContext fileContext, CancellationToken cancellationToken)
        {
            // A Scaly build is always whole: rebuild is build.
            uint id = fileContext.ContextType == BuildContextTypes.RebuildContextTypeGuid ? RebuildCommandId : BuildCommandId;
            return Task.FromResult<IReadOnlyList<IFileContextAction>>(new IFileContextAction[]
            {
                new BuildAction(this.workspace, filePath, fileContext, id),
            });
        }
    }

    private sealed class BuildAction : IFileContextAction, IVsCommandItem
    {
        private readonly IWorkspace workspace;
        private readonly string filePath;

        public BuildAction(IWorkspace workspace, string filePath, FileContext source, uint commandId)
        {
            this.workspace = workspace;
            this.filePath = filePath;
            this.Source = source;
            this.CommandId = commandId;
        }

        public Guid CommandGroup => BuildCommandGroup;

        public uint CommandId { get; }

        public string DisplayName => "Build " + Path.GetFileName(this.filePath);

        public FileContext Source { get; }

        public async Task<IFileContextActionResult> ExecuteAsync(IProgress<IFileContextActionProgressUpdate> progress, CancellationToken cancellationToken)
            => new FileContextActionResult(await ScalyBuild.BuildAsync(this.workspace, this.filePath, cancellationToken));
    }
}
