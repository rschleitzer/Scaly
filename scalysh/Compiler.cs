using System.Collections.Generic;

namespace scalyc
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var sources = new List<string>();
            {
                foreach (string file in options.files)
                {
                    try
                    {
                        var source = System.IO.File.ReadAllText(file);
                        sources.Add(source);
                    }
                    catch (System.IO.IOException) {
                        System.Console.WriteLine($"Can't read file: {file}");
                        return;
                    }
                }
            }

            var files = new List<FileSyntax>();
            {
                var index = 0;
                foreach (string source in sources)
                {
                    var fileName = System.IO.Path.GetFileName(options.files[index]);
                    try
                    {
                        var file = parseFile(fileName, source);
                        files.Add(file);
                        index++;
                    }
                    catch (ParserException e) {
                        System.Console.WriteLine($"Syntax error in {fileName} at {e.line}, {e.column}");
                        return;
                    }
                }
            }

            var program = new ProgramSyntax { files = files.ToArray() };
        }

        static FileSyntax parseFile(string file_name, string text)
        {
            var parser = new Parser(text);

            try
            {
                var file = parser.parse_file(file_name);
                return file;
            }
            catch (ParserException e)
            {
                throw new CompilerException(file_name, e.line, e.column);
            }
        }
    }
}