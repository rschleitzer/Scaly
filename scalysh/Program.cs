using System;

namespace scalysh
{
    class Program
    {
        static int Main(string[] arguments)
        {
            try
            {
                Options options = Options.ParseArguments(arguments);
            }
            catch (InvalidOptionException e)
            {
                var message = $"Invalid option {e.Option}\n";
                try { System.Console.WriteLine(message); } catch { return -1; }
                return -2;
            }
            catch (NoOutputOptionException e)
            {
                try { System.Console.WriteLine("No output option."); } catch { return -3; }
                return -4;
            }
            catch (NoFilesToCompileException e)
            {
                try { System.Console.WriteLine("No files to compile."); } catch { return -4; }
                return -5;
            }

            Compiler.compileFiles(options);

            return 0;
        }
    }
}
