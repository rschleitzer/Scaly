using LLVMSharp.Interop;
using System;

namespace Scaly.Compiler
{
    internal class Generator
    {
        public delegate int Main(int argc, string[] argv);

        internal static Main JitProgram(Definition definition)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName("JIT module");
            module.AddFunction("main", LLVMTypeRef.Int32);
            return null;
        }
    }
}