// Tells the Test Explorer which files of the open folder hold Scaly
// tests; the adapter (Scaly.TestAdapter.dll) discovers and runs them.
namespace Scaly.VisualStudio.Testing;

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestWindow.Extensibility;
using Microsoft.VisualStudio.TestWindow.Extensibility.Model;
using Microsoft.VisualStudio.Workspace.VSIntegration.Contracts;

[Export(typeof(ITestContainerDiscoverer))]
internal sealed class ScalyTestContainerDiscoverer : ITestContainerDiscoverer, IDisposable
{
    public static readonly Uri Executor = new("executor://scaly/v1");
    private static readonly Regex TestFunction = new(@"^\s*function\s+test\w*\s*\(\s*\)\s+returns\s+int\b", RegexOptions.Multiline);

    private readonly IVsFolderWorkspaceService workspaceService;
    private FileSystemWatcher? watcher;
    private string? watchedRoot;

    [ImportingConstructor]
    public ScalyTestContainerDiscoverer(IVsFolderWorkspaceService workspaceService)
    {
        this.workspaceService = workspaceService;
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
            return Directory.EnumerateFiles(root, "*.scaly", SearchOption.AllDirectories)
                .Where(HasTests)
                .Select(f => (ITestContainer)new ScalyTestContainer(this, f))
                .ToList();
        }
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

    public void Dispose() => this.watcher?.Dispose();
}

internal sealed class ScalyTestContainer : ITestContainer
{
    private readonly DateTime stamp;

    public ScalyTestContainer(ITestContainerDiscoverer discoverer, string source)
        : this(discoverer, source, File.GetLastWriteTimeUtc(source))
    {
    }

    private ScalyTestContainer(ITestContainerDiscoverer discoverer, string source, DateTime stamp)
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
