using LLVMSharp.Interop;
using System;
using System.Collections.Generic;

namespace Scaly.Compiler
{
    internal class TypeStore
    {
        public Dictionary<string, LLVMTypeRef> DataTypes = new Dictionary<string, LLVMTypeRef>();
        public Dictionary<string, LLVMTypeRef> FunctionTypes = new Dictionary<string, LLVMTypeRef>();
    }

    internal class ValueStore
    {
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();
    }

    internal class GlobalContext
    {
        public LLVMModuleRef Module;
        public LLVMBuilderRef Builder;
        public TypeStore TypeStore;
        public ValueStore ValueStore;
    }

    internal class LocalContext
    {
        public GlobalContext GlobalContext;
        public LocalContext ParentContext;
        public Dictionary<string, LLVMValueRef> Data = new Dictionary<string, LLVMValueRef>();

        public LLVMValueRef Resolve(string name)
        {
            if (Data.ContainsKey(name))
                return Data[name];

            if (ParentContext != null)
            {
                var valueRef = ParentContext.Resolve(name);
                if (valueRef != null)
                    return valueRef;
            }

            if (GlobalContext.ValueStore.Values.ContainsKey(name))
                return GlobalContext.ValueStore.Values[name];

            return null;
        }
    }


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
            using (var builder = module.Context.CreateBuilder())
            {
                var context = new GlobalContext { Module = module, Builder = builder, TypeStore = new TypeStore(), ValueStore = new ValueStore() };
                BuildTypes(context, definition);
                BuildValues(context, definition);
                BuildFunctions(context, definition);
            }
            //var function = MakeMainFunction(module);
            //LLVMBuilderRef builder = CreateBuilder(module, function);

            ////var argc = function.Params[0];
            //var argc = LLVMValueRef.CreateConstInt(LLVMTypeRef.CreateInt(32), 0);
            //builder.BuildRet(argc);
        }

        //static LLVMValueRef MakeMainFunction(LLVMModuleRef module)
        //{
        //    var @int = LLVMTypeRef.Int32;
        //    var @char = LLVMTypeRef.Int8;
        //    var pChar = LLVMTypeRef.CreatePointer(@char, 0);
        //    var ppChar = LLVMTypeRef.CreatePointer(pChar, 0);
        //    var mainType = LLVMTypeRef.CreateFunction(@int, new LLVMTypeRef[] { @int, ppChar });
        //    var function = module.AddFunction("main", mainType);
        //    return function;
        //}

        static void BuildTypes(GlobalContext context, Definition definition)
        {
            context.TypeStore.DataTypes.Add("Double", LLVMTypeRef.Double);
            context.TypeStore.DataTypes.Add("Long", LLVMTypeRef.Int64);
            context.TypeStore.DataTypes.Add("Size", LLVMTypeRef.Int64);
            context.TypeStore.DataTypes.Add("Integer", LLVMTypeRef.Int32);
            context.TypeStore.DataTypes.Add("Byte", LLVMTypeRef.Int8);
            context.TypeStore.DataTypes.Add("Boolean", LLVMTypeRef.Int8);

            foreach (var source in definition.Sources)
                BuildSourceTypes(context, source);

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionType(context, function);
        }

        static void BuildSourceTypes(GlobalContext context, Source source)
        {
            if (source.Functions != null)
                foreach (var function in source.Functions)
                    BuildFunctionType(context, function);

            if (source.Sources != null)
                foreach (var childSource in source.Sources)
                    BuildSourceTypes(context, childSource);
        }

        static void BuildFunctionType(GlobalContext context, Function function)
        {
            var returnType = GetSingleType(context, function.Routine.Result);
            var parameterTypes = GetTypes(context, function.Routine.Input);
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes);
            context.TypeStore.FunctionTypes.Add(function.Name, functionType);
        }

        static void BuildValues(GlobalContext context, Definition definition)
        {
            if (definition.Sources != null)
                foreach (var source in definition.Sources)
                    BuildSourceValues(context, source);

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionValue(context, function);

            if (definition.Operators != null)
                foreach (var @operator in definition.Operators)
                    BuildOperatorValue(context, @operator);
        }

        static void BuildSourceValues(GlobalContext context, Source source)
        {
            if (source.Functions != null)
                foreach (var function in source.Functions)
                    BuildFunctionValue(context, function);

            if (source.Operators != null)
                foreach (var @operator in source.Operators)
                    BuildOperatorValue(context, @operator);

            if (source.Sources != null)
                foreach (var childSource in source.Sources)
                    BuildSourceValues(context, childSource);

            if (source.Definitions != null)
                foreach (var definition in source.Definitions.Values)
                    BuildValues(context, definition);
        }

        static void BuildFunctionValue(GlobalContext context, Function function)
        {
            var functionType = context.TypeStore.FunctionTypes[function.Name];
            context.ValueStore.Values.Add(function.Name, context.Module.AddFunction(function.Name, functionType));
        }

        static void BuildOperatorValue(GlobalContext context, Operator @operator)
        {
            var functionType = context.TypeStore.FunctionTypes[@operator.Name];
            context.ValueStore.Values.Add(@operator.Name, context.Module.AddFunction(@operator.Name, functionType));
        }

        static void BuildFunctions(GlobalContext context, Definition definition)
        {
            foreach (var source in definition.Sources)
            {
                BuildSourceFunctions(context, source);
            }

            if (definition.Functions != null)
            {
                foreach (var function in definition.Functions)
                {
                    BuildFunction(context, function);
                }
            }
        }

        static void BuildSourceFunctions(GlobalContext context, Source source)
        {
            if (source.Functions != null)
            {
                foreach (var function in source.Functions)
                {
                    BuildFunction(context, function);
                }
            }
        }

        static void BuildFunction(GlobalContext context, Function function)
        {
            var llvmFunction = context.ValueStore.Values[function.Name];
            var block = llvmFunction.AppendBasicBlock(string.Empty);
            context.Builder.PositionAtEnd(block);
            var localContext = new LocalContext { GlobalContext = context, ParentContext = null };
            uint paramCount = 0;
            foreach (var parameter in function.Routine.Input)
            {
                localContext.Data.Add(parameter.Name, llvmFunction.GetParam(paramCount));
                paramCount++;
            }
            LLVMValueRef valueRef = null;
            foreach (var operation in function.Routine.Operations)
                valueRef = BuildOperation(localContext, operation);
            context.Builder.BuildRet(valueRef);
        }

        static LLVMValueRef BuildOperation(LocalContext context, Operation operation)
        {
            LLVMValueRef valueRef = null;
            foreach (var operand in operation.Operands)
            {
                valueRef = BuildOperand(context, valueRef, operand);
            }
            return valueRef;
        }

        static LLVMValueRef BuildOperand(LocalContext context, LLVMValueRef valueRef, Operand operand)
        {
            switch (operand.Expression)
            {
                case IntegerConstant integerConstant:
                    valueRef = LLVMValueRef.CreateConstInt(LLVMTypeRef.Int32, (ulong)integerConstant.Value);
                    break;
                case Name name:
                    valueRef = BuildName(context, name);
                    break;
                case Scope scope:
                    valueRef = BuildScope(context, scope);
                    break;
                case Object @object:
                    if (valueRef == null)
                        throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Syntax.file, @object.Syntax.start.line, @object.Syntax.start.column, @object.Syntax.end.line, @object.Syntax.end.column);
                    valueRef = BuildFunctionCall(context, valueRef, @object);
                    break;
                default:
                    throw new NotImplementedException($"Expression {operand.Expression.GetType()} not implemented.");
            }
            return valueRef;
        }

        static LLVMValueRef BuildFunctionCall(LocalContext context, LLVMValueRef function, Object @object)
        {
            var arguments = new List<LLVMValueRef>();
            foreach (var component in @object.Components)
            {
                LLVMValueRef valueRef = null;
                foreach (var operand in component.Value)
                {
                    valueRef = BuildOperand(context, valueRef, operand);
                }
                arguments.Add(valueRef);
            }
            return context.GlobalContext.Builder.BuildCall(function, arguments.ToArray());
        }

        static LLVMValueRef BuildName(LocalContext context, Name name)
        {
            var valueRef = context.Resolve(name.Path[0]);
            if (valueRef != null)
                return valueRef;

            throw new CompilerException($"The name '{name.Path[0]}' could not been found.", name.Syntax.file, name.Syntax.start.line, name.Syntax.start.column, name.Syntax.end.line, name.Syntax.end.column);
        }

        static LLVMValueRef BuildScope(LocalContext context, Scope scope)
        {
            LLVMValueRef valueRef = null;
            foreach (var operation in scope.Operations)
                valueRef = BuildOperation(context, operation);

            if (scope.Binding != null)
                valueRef = BuildBinding(context, scope.Binding);

            return valueRef;
        }

        static LLVMValueRef BuildBinding(LocalContext context, Binding binding)
        {
            var newContext = new LocalContext { GlobalContext = context.GlobalContext, ParentContext = context, Data = new Dictionary<string, LLVMValueRef>() };
            newContext.Data.Add(binding.Identifier, BuildOperation(context, binding.Operation));
            LLVMValueRef valueRef = null;
            foreach (var operation in binding.Operations)
                valueRef = BuildOperation(newContext, operation);

            return valueRef;
        }

        static LLVMTypeRef GetSingleType(GlobalContext context, List<Property> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return GetType(context, result[0].TypeSpec);
        }

        static LLVMTypeRef GetType(GlobalContext context, TypeSpec typeSpec)
        {
            if (typeSpec.Name == "Pointer")
                    return GetPointerType(context, typeSpec);

            if (context.TypeStore.DataTypes.ContainsKey(typeSpec.Name))
                return context.TypeStore.DataTypes[typeSpec.Name];

            throw new CompilerException($"The type {typeSpec.Name} could not been found.", typeSpec.Syntax.file, typeSpec.Syntax.start.line, typeSpec.Syntax.start.column, typeSpec.Syntax.end.line, typeSpec.Syntax.end.column);
        }

        static LLVMTypeRef GetPointerType(GlobalContext context, TypeSpec typeSpec)
        {
            var firstGenericArgument = typeSpec.Arguments[0];
            var pointerTarget = (firstGenericArgument.Name == "Pointer")? GetPointerType(context, firstGenericArgument): GetType(context, firstGenericArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static LLVMTypeRef[] GetTypes(GlobalContext context, List<Property> result)
        {
            if (result == null)
                return new LLVMTypeRef[] { };

            return result.ConvertAll(it => GetType(context, it.TypeSpec)).ToArray();

        }

        static void VerifyAndInitialize(LLVMModuleRef module)
        {
            module.Verify(LLVMVerifierFailureAction.LLVMPrintMessageAction);
            LLVM.InitializeNativeTarget();
            LLVM.InitializeNativeAsmParser();
            LLVM.InitializeNativeAsmPrinter();
        }
    }
}