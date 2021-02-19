using System.Collections.Generic;

namespace Scaly.Compiler
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var definition = Modeler.BuildFiles(options.files);
        }

        public static void compileProgram(string program)
        {
            var module = Modeler.BuildProgram(program);
        }
    }
}