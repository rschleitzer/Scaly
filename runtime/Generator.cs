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
            var function = MakeMainFunction(module);
            LLVMBuilderRef builder = CreateBuilder(module, function);

            var argc = function.Params[0];
            builder.BuildRet(argc);

            VerifyAndInitialize(module);
            var engine = module.CreateMCJITCompiler();
            var jitMain = engine.GetPointerToGlobal<Main>(function);
            return jitMain;
        }

        internal static void GenerateProgram(Definition definition, string outputName)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName(string.Empty);
            var function = MakeMainFunction(module);
            LLVMBuilderRef builder = CreateBuilder(module, function);

            var argc = function.Params[0];
            builder.BuildRet(argc);

            VerifyAndInitialize(module);
            LLVM.InitializeAllTargetInfos();
            var triple = LLVMTargetRef.DefaultTriple;
            LLVMTargetRef target = LLVMTargetRef.GetTargetFromTriple(triple);
            LLVMTargetMachineRef targetMachine = target.CreateTargetMachine(triple, "", "",
                LLVMCodeGenOptLevel.LLVMCodeGenLevelDefault, LLVMRelocMode.LLVMRelocDefault,
                LLVMCodeModel.LLVMCodeModelDefault);
            var message = "";
            targetMachine.TryEmitToFile(module, $"{outputName}.o", LLVMCodeGenFileType.LLVMObjectFile, out message);
        }

        static void VerifyAndInitialize(LLVMModuleRef module)
        {
            module.Verify(LLVMVerifierFailureAction.LLVMPrintMessageAction);
            LLVM.InitializeNativeTarget();
            LLVM.InitializeNativeAsmParser();
            LLVM.InitializeNativeAsmPrinter();
        }

        static LLVMBuilderRef CreateBuilder(LLVMModuleRef module, LLVMValueRef function)
        {
            var builder = module.Context.CreateBuilder();
            var block = function.AppendBasicBlock(string.Empty);
            builder.PositionAtEnd(block);
            return builder;
        }

        static LLVMValueRef MakeMainFunction(LLVMModuleRef module)
        {
            var @int = LLVMTypeRef.Int32;
            var @char = LLVMTypeRef.Int8;
            var pChar = LLVMTypeRef.CreatePointer(@char, 0);
            var ppChar = LLVMTypeRef.CreatePointer(pChar, 0);
            var mainType = LLVMTypeRef.CreateFunction(@int, new LLVMTypeRef[] { @int, ppChar });
            var function = module.AddFunction("main", mainType);
            return function;
        }
    }
}