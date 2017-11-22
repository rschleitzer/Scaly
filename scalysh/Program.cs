namespace scalysh
{
    class scalysh
    {
        static int Main(string[] arguments)
        {
            try
            {
                Options options = Options.ParseArguments(arguments);
                Compiler.compileFiles(options);
                return 0;
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
        }
    }
}
