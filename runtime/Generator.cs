using LLVMSharp.Interop;
using System;
using System.Collections.Generic;

namespace Scaly.Compiler
{
    internal class GlobalContext
    {
        public LLVMModuleRef Module;
        public LLVMBuilderRef Builder;
        public Dictionary<string, LLVMTypeRef> Types = new Dictionary<string, LLVMTypeRef>();
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();
        public bool finalPass;
    }

    internal class LocalContext
    {
        public GlobalContext GlobalContext;
        public LocalContext ParentContext;
        public Dictionary<string, LLVMTypeRef> Types = new Dictionary<string, LLVMTypeRef>();
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();

        public LLVMValueRef Resolve(string name)
        {
            if (Values.ContainsKey(name))
                return Values[name];

            if (ParentContext != null)
            {
                var valueRef = ParentContext.Resolve(name);
                if (valueRef != null)
                    return valueRef;
            }

            if (GlobalContext.Values.ContainsKey(name))
                return GlobalContext.Values[name];

            return null;
        }

        internal LLVMTypeRef ResolveType(string name, string file, Position start, Position end)
        {
            if (Types.ContainsKey(name))
                return Types[name];

            if (ParentContext != null)
            {
                return ParentContext.ResolveType(name, file, start, end);
            }
            else
            {
                if (GlobalContext.Types.ContainsKey(name))
                    return GlobalContext.Types[name];
            }

            throw new CompilerException($"The type {name} could not been found.", file, start.line, start.column, end.line, end.column);
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
                var context = new GlobalContext { Module = module, Builder = builder };
                BuildGlobalTypes(context, definition);
                context.finalPass = true;
                BuildGlobalValues(context, definition);
                BuildFunctions(context, definition);
            }
        }

        static void BuildGlobalTypes(GlobalContext context, Definition definition)
        {
            if (!context.finalPass)
            {
                context.Types.Add("Double", LLVMTypeRef.Double);
                context.Types.Add("Long", LLVMTypeRef.Int64);
                context.Types.Add("Size", LLVMTypeRef.Int64);
                context.Types.Add("Integer", LLVMTypeRef.Int32);
                context.Types.Add("Byte", LLVMTypeRef.Int8);
                context.Types.Add("Boolean", LLVMTypeRef.Int8);
            }
            var localContext = new LocalContext { GlobalContext = context, ParentContext = null };
            BuildDefinitionTypes(localContext, definition);
        }

        static void BuildDefinitionTypes(LocalContext context, Definition definition)
        {
            if (definition.Sources != null)
                foreach (var source in definition.Sources)
                    BuildSourceTypes(context, source);

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionType(context, function);

            if (definition.Operators != null)
                foreach (var @operator in definition.Operators)
                    BuildOperatorType(context, @operator);
        }

        static void BuildSourceTypes(LocalContext context, Source source)
        {
            if (source.Definitions != null)
                foreach (var definition in source.Definitions.Values)
                    BuildDefinitionTypes(context, definition);

            if (source.Functions != null)
                foreach (var function in source.Functions)
                    BuildFunctionType(context, function);

            if (source.Operators != null)
                foreach (var @operator in source.Operators)
                    BuildOperatorType(context, @operator);

            if (source.Sources != null)
                foreach (var childSource in source.Sources)
                    BuildSourceTypes(context, childSource);
        }

        static void BuildFunctionType(LocalContext context, Function function)
        {
            var returnType = GetSingleType(context, function.Routine.Result);
            var parameterTypes = GetTypes(context, function.Routine.Input);
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes);
            context.GlobalContext.Types.Add(function.Name, functionType);
        }

        static void BuildOperatorType(LocalContext context, Operator @operator)
        {
            var returnType = GetSingleType(context, @operator.Routine.Result);
            var parameterTypes = new List<LLVMTypeRef>();
            if (@operator.Routine.Input != null)
                parameterTypes.AddRange(GetTypes(context, @operator.Routine.Input));
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes.ToArray());
            context.GlobalContext.Types.Add(@operator.Name, functionType);
        }

        static void BuildGlobalValues(GlobalContext context, Definition definition)
        {
            var localContext = new LocalContext { GlobalContext = context, ParentContext = null };
            BuildValues(localContext, definition);
        }

        static void BuildValues(LocalContext context, Definition definition)
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

        static void BuildSourceValues(LocalContext context, Source source)
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

        static void BuildFunctionValue(LocalContext context, Function function)
        {
            var functionType = context.ResolveType(function.Name, function.Syntax.file, function.Syntax.start, function.Syntax.end);
            context.GlobalContext.Values.Add(function.Name, context.GlobalContext.Module.AddFunction(function.Name, functionType));
        }

        static void BuildOperatorValue(LocalContext context, Operator @operator)
        {
            var functionType = context.ResolveType(@operator.Name, @operator.Syntax.file, @operator.Syntax.start, @operator.Syntax.end);
            context.GlobalContext.Values.Add(@operator.Name, context.GlobalContext.Module.AddFunction(@operator.Name, functionType));
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
            var llvmFunction = context.Values[function.Name];
            var block = llvmFunction.AppendBasicBlock(string.Empty);
            context.Builder.PositionAtEnd(block);
            var localContext = new LocalContext { GlobalContext = context, ParentContext = null };
            uint paramCount = 0;
            foreach (var parameter in function.Routine.Input)
            {
                localContext.Values.Add(parameter.Name, llvmFunction.GetParam(paramCount));
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
            var newContext = new LocalContext { GlobalContext = context.GlobalContext, ParentContext = context };
            newContext.Values.Add(binding.Identifier, BuildOperation(context, binding.Operation));
            LLVMValueRef valueRef = null;
            foreach (var operation in binding.Operations)
                valueRef = BuildOperation(newContext, operation);

            return valueRef;
        }

        static LLVMTypeRef GetSingleType(LocalContext context, List<Property> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return GetType(context, result[0].TypeSpec);
        }

        static LLVMTypeRef GetType(LocalContext context, TypeSpec typeSpec)
        {
            if (typeSpec.Name == "Pointer")
                return GetPointerType(context, typeSpec);

            return context.ResolveType(typeSpec.Name, typeSpec.Syntax.file, typeSpec.Syntax.start, typeSpec.Syntax.end);
        }

        static LLVMTypeRef GetPointerType(LocalContext context, TypeSpec typeSpec)
        {
            var firstGenericArgument = typeSpec.Arguments[0];
            var pointerTarget = (firstGenericArgument.Name == "Pointer") ? GetPointerType(context, firstGenericArgument): GetType(context, firstGenericArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static LLVMTypeRef[] GetTypes(LocalContext context, List<Property> result)
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