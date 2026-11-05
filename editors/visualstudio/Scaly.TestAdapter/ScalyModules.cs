// Which .scaly files of a folder are modules of another: `module shapes` in
// a program loads shapes.scaly beside it, and in a module file m.scaly it
// loads m/shapes.scaly. A module's tests are run, and listed, by the program
// that loads it, so a module file is no test container of its own. Read from
// the text, not compiled: a `module` inside a `define` (whose file lies in a
// directory named after the namespace) is not followed. Compiled into the
// extension (the containers) and into the adapter (the fallback that reads
// the tests from the source).
namespace Scaly.Modules;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

internal static class ScalyModules
{
    private static readonly Regex ModuleDeclaration = new(@"^(?:private\s+)?module\s+(\w+)\s*(?:;.*)?$", RegexOptions.Multiline);

    /// <summary>The files a file's `module` declarations may load, those that exist.</summary>
    public static IEnumerable<string> Children(string file)
    {
        string text;
        try { text = File.ReadAllText(file); }
        catch (IOException) { yield break; }
        catch (UnauthorizedAccessException) { yield break; }

        var directory = Path.GetDirectoryName(file)!;
        var stem = Path.GetFileNameWithoutExtension(file);
        foreach (Match m in ModuleDeclaration.Matches(text))
        {
            var name = m.Groups[1].Value + ".scaly";
            // As a program's module, then as a module's child: which one the
            // file is depends on who loads it, and either may exist.
            foreach (var candidate in new[] { Path.Combine(directory, name), Path.Combine(directory, stem, name) })
            {
                if (File.Exists(candidate))
                {
                    yield return Path.GetFullPath(candidate);
                }
            }
        }
    }

    /// <summary>The files among these that another one loads as a module.</summary>
    public static HashSet<string> ModuleFiles(IEnumerable<string> files)
    {
        var modules = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        foreach (var file in files)
        {
            var self = Path.GetFullPath(file);
            foreach (var child in Children(file))
            {
                if (!string.Equals(child, self, StringComparison.OrdinalIgnoreCase))
                {
                    modules.Add(child);
                }
            }
        }
        return modules;
    }

    /// <summary>The file and every module it loads, transitively; the file first.</summary>
    public static List<string> Closure(string file)
    {
        var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        var order = new List<string>();
        var pending = new Queue<string>();
        pending.Enqueue(Path.GetFullPath(file));
        while (pending.Count > 0)
        {
            var next = pending.Dequeue();
            if (!seen.Add(next)) continue;
            order.Add(next);
            foreach (var child in Children(next).Where(c => !seen.Contains(c)))
            {
                pending.Enqueue(child);
            }
        }
        return order;
    }
}
