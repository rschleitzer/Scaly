using System.Collections.Generic;
using System.IO;

namespace scalysh
{
    public class Compiler
    {
        public static void CompileFiles(Options options) {
            var files = options.files;
            var sources = new List<string>();
            foreach (string file in files)
            {
                try
                {
                    var source = File.ReadAllText(file);
                    sources.Add(source);
                }
                catch (IOException) {
                    System.Console.WriteLine($"Can't read file: {file}");
                    return;
                }                    
            }

            var modules = new List<Module>();
            var index = 0;
            foreach (string source in sources) {
                var moduleName = Path.GetFileNameWithoutExtension(files[index]);
                try
                {
                    var module = parseUnit(moduleName, source);
                    modules.Add(module);
                    index++;
                }
                catch (ParserException e) {
                    System.Console.WriteLine($"Syntax error in {files[index]} at {e.line}, {e.column}");
                    return;
                }
            }

            var program = new Program(options.outputName, new List<Module>(modules));
            foreach (var item in modules)
                item.parent = program;

            var modelVisitor = new ModelVisitor();
            program.accept(modelVisitor);
            
            var model = modelVisitor.model;
            let msg: VarString$ = new VarString(model.name)
            msg.append("\n")
            let message: string$ = new string (msg)
            print(message) catch _ return
        }

        static function parseUnit(moduleName: string, text: string): Module throws CompilerError {
            mutable parser: Parser$ = new Parser(moduleName, text)

            let module: Module = parser.parseModule()
                catch ParserError.syntax(line: number, column: number) {
                    throw parser(line, column)
                }

            module
        }        
    }
}