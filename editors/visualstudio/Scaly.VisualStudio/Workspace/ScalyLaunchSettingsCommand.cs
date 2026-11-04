namespace Scaly.VisualStudio.Workspace;

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using Microsoft.VisualStudio.ComponentModelHost;
using Microsoft.VisualStudio.Extensibility;
using Microsoft.VisualStudio.Extensibility.Commands;
using Microsoft.VisualStudio.PlatformUI;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Workspace.VSIntegration.Contracts;

/// <summary>
/// "Scaly Launch Settings…" on a .scaly file, in the Solution Explorer of an
/// open folder and in the editor's context menu: the command line, working
/// directory and environment F5 and Ctrl+F5 start the program with.
/// </summary>
[VisualStudioContribution]
internal sealed class ScalyLaunchSettingsCommand : Command
{
    public override CommandConfiguration CommandConfiguration => new("%Scaly.LaunchSettings.DisplayName%")
    {
        VisibleWhen = ActivationConstraint.ClientContext(ClientContextKey.Shell.ActiveSelectionFileName, @"\.scaly$"),
    };

    public override async Task ExecuteCommandAsync(IClientContext context, CancellationToken cancellationToken)
    {
        ScalyPrograms.Extensibility ??= this.Extensibility;
        var selected = await context.GetSelectedPathAsync(cancellationToken);
        if (selected is null || !selected.IsFile || !ScalyBuild.IsScaly(selected.LocalPath))
        {
            return;
        }
        var source = selected.LocalPath;

        await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
        var folder = Folder() ?? Path.GetDirectoryName(source)!;
        var settings = ScalyLaunchSettings.Load(folder, source);
        if (new ScalyLaunchSettingsDialog(Path.GetFileName(source), settings).ShowModal() == true)
        {
            ScalyLaunchSettings.Save(folder, source, settings);
        }
    }

    /// <summary>The open folder, or null outside Open Folder.</summary>
    internal static string? Folder()
    {
        ThreadHelper.ThrowIfNotOnUIThread();
        var model = ServiceProvider.GlobalProvider.GetService(typeof(SComponentModel)) as IComponentModel;
        return model?.GetService<IVsFolderWorkspaceService>()?.CurrentWorkspace?.Location;
    }
}

/// <summary>Where the command stands: Solution Explorer (Open Folder) and the editor.</summary>
internal static class ScalyMenus
{
    // The file context menu of Solution Explorer in Open Folder mode
    // (Microsoft.VisualStudio.Workspace.VSIntegration: PackageIds.idmWSE_ContextMenu).
    private static readonly Guid WorkspaceExplorerCommandSet = new("cfb400f1-5c60-4f3c-856e-180d28def0b7");
    private const uint WorkspaceExplorerContextMenu = 0x0002;

    // The editor's context menu (guidSHLMainMenu, IDM_VS_CTXT_CODEWIN).
    private static readonly Guid ShellMainMenu = new("d309f791-903f-11d0-9efc-00a0c911004f");
    private const uint CodeWindowContextMenu = 0x040D;

    [VisualStudioContribution]
    public static CommandGroupConfiguration LaunchGroup => new(
        GroupPlacement.VsctParent(WorkspaceExplorerCommandSet, WorkspaceExplorerContextMenu, priority: 0x0180),
        GroupPlacement.VsctParent(ShellMainMenu, CodeWindowContextMenu, priority: 0x0700))
    {
        Children = new[] { GroupChild.Command<ScalyLaunchSettingsCommand>() },
    };
}

/// <summary>The dialog: three fields, OK and Cancel.</summary>
internal sealed class ScalyLaunchSettingsDialog : DialogWindow
{
    private readonly ScalyLaunchSettings settings;
    private readonly TextBox arguments = new() { Margin = new Thickness(0, 2, 0, 10) };
    private readonly TextBox workingDirectory = new() { Margin = new Thickness(0, 2, 0, 10) };
    private readonly TextBox environment = new()
    {
        AcceptsReturn = true,
        VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
        HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
        Height = 110,
        Margin = new Thickness(0, 2, 0, 10),
        FontFamily = new System.Windows.Media.FontFamily("Consolas"),
    };

    public ScalyLaunchSettingsDialog(string file, ScalyLaunchSettings settings)
    {
        this.settings = settings;
        this.Title = "Scaly Launch Settings — " + file;
        this.Width = 520;
        this.SizeToContent = SizeToContent.Height;
        this.ResizeMode = ResizeMode.NoResize;
        this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
        this.HasMinimizeButton = false;
        this.HasMaximizeButton = false;

        this.arguments.Text = settings.Arguments;
        this.workingDirectory.Text = settings.WorkingDirectory;
        this.environment.Text = settings.EnvironmentText;

        var ok = new Button { Content = "OK", IsDefault = true, MinWidth = 75, Margin = new Thickness(0, 0, 8, 0) };
        ok.Click += (_, _) => this.Accept();
        var cancel = new Button { Content = "Cancel", IsCancel = true, MinWidth = 75 };
        var buttons = new StackPanel { Orientation = Orientation.Horizontal, HorizontalAlignment = HorizontalAlignment.Right };
        buttons.Children.Add(ok);
        buttons.Children.Add(cancel);

        var panel = new StackPanel { Margin = new Thickness(12) };
        panel.Children.Add(Label("_Arguments:", this.arguments, "The command line after the program's name, as in a console: Ada Grace"));
        panel.Children.Add(this.arguments);
        panel.Children.Add(Label("_Working directory:", this.workingDirectory, "Empty: the folder of the .scaly file."));
        panel.Children.Add(this.workingDirectory);
        panel.Children.Add(Label("_Environment:", this.environment, "One NAME=value a line, added to Visual Studio's environment."));
        panel.Children.Add(this.environment);
        panel.Children.Add(buttons);
        this.Content = panel;
    }

    private static UIElement Label(string text, Control target, string hint)
    {
        var stack = new StackPanel();
        stack.Children.Add(new Label { Content = text, Target = target, Padding = new Thickness(0) });
        stack.Children.Add(new TextBlock { Text = hint, Opacity = 0.7, TextWrapping = TextWrapping.Wrap, Margin = new Thickness(0, 1, 0, 0) });
        return stack;
    }

    private void Accept()
    {
        this.settings.Arguments = this.arguments.Text.Trim();
        this.settings.WorkingDirectory = this.workingDirectory.Text.Trim();
        this.settings.SetEnvironment(this.environment.Text);
        this.DialogResult = true;
    }
}
