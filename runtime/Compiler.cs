using System.Collections.Generic;

namespace Scaly.Compiler
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var definition = Modeler.BuildFiles(options.files);
            Generator.GenerateProgram(definition, options.outputName);
        }

        public static int compileAndRunProgram(string program, string[] arguments)
        {
            var source = Modeler.BuildProgram(program);
            var runtime = Modeler.BuildRuntime();
            var main = Generator.JitProgram(new Definition { Sources = new List<Source> { source, runtime } });
            int ret = main(arguments.Length, arguments);
            return ret;
        }
    }
}