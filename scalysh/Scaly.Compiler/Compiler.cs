using System.Collections.Generic;

namespace scalyc
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var module = Modeler.Build(options.files);
        }
    }
}