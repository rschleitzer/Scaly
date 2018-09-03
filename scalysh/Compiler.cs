using System.Collections.Generic;

namespace scalysh
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

            var files = new List<File>();
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

            var program = new Program(options.outputName, files.ToArray());
            foreach (var item in files)
                item.parent = program;

            var modelVisitor = new CVisitor();
            program.accept(modelVisitor);
        }

        static File parseFile(string fileName, string text)
        {
            var parser = new Parser(fileName, text);

            try
            {
                var file = parser.parseFile();
                file.fileName = fileName;
                return file;
            }
            catch (ParserException e)
            {
                throw new CompilerException(fileName, e.line, e.column);
            }
        }
    }
}