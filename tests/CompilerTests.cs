using Microsoft.VisualStudio.TestTools.UnitTesting;
using Scaly.Compiler;

namespace tests
{
    [TestClass]
    public class CompilerTests
    {
        [TestMethod]
        public void TestShortest()
        {
            Compiler.compileProgram("0");
        }
    }
}
