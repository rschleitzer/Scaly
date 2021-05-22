using Microsoft.VisualStudio.TestTools.UnitTesting;
using Scaly.Compiler;

namespace tests
{
    //[TestClass]
    public class CqlTests
    {
        //[TestMethod]
        public void FirstNot()
        {
            Assert.AreEqual(Compiler.compileAndRunProgram("module CQL:use CQL.*:if not(true):1 else 0", new string[] { }), 0);
        }
    }
}
