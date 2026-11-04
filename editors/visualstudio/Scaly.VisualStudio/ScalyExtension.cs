namespace Scaly.VisualStudio;

using Microsoft.VisualStudio.Extensibility;
using Microsoft.VisualStudio.Extensibility.Settings;

/// <summary>
/// The extension: in process, because the grammar's .pkgdef and the Open
/// Folder and Test Explorer parts need it.
/// </summary>
[VisualStudioContribution]
internal sealed class ScalyExtension : Extension
{
    public override ExtensionConfiguration ExtensionConfiguration => new()
    {
        RequiresInProcessHosting = true,
    };
}

#pragma warning disable VSEXTPREVIEW_SETTINGS // The settings API is in preview.

/// <summary>
/// Tools > Options > Scaly. Empty means: find the program on PATH, then where
/// the scaly.io installer puts it (%USERPROFILE%\.scaly\toolchain\libexec).
/// </summary>
internal static class ScalySettings
{
    [VisualStudioContribution]
    internal static SettingCategory Category { get; } = new("scaly", "%Scaly.Settings.Category%")
    {
        Description = "%Scaly.Settings.Category.Description%",
    };

    [VisualStudioContribution]
    internal static Setting.String ServerPath { get; } = new("serverPath", "%Scaly.Settings.ServerPath%", Category, defaultValue: "")
    {
        Description = "%Scaly.Settings.ServerPath.Description%",
    };

    [VisualStudioContribution]
    internal static Setting.String ToolPath { get; } = new("toolPath", "%Scaly.Settings.ToolPath%", Category, defaultValue: "")
    {
        Description = "%Scaly.Settings.ToolPath.Description%",
    };

    [VisualStudioContribution]
    internal static Setting.String Home { get; } = new("home", "%Scaly.Settings.Home%", Category, defaultValue: "")
    {
        Description = "%Scaly.Settings.Home.Description%",
    };

    [VisualStudioContribution]
    internal static Setting.String Target { get; } = new("target", "%Scaly.Settings.Target%", Category, defaultValue: "")
    {
        Description = "%Scaly.Settings.Target.Description%",
    };
}

#pragma warning restore VSEXTPREVIEW_SETTINGS
