namespace Scaly.VisualStudio.Workspace;

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
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
/// F5 (and Ctrl+F5) on a .scaly file: build it (so the program is never
/// stale), then start it under Visual Studio's native debugger. The program
/// carries CodeView and a .pdb, so breakpoints in the .scaly source bind.
/// Its command line, working directory and environment come from "Scaly
/// Launch Settings…" (<see cref="ScalyLaunchSettings"/>); an entry for the
/// file in .vs\launch.vs.json, where someone wrote one, takes precedence key
/// by key.
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
        var configuration = debugLaunchContext.LaunchConfiguration;
        var source = Text(configuration, LaunchConfigurationConstants.ProjectKey) ?? Text(configuration, LaunchConfigurationConstants.ProgramKey);
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

        var settings = ScalyLaunchSettings.Load(workspaceContext.Location, source);
        var arguments = Arguments(configuration) ?? settings.Arguments;
        var directory = Text(configuration, LaunchConfigurationConstants.CurrentDirKey);
        if (string.IsNullOrEmpty(directory))
        {
            directory = settings.WorkingDirectory;
        }
        if (string.IsNullOrEmpty(directory))
        {
            directory = Path.GetDirectoryName(source);
        }
        else if (!Path.IsPathRooted(directory))
        {
            directory = Path.Combine(workspaceContext.Location, directory);
        }
        var variables = new List<KeyValuePair<string, string>>(settings.Environment);
        variables.AddRange(Environment(configuration));

        await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
        if (serviceProvider.GetService(typeof(SVsShellDebugger)) is not IVsDebugger4 debugger)
        {
            return;
        }
        var target = new VsDebugTargetInfo4
        {
            dlo = (uint)DEBUG_LAUNCH_OPERATION.DLO_CreateProcess,
            LaunchFlags = vsDebugTargetInfo.LaunchFlags,
            bstrExe = ScalyBuild.ProgramOf(source),
            bstrCurDir = directory,
            bstrArg = arguments,
            bstrEnv = variables.Count == 0 ? null : EnvironmentBlock(variables),
            guidLaunchDebugEngine = VSConstants.DebugEnginesGuids.NativeOnly_guid,
        };
        var results = new VsDebugTargetProcessInfo[1];
        debugger.LaunchDebugTargets4(1, new[] { target }, results);
    }

    private static string? Text(IPropertySettings settings, string key)
        => settings.TryGetValue(key, out object? value) ? value?.ToString() : null;

    // "args" in launch.vs.json: a string, or an array of words.
    private static string? Arguments(IPropertySettings settings)
    {
        if (!settings.TryGetValue(LaunchConfigurationConstants.ArgsKey, out object? value) || value is null)
        {
            return null;
        }
        if (value is string text)
        {
            return text;
        }
        if (value is IEnumerable words)
        {
            return string.Join(" ", words.Cast<object?>().Select(w => Quote(w?.ToString() ?? "")));
        }
        return value.ToString();
    }

    // "env" in launch.vs.json: an object of NAME: value.
    private static IEnumerable<KeyValuePair<string, string>> Environment(IPropertySettings settings)
    {
        if (settings.TryGetValue(LaunchConfigurationConstants.EnvKey, out object? value)
            && value is IEnumerable<KeyValuePair<string, object>> variables)
        {
            foreach (var variable in variables)
            {
                yield return new KeyValuePair<string, string>(variable.Key, variable.Value?.ToString() ?? "");
            }
        }
    }

    // A word as the C runtime splits a command line.
    private static string Quote(string word)
    {
        if (word.Length > 0 && word.IndexOfAny(new[] { ' ', '\t', '"' }) < 0)
        {
            return word;
        }
        var quoted = new StringBuilder("\"");
        var backslashes = 0;
        foreach (var c in word)
        {
            if (c == '\\')
            {
                backslashes++;
                continue;
            }
            quoted.Append('\\', c == '"' ? backslashes * 2 + 1 : backslashes);
            backslashes = 0;
            quoted.Append(c);
        }
        quoted.Append('\\', backslashes * 2).Append('"');
        return quoted.ToString();
    }

    // The debugger takes a whole environment, not additions: Visual Studio's
    // own, with the program's variables over it, NAME=value each NUL-ended,
    // sorted as CreateProcess expects.
    private static string EnvironmentBlock(IEnumerable<KeyValuePair<string, string>> variables)
    {
        var all = new SortedDictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        foreach (DictionaryEntry entry in System.Environment.GetEnvironmentVariables())
        {
            all[(string)entry.Key] = (string?)entry.Value ?? "";
        }
        foreach (var variable in variables)
        {
            all[variable.Key] = variable.Value;
        }
        var block = new StringBuilder();
        foreach (var entry in all)
        {
            block.Append(entry.Key).Append('=').Append(entry.Value).Append('\0');
        }
        return block.Append('\0').ToString();
    }
}
