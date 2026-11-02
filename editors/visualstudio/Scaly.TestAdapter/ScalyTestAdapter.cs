// A test adapter for Scaly's test functions (`test*` returning int,
// 0 = passed), run by `scaly test <file>`. It runs in the test host, apart
// from Visual Studio, so it finds scaly on PATH or in the scaly.io
// installation and does not read the extension's settings.
namespace Scaly.TestAdapter;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Logging;

internal static class ScalyTests
{
    public const string ExecutorUri = "executor://scaly/v1";

    // Read from the source; a `scaly test --list` would make this exact.
    private static readonly Regex TestFunction = new(@"^\s*function\s+(test\w*)\s*\(\s*\)\s+returns\s+int\b");

    public static IEnumerable<TestCase> Discover(string source)
    {
        var lines = File.ReadAllLines(source);
        for (int i = 0; i < lines.Length; i++)
        {
            var m = TestFunction.Match(lines[i]);
            if (m.Success)
            {
                yield return new TestCase(m.Groups[1].Value, new Uri(ExecutorUri), source)
                {
                    DisplayName = m.Groups[1].Value,
                    CodeFilePath = source,
                    LineNumber = i + 1,
                };
            }
        }
    }

    public static string? FindScaly()
    {
        foreach (var dir in (Environment.GetEnvironmentVariable("PATH") ?? "").Split(Path.PathSeparator))
        {
            if (dir.Length == 0) continue;
            var p = Path.Combine(dir, "scaly.exe");
            if (File.Exists(p)) return p;
        }
        var home = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".scaly");
        foreach (var p in new[] { Path.Combine(home, "toolchain", "libexec", "scaly.exe"), Path.Combine(home, "bin", "scaly.exe") })
        {
            if (File.Exists(p)) return p;
        }
        return null;
    }

    private static readonly Regex ResultLine = new(@"^test\s+(\S+)\s+\.\.\.\s+(ok|FAIL)(.*)$");

    // Runs all tests of one file once and reports the requested ones.
    public static void Run(string source, IReadOnlyCollection<TestCase> tests, IFrameworkHandle handle)
    {
        var scaly = FindScaly();
        foreach (var t in tests) handle.RecordStart(t);
        var start = DateTimeOffset.Now;

        var results = new Dictionary<string, (bool ok, string detail)>();
        var output = new List<string>();
        if (scaly is null)
        {
            output.Add("scaly not found. Install it with 'irm https://scaly.io/install.ps1 | iex' in PowerShell.");
        }
        else
        {
            var filter = tests.Count == 1 ? " " + tests.First().FullyQualifiedName : "";
            var info = new ProcessStartInfo(scaly, $"test \"{source}\"{filter}")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
                WorkingDirectory = Path.GetDirectoryName(source),
            };
            using var p = Process.Start(info)!;
            var err = p.StandardError.ReadToEndAsync();
            string? line;
            while ((line = p.StandardOutput.ReadLine()) != null)
            {
                output.Add(line);
                var m = ResultLine.Match(line);
                if (m.Success) results[m.Groups[1].Value] = (m.Groups[2].Value == "ok", m.Groups[3].Value.Trim());
            }
            p.WaitForExit();
            output.AddRange(err.Result.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries).Select(l => l.TrimEnd('\r')));
        }

        var duration = DateTimeOffset.Now - start;
        foreach (var t in tests)
        {
            var r = new TestResult(t) { Duration = duration, StartTime = start, EndTime = DateTimeOffset.Now };
            if (results.TryGetValue(t.FullyQualifiedName, out var res))
            {
                r.Outcome = res.ok ? TestOutcome.Passed : TestOutcome.Failed;
                if (!res.ok) r.ErrorMessage = res.detail.Length > 0 ? res.detail : "failed";
            }
            else
            {
                // No result line: the file did not compile, or the test did not run.
                r.Outcome = TestOutcome.Failed;
                r.ErrorMessage = string.Join("\n", output);
            }
            handle.RecordEnd(t, r.Outcome);
            handle.RecordResult(r);
        }
    }
}

[FileExtension(".scaly")]
[DefaultExecutorUri(ScalyTests.ExecutorUri)]
public sealed class ScalyTestDiscoverer : ITestDiscoverer
{
    public void DiscoverTests(IEnumerable<string> sources, IDiscoveryContext discoveryContext, IMessageLogger logger, ITestCaseDiscoverySink discoverySink)
    {
        foreach (var source in sources.Where(s => s.EndsWith(".scaly", StringComparison.OrdinalIgnoreCase)))
        {
            foreach (var t in ScalyTests.Discover(source)) discoverySink.SendTestCase(t);
        }
    }
}

[ExtensionUri(ScalyTests.ExecutorUri)]
public sealed class ScalyTestExecutor : ITestExecutor
{
    public void RunTests(IEnumerable<TestCase>? tests, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
    {
        if (tests is null || frameworkHandle is null) return;
        foreach (var group in tests.GroupBy(t => t.Source))
        {
            ScalyTests.Run(group.Key, group.ToList(), frameworkHandle);
        }
    }

    public void RunTests(IEnumerable<string>? sources, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
    {
        if (sources is null) return;
        this.RunTests(sources.SelectMany(ScalyTests.Discover).ToList(), runContext, frameworkHandle);
    }

    public void Cancel()
    {
    }
}
