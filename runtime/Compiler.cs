using System.Collections.Generic;

namespace Scaly.Compiler
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var module = Modeler.BuildFiles(options.files);
        }
    }
}