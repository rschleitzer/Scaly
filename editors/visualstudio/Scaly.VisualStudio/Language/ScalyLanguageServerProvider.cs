namespace Scaly.VisualStudio.Language;

using System.Diagnostics;
using System.IO.Pipelines;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Extensibility;
using Microsoft.VisualStudio.Extensibility.Editor;
using Microsoft.VisualStudio.Extensibility.LanguageServer;
using Microsoft.VisualStudio.Extensibility.Shell;
using Microsoft.VisualStudio.Extensibility.VSSdkCompatibility;
using Microsoft.VisualStudio.RpcContracts.LanguageServerProvider;
using Microsoft.VisualStudio.Workspace.VSIntegration.Contracts;
using Nerdbank.Streams;

#pragma warning disable VSEXTPREVIEW_LSP // The language server API is in preview.

/// <summary>
/// Starts scalyls over stdio for every .scaly document: diagnostics, hover,
/// definitions, references, rename, completion, semantic tokens.
/// </summary>
[VisualStudioContribution]
internal sealed class ScalyLanguageServerProvider : LanguageServerProvider
{
    // Shown once a session: the server is asked for again with every document opened.
    private static int missingReported;

    private readonly MefInjection<IVsFolderWorkspaceService> workspaceService;

    public ScalyLanguageServerProvider(
        ExtensionCore container,
        VisualStudioExtensibility extensibility,
        MefInjection<IVsFolderWorkspaceService> workspaceService)
        : base(container, extensibility)
    {
        this.workspaceService = workspaceService;
        ScalyPrograms.Extensibility = extensibility;
    }

    /// <summary>The content type of a .scaly file.</summary>
    [VisualStudioContribution]
    public static DocumentTypeConfiguration ScalyDocumentType => new("scaly")
    {
        FileExtensions = new[] { ".scaly" },
        BaseDocumentType = LanguageServerBaseDocumentType,
    };

    public override LanguageServerProviderConfiguration LanguageServerProviderConfiguration => new(
        "%Scaly.LanguageServer.DisplayName%",
        new[] { DocumentFilter.FromDocumentType(ScalyDocumentType) });

    public override async Task<IDuplexPipe?> CreateServerConnectionAsync(CancellationToken cancellationToken)
    {
        var server = await ScalyPrograms.FindAsync(ScalySettings.ServerPath, "scalyls", cancellationToken);
        if (server is null)
        {
            if (Interlocked.Exchange(ref missingReported, 1) == 0)
            {
                await this.Extensibility.Shell().ShowPromptAsync(
                    "Scaly: the language server scalyls was not found. Install Scaly with "
                    + $"'{ScalyPrograms.InstallCommand}' in PowerShell and start Visual Studio again, "
                    + "or set its path under Tools > Options > Scaly.",
                    PromptOptions.OK,
                    cancellationToken);
            }
            return null;
        }

        var folder = (await this.workspaceService.GetServiceAsync()).CurrentWorkspace?.Location;
        var info = new ProcessStartInfo(server)
        {
            RedirectStandardInput = true,
            RedirectStandardOutput = true,
            UseShellExecute = false,
            CreateNoWindow = true,
        };
        var home = await ScalyPrograms.HomeAsync(folder, cancellationToken);
        if (home is not null)
        {
            info.EnvironmentVariables["SCALY_HOME"] = home;
        }
        if (folder is not null)
        {
            info.WorkingDirectory = folder;
        }

#pragma warning disable CA2000 // Visual Studio stops the server; the process ends with it.
        var process = new Process { StartInfo = info };
#pragma warning restore CA2000
        if (!process.Start())
        {
            return null;
        }

        return new DuplexPipe(
            PipeReader.Create(process.StandardOutput.BaseStream),
            PipeWriter.Create(process.StandardInput.BaseStream));
    }

    public override Task OnServerInitializationResultAsync(
        ServerInitializationResult serverInitializationResult,
        LanguageServerInitializationFailureInfo? initializationFailureInfo,
        CancellationToken cancellationToken)
    {
        if (serverInitializationResult == ServerInitializationResult.Failed)
        {
            // A server that cannot start is not started again with every document.
            this.Enabled = false;
        }
        return base.OnServerInitializationResultAsync(serverInitializationResult, initializationFailureInfo, cancellationToken);
    }
}

#pragma warning restore VSEXTPREVIEW_LSP
