using Scaly.Compiler;

namespace scalyc
{
    class scalyc
    {
        static int Main(string[] arguments)
        {
            Options options = null;
            try
            {
                options = Options.ParseArguments(arguments);
            }
            catch (InvalidOptionException e)
            {
                var message = $"Invalid option {e.Option}\n";
                try { System.Console.WriteLine(message); } catch { return -1; }
                return -2;
            }
            catch (NoOutputOptionException)
            {
                try { System.Console.WriteLine("No output option."); } catch { return -3; }
                return -4;
            }
            catch (NoFilesToCompileException)
            {
                try { System.Console.WriteLine("No files to compile."); } catch { return -4; }
                return -5;
            }

            try
            {
                if (options.files.Length > 0)
                    Compiler.compileFiles(options);
                else
                    Compiler.Repl();
            }
            catch (CompilerException e)
            {
                var message = $"Compiler error in file {e.Span.file} at {e.Span.start.line},{e.Span.start.column}-{e.Span.end.line},{e.Span.end.column}.";
                try { System.Console.WriteLine(message); } catch { return -4; }
            }
            return 0;
        }
    }
}
