namespace Scaly.VisualStudio.Workspace;

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Workspace;
using Microsoft.VisualStudio.Workspace.Debug;
using Microsoft.VisualStudio.Workspace.Extensions.VS.Debug;
using Task = System.Threading.Tasks.Task;

/// <summary>
/// F5 on a .scaly file: build it (so the program is never stale), then start
/// it under Visual Studio's native debugger. The program carries CodeView and
/// a .pdb, so breakpoints in the .scaly source bind.
/// </summary>
[ExportVsDebugLaunchTarget("2D7E4B19-5F3A-4C86-9E0B-7A1C3D5F8E64", new[] { ScalyBuild.Extension }, ProviderPriority.Highest)]
internal sealed class ScalyDebugLaunchTargetProvider : IVsDebugLaunchTargetProvider, IVsDebugLaunchTargetProvider4
{
    public void SetupDebugTargetInfo(ref VsDebugTargetInfo vsDebugTargetInfo, DebugLaunchActionContext debugLaunchContext)
    {
        // Not used: LaunchDebugTargetAsync takes the whole launch.
    }

    public async Task LaunchDebugTargetAsync(
        IWorkspace workspaceContext,
        IServiceProvider serviceProvider,
        VsDebugTargetInfo4 vsDebugTargetInfo,
        DebugLaunchActionContext debugLaunchContext,
        CancellationToken cancellationToken)
    {
        var settings = debugLaunchContext.LaunchConfiguration;
        var source = Get(settings, LaunchConfigurationConstants.ProjectKey) ?? Get(settings, LaunchConfigurationConstants.ProgramKey);
        if (source is null || !ScalyBuild.IsScaly(source))
        {
            return;
        }
        if (!Path.IsPathRooted(source))
        {
            source = Path.Combine(workspaceContext.Location, source);
        }

        if (!await ScalyBuild.BuildAsync(workspaceContext, source, cancellationToken))
        {
            return;
        }

        await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
        if (serviceProvider.GetService(typeof(SVsShellDebugger)) is not IVsDebugger4 debugger)
        {
            return;
        }
        var target = new VsDebugTargetInfo4
        {
            dlo = (uint)DEBUG_LAUNCH_OPERATION.DLO_CreateProcess,
            bstrExe = ScalyBuild.ProgramOf(source),
            bstrCurDir = Get(settings, LaunchConfigurationConstants.CurrentDirKey) ?? Path.GetDirectoryName(source),
            bstrArg = Get(settings, LaunchConfigurationConstants.ArgsKey),
            guidLaunchDebugEngine = VSConstants.DebugEnginesGuids.NativeOnly_guid,
        };
        var results = new VsDebugTargetProcessInfo[1];
        debugger.LaunchDebugTargets4(1, new[] { target }, results);
    }

    private static string? Get(IPropertySettings settings, string key)
        => settings.TryGetValue(key, out object? value) ? value?.ToString() : null;
}
