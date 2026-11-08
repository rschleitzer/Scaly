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

    /// <summary>
    /// The extensibility object the settings are read through. Visual Studio
    /// does not offer it as a service to this extension (measured:
    /// ServiceUnavailableException), so the extension's parts, which get it
    /// injected, leave it here when they are made: the language server
    /// provider with the first .scaly document, a command when it runs.
    /// </summary>
    public static VisualStudioExtensibility? Extensibility
    {
        get => extensibility;
        set
        {
            extensibility = value;
            if (value is not null)
            {
                WatchSettings(value);
            }
        }
    }

    private static VisualStudioExtensibility? extensibility;

    /// <summary>Why the last setting read fell back to the default, or null.</summary>
    public static string? SettingsError { get; set; }

    /// <summary>
    /// When the tool or SCALY_HOME setting last changed (UTC); MinValue before
    /// any change. The Test Explorer discovers a file again only where its
    /// container's stamp is newer, and a setting changes no file: the test
    /// containers take this stamp too.
    /// </summary>
    public static DateTime SettingsChangedAt { get; private set; } = DateTime.MinValue;

    /// <summary>Raised when the tool or SCALY_HOME setting changed.</summary>
    public static event EventHandler? SettingsChanged;

    private static int watching;
    private static string? settingsSeen;
    private static IDisposable? settingsSubscription;

    // Subscribes once to the Scaly settings. The first call of the handler
    // brings the current values; a later one with other values for the tool
    // or the home is a change.
    private static void WatchSettings(VisualStudioExtensibility source)
    {
        if (Interlocked.Exchange(ref watching, 1) == 1)
        {
            return;
        }
        _ = Task.Run(async () =>
        {
            try
            {
                settingsSubscription = await source.Settings().SubscribeAsync(
                    new[] { ScalySettings.Category },
                    CancellationToken.None,
                    values =>
                    {
                        var seen = values.ValueOrDefault(ScalySettings.ToolPath, "") + "\n" + values.ValueOrDefault(ScalySettings.Home, "");
                        var before = Interlocked.Exchange(ref settingsSeen, seen);
                        if (before is not null && before != seen)
                        {
                            SettingsChangedAt = DateTime.UtcNow;
                            SettingsChanged?.Invoke(null, EventArgs.Empty);
                        }
                    });
            }
            catch (Exception e) when (e is not OperationCanceledException)
            {
                // Without the subscription a changed setting reaches the Test
                // Explorer with the next change of a file, as before.
                SettingsError = $"the Scaly settings could not be watched ({e.GetType().Name}: {e.Message})";
                Interlocked.Exchange(ref watching, 0);
            }
        });
    }

    public static async Task<string?> FindAsync(Setting.String setting, string name, CancellationToken cancellationToken)
    {
        var configured = (await ReadAsync(setting, cancellationToken)).Trim();
        if (configured.Length > 0)
        {
            return File.Exists(configured) ? configured : null;
        }
        return OnPath(name) ?? Installed(name);
    }

    /// <summary>The target triple to build for, or empty: the machine's own.</summary>
    public static async Task<string> TargetAsync(CancellationToken cancellationToken)
        => (await ReadAsync(ScalySettings.Target, cancellationToken)).Trim();

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
            var extensibility = Extensibility
                ?? await AsyncServiceProvider.GlobalProvider.GetServiceAsync<VisualStudioExtensibility, VisualStudioExtensibility>();
            var value = await extensibility.Settings().ReadEffectiveValueAsync(setting, cancellationToken);
            if (!value.Succeeded)
            {
                // ValueOrDefault would answer the default without a word.
                SettingsError = $"a Scaly setting could not be read ({value.Error})";
                return "";
            }
            return value.Value ?? "";
        }
        catch (Exception e) when (e is not OperationCanceledException)
        {
            // An unreadable setting is an empty one: fall back to PATH and the
            // installation, and say why where the build reports.
            SettingsError = Extensibility is null
                ? "the Scaly settings are read once a .scaly file has been opened in the editor"
                : $"a Scaly setting could not be read ({e.GetType().Name}: {e.Message})";
            return "";
        }
    }
}

#pragma warning restore VSEXTPREVIEW_SETTINGS
