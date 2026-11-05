// A test adapter for Scaly's test functions (a function or procedure named
// `test` or `test_<name>`, taking nothing, answering an int, 0 = passed), run
// by `scaly test <file>`. It runs in the test host, apart from Visual Studio:
// the extension hands it the tool and SCALY_HOME it builds with through the
// run settings (<ScalyAdapter>); without them it finds scaly on PATH or in the
// scaly.io installation.
namespace Scaly.TestAdapter;

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Logging;
using Scaly.Modules;

/// <summary>The tool and the home the tests run with.</summary>
internal sealed class ScalyRun
{
    public string? Tool { get; private set; }

    public string? Home { get; private set; }

    /// <summary>From the run settings the extension wrote, else PATH and the installation.</summary>
    public static ScalyRun From(string? settingsXml)
    {
        var run = new ScalyRun();
        if (!string.IsNullOrEmpty(settingsXml))
        {
            try
            {
                var element = XDocument.Parse(settingsXml).Root?.Element("ScalyAdapter");
                run.Tool = NonEmpty(element?.Element("Tool")?.Value);
                run.Home = NonEmpty(element?.Element("Home")?.Value);
            }
            catch (System.Xml.XmlException)
            {
                // Unreadable settings are none.
            }
        }
        if (run.Tool is null || !File.Exists(run.Tool))
        {
            run.Tool = FindScaly();
        }
        return run;
    }

    private static string? NonEmpty(string? text) => string.IsNullOrWhiteSpace(text) ? null : text!.Trim();

    private static string? FindScaly()
    {
        foreach (var dir in (Environment.GetEnvironmentVariable("PATH") ?? "").Split(Path.PathSeparator))
        {
            if (dir.Length == 0) continue;
            try
            {
                var p = Path.Combine(dir.Trim('"'), "scaly.exe");
                if (File.Exists(p)) return p;
            }
            catch (ArgumentException)
            {
                // A malformed PATH entry.
            }
        }
        var home = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".scaly");
        foreach (var p in new[] { Path.Combine(home, "toolchain", "libexec", "scaly.exe"), Path.Combine(home, "bin", "scaly.exe") })
        {
            if (File.Exists(p)) return p;
        }
        return null;
    }

    /// <summary>Runs <c>scaly test</c> with these arguments: its exit code, stdout and stderr lines.</summary>
    public (int code, List<string> output, List<string> errors) Test(string source, string arguments)
    {
        var info = new ProcessStartInfo(this.Tool!, $"test \"{source}\"{arguments}")
        {
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true,
            WorkingDirectory = Path.GetDirectoryName(source),
        };
        if (this.Home is not null)
        {
            info.EnvironmentVariables["SCALY_HOME"] = this.Home;
        }
        using var p = Process.Start(info)!;
        var err = p.StandardError.ReadToEndAsync();
        var output = new List<string>();
        string? line;
        while ((line = p.StandardOutput.ReadLine()) != null)
        {
            output.Add(line);
        }
        p.WaitForExit();
        var errors = err.Result.Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries).Select(l => l.TrimEnd('\r')).ToList();
        return (p.ExitCode, output, errors);
    }
}

internal static class ScalyTests
{
    public const string ExecutorUri = "executor://scaly/v1";

    // The fallback where `scaly test --list` has no answer: the file does not
    // compile, or the tool predates the flag. It reads the file and its modules,
    // misses a signature spread over lines and does not see a namespace.
    private static readonly Regex TestFunction = new(@"^\s*(?:function|procedure)\s+(test(?:_\w*)?)\s*\(\s*\)\s+returns\s+int\b");

    public static IEnumerable<TestCase> Discover(string source, ScalyRun run, IMessageLogger? logger)
    {
        if (run.Tool is not null)
        {
            var (code, output, errors) = run.Test(source, " --list");
            // An older tool takes --list for a filter and answers "no tests: ...".
            if (code == 0 && !output.Any(l => l.StartsWith("no tests:", StringComparison.Ordinal)))
            {
                var directory = Path.GetDirectoryName(source)!;
                var found = new List<TestCase>();
                foreach (var line in output)
                {
                    var parts = line.Split('\t');
                    if (parts.Length != 3 || !int.TryParse(parts[2], out int number))
                    {
                        continue;
                    }
                    found.Add(Test(parts[0], source, Path.GetFullPath(Path.Combine(directory, parts[1])), number));
                }
                return found;
            }
            logger?.SendMessage(
                TestMessageLevel.Informational,
                $"Scaly: {Path.GetFileName(source)} did not list its tests ({string.Join(" ", errors.Concat(output).Take(3))}); they are read from the source instead.");
        }
        return FromSource(source);
    }

    // Where the Test Explorer puts a test: container, namespace, class, test
    // group. Without it, it splits the name at its dots and shows
    // <Empty Namespace> and <Empty Class> for a name without any.
    private static readonly TestProperty Hierarchy = TestProperty.Register(
        "TestCase.Hierarchy", "Hierarchy", typeof(string[]), TestPropertyAttributes.Immutable, typeof(TestCase));

    // A test as the tool names it (`test_add`, `Suite.test_inside`), under its
    // container (the program), the folder of the file it is written in, named
    // from the program's folder on (`modules`, `modules\helpers`), and the file.
    private static TestCase Test(string label, string source, string file, int line)
    {
        var test = new TestCase(label, new Uri(ExecutorUri), source)
        {
            DisplayName = label,
            CodeFilePath = file,
            LineNumber = line,
        };
        var home = Path.GetDirectoryName(source)!.TrimEnd(Path.DirectorySeparatorChar);
        var folder = Path.GetDirectoryName(file)!.TrimEnd(Path.DirectorySeparatorChar);
        var parent = Path.GetDirectoryName(home);
        var shown = parent is not null && folder.StartsWith(parent + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase)
            ? folder.Substring(parent.Length + 1)
            : folder;
        test.SetPropertyValue(Hierarchy, new string?[] { null, shown, Path.GetFileName(file), label });
        return test;
    }

    // The file's own tests and those of the modules it loads, which the tool
    // would list with the file too.
    private static IEnumerable<TestCase> FromSource(string source)
    {
        foreach (var file in ScalyModules.Closure(source))
        {
            var lines = File.ReadAllLines(file);
            for (int i = 0; i < lines.Length; i++)
            {
                var m = TestFunction.Match(lines[i]);
                if (m.Success)
                {
                    yield return Test(m.Groups[1].Value, source, file, i + 1);
                }
            }
        }
    }

    // `test <name> ... ok` / `test <name> ... FAIL (answered 1)`; a test that
    // traps leaves its line at `test <name> ... ` and the trap's message after.
    private static readonly Regex ResultLine = new(@"^test\s+(\S+)\s+\.\.\.\s*(ok|FAIL)?(.*)$");
    private static readonly Regex TrailingVerdict = new(@"^(ok|FAIL)( \(answered -?\d+\))?$");

    // Runs all tests of one file once and reports the requested ones.
    public static void Run(string source, IReadOnlyCollection<TestCase> tests, ScalyRun run, IFrameworkHandle handle)
    {
        foreach (var t in tests) handle.RecordStart(t);
        var start = DateTimeOffset.Now;

        var results = new Dictionary<string, (TestOutcome outcome, string detail)>();
        var output = new List<string>();
        if (run.Tool is null)
        {
            output.Add("scaly not found. Install it with 'irm https://scaly.io/install.ps1 | iex' in PowerShell.");
        }
        else
        {
            var filter = tests.Count == 1 ? " " + tests.First().FullyQualifiedName : "";
            var (_, lines, errors) = run.Test(source, filter);
            output.AddRange(lines);
            output.AddRange(errors);

            string? running = null;
            var trace = new List<string>();
            foreach (var line in lines)
            {
                var m = ResultLine.Match(line);
                if (m.Success)
                {
                    running = null;
                    if (m.Groups[2].Success)
                    {
                        results[m.Groups[1].Value] = (m.Groups[2].Value == "ok" ? TestOutcome.Passed : TestOutcome.Failed, m.Groups[3].Value.Trim());
                    }
                    else
                    {
                        running = m.Groups[1].Value;
                        trace.Clear();
                        trace.Add(m.Groups[3].Value);
                    }
                }
                else if (running is not null)
                {
                    // A test that prints: its own lines stand between its
                    // name and the verdict, which ends a line of its own.
                    var verdict = TrailingVerdict.Match(line);
                    if (verdict.Success)
                    {
                        results[running] = (verdict.Groups[1].Value == "ok" ? TestOutcome.Passed : TestOutcome.Failed, verdict.Groups[2].Value.Trim());
                        running = null;
                    }
                    else
                    {
                        trace.Add(line);
                    }
                }
            }
            if (running is not null)
            {
                // The run ended inside this test: it trapped, and the tests
                // after it never ran.
                trace.AddRange(errors);
                var message = string.Join("\n", trace.Where(l => l.Trim().Length > 0));
                results[running] = (TestOutcome.Failed, message.Length > 0 ? message : "the test ended the run");
                foreach (var t in tests.Where(t => !results.ContainsKey(t.FullyQualifiedName)))
                {
                    results[t.FullyQualifiedName] = (TestOutcome.Skipped, $"not run: {running} ended the run before it");
                }
            }
        }

        var duration = DateTimeOffset.Now - start;
        foreach (var t in tests)
        {
            var r = new TestResult(t) { Duration = duration, StartTime = start, EndTime = DateTimeOffset.Now };
            if (results.TryGetValue(t.FullyQualifiedName, out var res))
            {
                r.Outcome = res.outcome;
                if (res.outcome != TestOutcome.Passed) r.ErrorMessage = res.detail.Length > 0 ? res.detail : "failed";
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
        var run = ScalyRun.From(discoveryContext?.RunSettings?.SettingsXml);
        foreach (var source in sources.Where(s => s.EndsWith(".scaly", StringComparison.OrdinalIgnoreCase)))
        {
            foreach (var t in ScalyTests.Discover(source, run, logger)) discoverySink.SendTestCase(t);
        }
    }
}

[ExtensionUri(ScalyTests.ExecutorUri)]
public sealed class ScalyTestExecutor : ITestExecutor
{
    public void RunTests(IEnumerable<TestCase>? tests, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
    {
        if (tests is null || frameworkHandle is null) return;
        var run = ScalyRun.From(runContext?.RunSettings?.SettingsXml);
        foreach (var group in tests.GroupBy(t => t.Source))
        {
            ScalyTests.Run(group.Key, group.ToList(), run, frameworkHandle);
        }
    }

    public void RunTests(IEnumerable<string>? sources, IRunContext? runContext, IFrameworkHandle? frameworkHandle)
    {
        if (sources is null) return;
        var run = ScalyRun.From(runContext?.RunSettings?.SettingsXml);
        this.RunTests(sources.SelectMany(s => ScalyTests.Discover(s, run, frameworkHandle)).ToList(), runContext, frameworkHandle);
    }

    public void Cancel()
    {
    }
}
