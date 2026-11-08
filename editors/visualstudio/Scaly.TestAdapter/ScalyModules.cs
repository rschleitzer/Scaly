// Which .scaly files of a folder are modules of another: `module shapes` in
// a program loads shapes.scaly beside it, and in a module file m.scaly it
// loads m/shapes.scaly. Inside `define A { ... }` the file lies one directory
// deeper, A/shapes.scaly -- unless the directory is already named A, as for
// `define scaly { module memory }` in scaly.scaly (Modeler.handle_namespace).
// A module's tests are run, and listed, by the program that loads it, so a
// module file is no test container of its own. Read from the text, not
// compiled: the open defines are followed by their braces. Compiled into the
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
    private static readonly Regex ModuleDeclaration = new(@"^\s*(?:private\s+)?module\s+(\w+)\s*$");
    private static readonly Regex DefineHead = new(@"^\s*(?:private\s+)?define\s+(\w+)");
    // A declaration that begins after a define without a body: that define
    // opens no block, and the next `{` is not its.
    private static readonly Regex OtherDeclaration = new(@"^\s*(?:private\s+)?(?:function|procedure|operator|init|module|use|package|let|var|mutable|shared)\b");

    /// <summary>The files a file's `module` declarations may load, those that exist.</summary>
    public static IEnumerable<string> Children(string file)
    {
        string[] lines;
        try { lines = File.ReadAllLines(file); }
        catch (IOException) { yield break; }
        catch (UnauthorizedAccessException) { yield break; }

        var directory = Path.GetDirectoryName(file)!;
        var stem = Path.GetFileNameWithoutExtension(file);
        // As a program's module, then as a module's child: which one the file
        // is depends on who loads it, and either may exist.
        var bases = new[] { directory, Path.Combine(directory, stem) };

        // The defines open at this line, outermost first, each with the brace
        // depth its block opened at.
        var open = new List<(string name, int depth)>();
        string? pending = null;
        int depth = 0;
        foreach (var raw in lines)
        {
            var line = Code(raw);

            var module = ModuleDeclaration.Match(line);
            if (module.Success)
            {
                var name = module.Groups[1].Value + ".scaly";
                foreach (var start in bases)
                {
                    var candidate = Path.Combine(Within(start, open), name);
                    if (File.Exists(candidate))
                    {
                        yield return Path.GetFullPath(candidate);
                    }
                }
                pending = null;
                continue;
            }

            var define = DefineHead.Match(line);
            if (define.Success)
            {
                pending = define.Groups[1].Value;
            }
            else if (OtherDeclaration.IsMatch(line))
            {
                pending = null;
            }

            foreach (var c in line)
            {
                if (c == '{')
                {
                    depth++;
                    if (pending is not null)
                    {
                        open.Add((pending, depth));
                        pending = null;
                    }
                }
                else if (c == '}')
                {
                    if (open.Count > 0 && open[open.Count - 1].depth == depth)
                    {
                        open.RemoveAt(open.Count - 1);
                    }
                    depth--;
                }
            }
        }
    }

    // A line without its comment and without what its strings hold (their
    // quotes stay): a `;` or a brace in "...", '...' or `...` is no comment
    // and opens no block.
    private static string Code(string line)
    {
        var code = new System.Text.StringBuilder(line.Length);
        char quote = '\0';
        for (int i = 0; i < line.Length; i++)
        {
            var c = line[i];
            if (quote != '\0')
            {
                if (c == '\\')
                {
                    i++;
                }
                else if (c == quote)
                {
                    quote = '\0';
                    code.Append(c);
                }
                continue;
            }
            if (c == ';')
            {
                break;
            }
            if (c == '"' || c == '\'' || c == '`')
            {
                quote = c;
            }
            code.Append(c);
        }
        return code.ToString();
    }

    // The directory a module declared inside these defines lies in: one level
    // per define, except where the directory already bears its name.
    private static string Within(string start, List<(string name, int depth)> open)
    {
        var path = start;
        foreach (var (name, _) in open)
        {
            if (!string.Equals(Path.GetFileName(path), name, StringComparison.Ordinal))
            {
                path = Path.Combine(path, name);
            }
        }
        return path;
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
