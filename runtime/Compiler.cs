using System.Collections.Generic;

namespace Scaly.Compiler
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var definition = Modeler.BuildFiles(options.files);
        }

        public static int compileAndRunProgram(string program, string[] arguments)
        {
            var definition = Modeler.BuildProgram(program);
            var main = Generator.JitProgram(definition);
            int ret = main(arguments.Length, arguments);
            return ret;
        }
    }
}