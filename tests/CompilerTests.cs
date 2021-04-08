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
            Assert.AreEqual(Compiler.compileAndRunProgram("use pointer.*:use size.*:strlen('argument values'*) 'as int'", new string[] { "bogus" }), 5);
        }

        [TestMethod]
        public void TestBooleanLiterals()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("use bool.*:true  'as int'", new string[] { }), 1);
            Assert.AreEqual(Compiler.compileAndRunProgram("use bool.*:false 'as int'", new string[] { }), 0);
        }

        [TestMethod]
        public void TestIf()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("if true:1:else 0", new string[] { }), 1);
            Assert.AreEqual(Compiler.compileAndRunProgram("if false:1:else 0", new string[] { }), 0);
            Assert.AreEqual(Compiler.compileAndRunProgram("if true:0:else 1", new string[] { }), 0);
            Assert.AreEqual(Compiler.compileAndRunProgram("if false:0:else 1", new string[] { }), 1);
        }

        [TestMethod]
        public void TestEmptyClassBody()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("define A (this: bool) {}:0", new string[] { }), 0);
        }
    }
}
