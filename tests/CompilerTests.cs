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
            Assert.AreEqual(Compiler.compileAndRunProgram("use Boolean.*:true  'as Integer'", new string[] { }), 1);
            Assert.AreEqual(Compiler.compileAndRunProgram("use Boolean.*:false 'as Integer'", new string[] { }), 0);
        }

        [TestMethod]
        public void TestIf()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("if true:1:else 0", new string[] { }), 1);
            Assert.AreEqual(Compiler.compileAndRunProgram("if false:1:else 0", new string[] { }), 0);
            Assert.AreEqual(Compiler.compileAndRunProgram("if true:0:else 1", new string[] { }), 0);
            Assert.AreEqual(Compiler.compileAndRunProgram("if false:0:else 1", new string[] { }), 1);
        }
    }
}
