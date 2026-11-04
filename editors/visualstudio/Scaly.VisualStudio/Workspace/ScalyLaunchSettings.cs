namespace Scaly.VisualStudio.Workspace;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

/// <summary>
/// What a .scaly program is started with: its command line, its working
/// directory and the variables added to its environment. Set per file with
/// "Scaly Launch Settings…", kept out of sight in the folder's .vs directory
/// (<c>.vs\scaly\launch.txt</c>), one section per file:
/// <code>
/// [greet.scaly]
/// args=Ada Grace
/// cwd=
/// env=GREETING=Hello
/// </code>
/// </summary>
internal sealed class ScalyLaunchSettings
{
    public string Arguments { get; set; } = "";

    /// <summary>Empty: the folder of the source file.</summary>
    public string WorkingDirectory { get; set; } = "";

    public List<KeyValuePair<string, string>> Environment { get; } = new();

    public bool IsEmpty => this.Arguments.Length == 0 && this.WorkingDirectory.Length == 0 && this.Environment.Count == 0;

    public static ScalyLaunchSettings Load(string folder, string source)
    {
        var all = ReadAll(folder);
        return all.TryGetValue(Key(folder, source), out var settings) ? settings : new ScalyLaunchSettings();
    }

    public static void Save(string folder, string source, ScalyLaunchSettings settings)
    {
        var all = ReadAll(folder);
        var key = Key(folder, source);
        if (settings.IsEmpty)
        {
            all.Remove(key);
        }
        else
        {
            all[key] = settings;
        }

        var text = new StringBuilder();
        text.Append("# Scaly launch settings, one section per file. Written by Visual Studio's Scaly extension.\n");
        foreach (var entry in all.OrderBy(e => e.Key, StringComparer.OrdinalIgnoreCase))
        {
            text.Append('\n').Append('[').Append(entry.Key).Append("]\n");
            text.Append("args=").Append(OneLine(entry.Value.Arguments)).Append('\n');
            text.Append("cwd=").Append(OneLine(entry.Value.WorkingDirectory)).Append('\n');
            foreach (var variable in entry.Value.Environment)
            {
                text.Append("env=").Append(OneLine(variable.Key)).Append('=').Append(OneLine(variable.Value)).Append('\n');
            }
        }
        var path = StorePath(folder);
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, text.ToString(), new UTF8Encoding(false));
    }

    /// <summary>The environment as text, one NAME=value a line, for the dialog.</summary>
    public string EnvironmentText => string.Join("\r\n", this.Environment.Select(v => v.Key + "=" + v.Value));

    /// <summary>Reads NAME=value lines; a line without '=' or with an empty name is left out.</summary>
    public void SetEnvironment(string text)
    {
        this.Environment.Clear();
        foreach (var line in text.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None))
        {
            var variable = Variable(line.Trim());
            if (variable is not null)
            {
                this.Environment.Add(variable.Value);
            }
        }
    }

    private static KeyValuePair<string, string>? Variable(string line)
    {
        var equals = line.IndexOf('=');
        if (equals <= 0)
        {
            return null;
        }
        return new KeyValuePair<string, string>(line.Substring(0, equals).Trim(), line.Substring(equals + 1));
    }

    private static string StorePath(string folder) => Path.Combine(folder, ".vs", "scaly", "launch.txt");

    // The section name: the file relative to the folder, with forward slashes.
    private static string Key(string folder, string source)
    {
        var full = Path.GetFullPath(source);
        var root = Path.GetFullPath(folder).TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar) + Path.DirectorySeparatorChar;
        var relative = full.StartsWith(root, StringComparison.OrdinalIgnoreCase) ? full.Substring(root.Length) : full;
        return relative.Replace('\\', '/');
    }

    private static string OneLine(string text) => text.Replace("\r", " ").Replace("\n", " ");

    private static Dictionary<string, ScalyLaunchSettings> ReadAll(string folder)
    {
        var all = new Dictionary<string, ScalyLaunchSettings>(StringComparer.OrdinalIgnoreCase);
        string[] lines;
        try
        {
            lines = File.ReadAllLines(StorePath(folder));
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            return all;
        }

        ScalyLaunchSettings? current = null;
        foreach (var raw in lines)
        {
            var line = raw.TrimEnd();
            if (line.Length == 0 || line.StartsWith("#", StringComparison.Ordinal))
            {
                continue;
            }
            if (line.StartsWith("[", StringComparison.Ordinal) && line.EndsWith("]", StringComparison.Ordinal))
            {
                current = new ScalyLaunchSettings();
                all[line.Substring(1, line.Length - 2)] = current;
                continue;
            }
            if (current is null)
            {
                continue;
            }
            var equals = line.IndexOf('=');
            if (equals < 0)
            {
                continue;
            }
            var value = line.Substring(equals + 1);
            switch (line.Substring(0, equals))
            {
                case "args":
                    current.Arguments = value;
                    break;
                case "cwd":
                    current.WorkingDirectory = value;
                    break;
                case "env":
                    var variable = Variable(value);
                    if (variable is not null)
                    {
                        current.Environment.Add(variable.Value);
                    }
                    break;
            }
        }
        return all;
    }
}
