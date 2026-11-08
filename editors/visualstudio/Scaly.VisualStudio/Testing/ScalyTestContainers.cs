// Tells the Test Explorer which files of the open folder hold Scaly
// tests; the adapter (Scaly.TestAdapter.dll) discovers and runs them.
namespace Scaly.VisualStudio.Testing;

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Xml;
using System.Xml.XPath;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestWindow.Extensibility;
using Microsoft.VisualStudio.TestWindow.Extensibility.Model;
using Microsoft.VisualStudio.Workspace.VSIntegration.Contracts;
using Scaly.Modules;

[Export(typeof(ITestContainerDiscoverer))]
internal sealed class ScalyTestContainerDiscoverer : ITestContainerDiscoverer, IDisposable
{
    public static readonly Uri Executor = new("executor://scaly/v1");
    // A file is offered when a line opens something that may be a test; the
    // adapter asks the tool which tests it holds (scaly test --list).
    private static readonly Regex TestFunction = new(@"^\s*(?:function|procedure)\s+test(?:_\w*)?\s*\(", RegexOptions.Multiline);

    private readonly IVsFolderWorkspaceService workspaceService;
    private readonly EventHandler settingsChanged;
    private FileSystemWatcher? watcher;
    private string? watchedRoot;

    [ImportingConstructor]
    public ScalyTestContainerDiscoverer(IVsFolderWorkspaceService workspaceService)
    {
        this.workspaceService = workspaceService;
        // Another tool or SCALY_HOME names the tests anew (a newer tool lists
        // `Outer.Inner.test_deep` where an older one is read from the source):
        // every container is one to discover again.
        this.settingsChanged = (_, _) => this.TestContainersUpdated?.Invoke(this, EventArgs.Empty);
        ScalyPrograms.SettingsChanged += this.settingsChanged;
    }

    public Uri ExecutorUri => Executor;

    public event EventHandler? TestContainersUpdated;

    public IEnumerable<ITestContainer> TestContainers
    {
        get
        {
            var root = this.workspaceService.CurrentWorkspace?.Location;
            if (root is null || !Directory.Exists(root))
            {
                return Array.Empty<ITestContainer>();
            }
            this.Watch(root);
            // A module's tests belong to the program that loads it: the
            // program is the container, and a change to the module makes it
            // one to discover again.
            var files = Directory.EnumerateFiles(root, "*.scaly", SearchOption.AllDirectories).ToList();
            var modules = ScalyModules.ModuleFiles(files);
            var containers = new List<ITestContainer>();
            foreach (var file in files.Where(f => !modules.Contains(Path.GetFullPath(f))))
            {
                var closure = ScalyModules.Closure(file);
                if (closure.Any(HasTests))
                {
                    var written = closure.Max(LastWrite);
                    var settings = ScalyPrograms.SettingsChangedAt;
                    containers.Add(new ScalyTestContainer(this, file, settings > written ? settings : written));
                }
            }
            return containers;
        }
    }

    private static DateTime LastWrite(string file)
    {
        try { return File.GetLastWriteTimeUtc(file); }
        catch (IOException) { return DateTime.MinValue; }
    }

    private static bool HasTests(string file)
    {
        try { return TestFunction.IsMatch(File.ReadAllText(file)); }
        catch (IOException) { return false; }
    }

    private void Watch(string root)
    {
        if (this.watchedRoot == root) return;
        this.watcher?.Dispose();
        this.watchedRoot = root;
        this.watcher = new FileSystemWatcher(root, "*.scaly") { IncludeSubdirectories = true };
        FileSystemEventHandler changed = (_, _) => this.TestContainersUpdated?.Invoke(this, EventArgs.Empty);
        this.watcher.Changed += changed;
        this.watcher.Created += changed;
        this.watcher.Deleted += changed;
        this.watcher.Renamed += (_, _) => this.TestContainersUpdated?.Invoke(this, EventArgs.Empty);
        this.watcher.EnableRaisingEvents = true;
    }

    public void Dispose()
    {
        ScalyPrograms.SettingsChanged -= this.settingsChanged;
        this.watcher?.Dispose();
    }
}

/// <summary>
/// Hands the test adapter, which runs in the test host apart from Visual
/// Studio, the tool and SCALY_HOME the builds use: an element
/// <c>&lt;ScalyAdapter&gt;&lt;Tool/&gt;&lt;Home/&gt;&lt;/ScalyAdapter&gt;</c> in the run settings.
/// </summary>
[Export(typeof(IRunSettingsService))]
internal sealed class ScalyRunSettings : IRunSettingsService
{
    private readonly IVsFolderWorkspaceService workspaceService;

    [ImportingConstructor]
    public ScalyRunSettings(IVsFolderWorkspaceService workspaceService)
    {
        this.workspaceService = workspaceService;
    }

    public string Name => "ScalyAdapter";

    public IXPathNavigable AddRunSettings(IXPathNavigable inputRunSettingDocument, IRunSettingsConfigurationInfo configurationInfo, ILogger log)
    {
        var folder = this.workspaceService.CurrentWorkspace?.Location;
        var (tool, home) = ThreadHelper.JoinableTaskFactory.Run(async () =>
            (await ScalyPrograms.FindAsync(ScalySettings.ToolPath, "scaly", CancellationToken.None),
             await ScalyPrograms.HomeAsync(folder, CancellationToken.None)));

        var document = new XmlDocument();
        document.LoadXml(inputRunSettingDocument.CreateNavigator()!.OuterXml);
        var root = document.DocumentElement;
        if (root is null)
        {
            return inputRunSettingDocument;
        }
        if (root.SelectSingleNode(this.Name) is XmlNode old)
        {
            root.RemoveChild(old);
        }
        var element = document.CreateElement(this.Name);
        element.AppendChild(document.CreateElement("Tool")).InnerText = tool ?? "";
        element.AppendChild(document.CreateElement("Home")).InnerText = home ?? "";
        root.AppendChild(element);
        return document;
    }
}

internal sealed class ScalyTestContainer : ITestContainer
{
    private readonly DateTime stamp;

    /// <param name="stamp">The newest write of the file and the modules it loads, or the last change of the tool or home setting if that is newer.</param>
    public ScalyTestContainer(ITestContainerDiscoverer discoverer, string source, DateTime stamp)
    {
        this.Discoverer = discoverer;
        this.Source = source;
        this.stamp = stamp;
    }

    public ITestContainerDiscoverer Discoverer { get; }
    public string Source { get; }
    public IEnumerable<Guid> DebugEngines => Array.Empty<Guid>();
    public FrameworkVersion TargetFramework => FrameworkVersion.None;
    public Architecture TargetPlatform => Architecture.AnyCPU;
    public bool IsAppContainerTestContainer => false;

    public IDeploymentData? DeployAppContainer() => null;

    public int CompareTo(ITestContainer other)
    {
        if (other is not ScalyTestContainer c) return -1;
        int byName = string.Compare(this.Source, c.Source, StringComparison.OrdinalIgnoreCase);
        return byName != 0 ? byName : this.stamp.CompareTo(c.stamp);
    }

    public ITestContainer Snapshot() => new ScalyTestContainer(this.Discoverer, this.Source, this.stamp);
}
