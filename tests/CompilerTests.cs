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
            Assert.AreEqual(Compiler.compileAndRunProgram("42", new string[] { }), 42);
        }

        [TestMethod]
        public void TestArgc()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("'argument count'", new string[] { "foo", "bar", "baz" }), 3);
        }

        [TestMethod]
        public void TestStrlen()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("use Pointer.*:use Size.*:strlen('argument values'*) 'as Integer'", new string[] { "bogus" }), 5);
        }

        [TestMethod]
        public void TestBooleanLiterals()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("true", new string[] { }), 1);
            Assert.AreEqual(Compiler.compileAndRunProgram("false", new string[] { }), 0);
        }
    }
}
