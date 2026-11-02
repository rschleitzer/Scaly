namespace Scaly.VisualStudio;

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.Extensibility;
using Microsoft.VisualStudio.Extensibility.Settings;
using Microsoft.VisualStudio.Shell;

#pragma warning disable VSEXTPREVIEW_SETTINGS // The settings API is in preview.

/// <summary>
/// Finds the Scaly programs and the SCALY_HOME to hand them, the way the VS
/// Code extension does: an explicit setting wins, then PATH, then the
/// scaly.io installation.
/// </summary>
internal static class ScalyPrograms
{
    public const string InstallCommand = "irm https://scaly.io/install.ps1 | iex";

    public static async Task<string?> FindAsync(Setting.String setting, string name, CancellationToken cancellationToken)
    {
        var configured = (await ReadAsync(setting, cancellationToken)).Trim();
        if (configured.Length > 0)
        {
            return File.Exists(configured) ? configured : null;
        }
        return OnPath(name) ?? Installed(name);
    }

    /// <summary>
    /// SCALY_HOME: the setting, else the open folder when it is a Scaly checkout,
    /// else null (an installed program sets its own).
    /// </summary>
    public static async Task<string?> HomeAsync(string? folder, CancellationToken cancellationToken)
    {
        var configured = (await ReadAsync(ScalySettings.Home, cancellationToken)).Trim();
        if (configured.Length > 0)
        {
            return configured;
        }
        if (folder is not null && Directory.Exists(Path.Combine(folder, "packages", "scaly")))
        {
            return folder;
        }
        return null;
    }

    private static string? OnPath(string name)
    {
        foreach (var dir in (Environment.GetEnvironmentVariable("PATH") ?? "").Split(Path.PathSeparator))
        {
            if (dir.Length == 0)
            {
                continue;
            }
            try
            {
                var candidate = Path.Combine(dir.Trim('"'), name + ".exe");
                if (File.Exists(candidate))
                {
                    return candidate;
                }
            }
            catch (ArgumentException)
            {
                // A malformed PATH entry.
            }
        }
        return null;
    }

    // Where install.ps1 puts the programs. A Visual Studio started before the
    // installation ran does not see the PATH it extended.
    private static string? Installed(string name)
    {
        var home = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".scaly");
        foreach (var candidate in new[]
        {
            Path.Combine(home, "toolchain", "libexec", name + ".exe"),
            Path.Combine(home, "bin", name + ".exe"),
        })
        {
            if (File.Exists(candidate))
            {
                return candidate;
            }
        }
        return null;
    }

    private static async Task<string> ReadAsync(Setting.String setting, CancellationToken cancellationToken)
    {
        try
        {
            var extensibility = await AsyncServiceProvider.GlobalProvider.GetServiceAsync<VisualStudioExtensibility, VisualStudioExtensibility>();
            var value = await extensibility.Settings().ReadEffectiveValueAsync(setting, cancellationToken);
            return value.ValueOrDefault(defaultValue: "") ?? "";
        }
        catch (Exception e) when (e is not OperationCanceledException)
        {
            // An unreadable setting is an empty one: fall back to PATH and the installation.
            return "";
        }
    }
}

#pragma warning restore VSEXTPREVIEW_SETTINGS
