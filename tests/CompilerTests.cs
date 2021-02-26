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
            Assert.AreEqual(Compiler.compileAndRunProgram("0", new string[] { }), 0);
            Assert.AreEqual(Compiler.compileAndRunProgram("'argument count'", new string[] { "foo", "bar", "baz" }), 3);
        }
    }
}
