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
            var @int = LLVMTypeRef.Int32;
            var @char = LLVMTypeRef.Int8;
            var pChar = LLVMTypeRef.CreatePointer(@char, 0);
            var ppChar = LLVMTypeRef.CreatePointer(pChar, 0);
            var mainType = LLVMTypeRef.CreateFunction(@int, new LLVMTypeRef[] { @int, ppChar });
            var function = module.AddFunction("JitMain", mainType);
            var block = function.AppendBasicBlock(string.Empty);
            var builder = module.Context.CreateBuilder();
            builder.PositionAtEnd(block);
            var argc = function.Params[0];
            builder.BuildRet(argc);
            module.Verify(LLVMVerifierFailureAction.LLVMPrintMessageAction);
            LLVM.InitializeNativeTarget();
            var engine = module.CreateMCJITCompiler();
            LLVM.InitializeNativeAsmParser();
            LLVM.InitializeNativeAsmPrinter();
            var jitMain = engine.GetPointerToGlobal<Main>(function);
            return jitMain;
        }
    }
}