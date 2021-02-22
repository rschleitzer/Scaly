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
        }

        [TestMethod]
        public void TestReturnArgc()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("3", new string[] { "foo", "bar", "baz" }), 3);
        }
    }
}