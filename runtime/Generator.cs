using LLVMSharp.Interop;
using System;
using System.Collections.Generic;

namespace Scaly.Compiler
{
    internal class Generator
    {
        public delegate int Main(int argc, string[] argv);

        internal static Main JitProgram(Definition definition)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName("JIT module");
            Generate(module, definition);
            VerifyAndInitialize(module);
            var engine = module.CreateMCJITCompiler();
            var function = module.GetNamedFunction("main");
            var jitMain = engine.GetPointerToGlobal<Main>(function);
            return jitMain;
        }

        internal static void GenerateProgram(Definition definition, string outputName)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName(string.Empty);
            Generate(module, definition);

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

        static void Generate(LLVMModuleRef module, Definition definition)
        {
            foreach (var source in definition.Sources)
            {
                GenerateSource(module, source);
            }

            if (definition.Functions != null)
            {
                foreach (var functions in definition.Functions)
                {
                    foreach (var function in functions.Value)
                    {
                        GenerateFunction(module, function);
                    }
                }
            }
            //var function = MakeMainFunction(module);
            //LLVMBuilderRef builder = CreateBuilder(module, function);

            ////var argc = function.Params[0];
            //var argc = LLVMValueRef.CreateConstInt(LLVMTypeRef.CreateInt(32), 0);
            //builder.BuildRet(argc);
        }

        static void GenerateSource(LLVMModuleRef module, Source source)
        {
            if (source.Functions != null)
            {
                foreach (var functions in source.Functions)
                {
                    foreach (var function in functions.Value)
                    {
                        GenerateFunction(module, function);
                    }
                }
            }
        }

        static void GenerateFunction(LLVMModuleRef module, Function function)
        {
            var returnType = GenerateSingleType(function.Routine.Result);
            var parameterTypes = GenerateTypes(function.Routine.Input);
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes);
            var llvmFunction = module.AddFunction(function.Name, functionType);
            BuildFunction(module, llvmFunction, function.Routine.Operations);
        }

        static void BuildFunction(LLVMModuleRef module, LLVMValueRef llvmFunction, List<Operation> operations)
        {
            LLVMBuilderRef builder = CreateBuilder(module, llvmFunction);
            LLVMValueRef valueRef = null;
            foreach (var operation in operations)
            {
                valueRef = BuildOperation(builder, operation);
            }
            builder.BuildRet(valueRef);
        }

        static LLVMValueRef BuildOperation(LLVMBuilderRef builder, Operation operation)
        {
            LLVMValueRef valueRef = null;
            foreach (var operand in operation.Operands)
            {
                switch (operand.Expression)
                {
                    case IntegerConstant integerConstant:
                        valueRef = LLVMValueRef.CreateConstInt(LLVMTypeRef.Int32, (ulong)integerConstant.Value);
                        break;
                    case Name name:
                        valueRef = BuildName(builder, name);
                        break;
                    default:
                        throw new NotImplementedException($"Expression {operand.Expression.GetType()} not implemented.");
                }
            }
            return valueRef;
        }

        static LLVMValueRef BuildName(LLVMBuilderRef builder, Name name)
        {
            throw new NotImplementedException();
        }

        static LLVMTypeRef GenerateSingleType(List<Property> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return GenerateType(result[0].TypeSpec);
        }

        static LLVMTypeRef GenerateType(TypeSpec typeSpec)
        {
            switch (typeSpec.Name)
            {
                case "Integer":
                    return LLVMTypeRef.Int32;
                case "Byte":
                    return LLVMTypeRef.Int8;
                case "Pointer":
                    return GeneratePointerType(typeSpec);
                default:
                    throw new NotImplementedException($"Unknown type {typeSpec.Name}");
            }
        }

        static LLVMTypeRef GeneratePointerType(TypeSpec typeSpec)
        {
            var firstGenericArgument = typeSpec.Arguments[0];
            var pointerTarget = (firstGenericArgument.Name == "Pointer")? GeneratePointerType(firstGenericArgument): GenerateType(firstGenericArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static LLVMTypeRef[] GenerateTypes(List<Property> result)
        {
            if (result == null)
                return new LLVMTypeRef[] { };

            return result.ConvertAll(it => GenerateType(it.TypeSpec)).ToArray();

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