using LLVMSharp.Interop;
using System;
using System.Collections.Generic;
using System.Text;

namespace Scaly.Compiler
{
    internal class GlobalContext
    {
        public Definition Definition;
        public LLVMModuleRef Module;
        public LLVMBuilderRef Builder;
        public Dictionary<string, Definition> DefinitionDictionary = new Dictionary<string, Definition>();
        public Dictionary<string, Source> SourceDictionary = new Dictionary<string, Source>();
        public Dictionary<string, LLVMTypeRef> Types = new Dictionary<string, LLVMTypeRef>();
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();
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

        internal LLVMTypeRef ResolveType(string name)
        {
            if (GlobalContext.Types.ContainsKey(name))
                return GlobalContext.Types[name];

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
                var context = new GlobalContext { Definition = definition, Module = module, Builder = builder };
                var localContext = new LocalContext { GlobalContext = context, ParentContext = null };
                string path = null;
                BuildDefinitionDictionary(context, path, null, definition);
                BuildDefinitionTypes(localContext, null, definition);
                BuildDefinitionValues(localContext, definition);
                BuildFunctions(context, definition);
            }
        }

        static void BuildDefinitionDictionary(GlobalContext context, string parentPath, Source source, Definition definition)
        {
            var path = parentPath == null? "" : parentPath + (parentPath == "" ? "" : ".") + definition.Type.Name;
            if (path != "")
            {
                if (context.DefinitionDictionary.ContainsKey(path))
                    throw new CompilerException($"The definition {path} was already defined.", definition.Span);
                context.DefinitionDictionary.Add(path, definition);
                context.SourceDictionary.Add(path, source);
            }

            if (definition.Sources != null)
                foreach (var childSource in definition.Sources)
                    BuildSourceDefinitionDictionary(context, path, childSource);

            if (definition.Definitions != null)
                foreach (var childDefinition in definition.Definitions)
                    BuildDefinitionDictionary(context, path, source, childDefinition.Value);
        }

        static void BuildSourceDefinitionDictionary(GlobalContext context, string path, Source source)
        {
            if (source.Sources != null)
                foreach (var childSource in source.Sources)
                    BuildSourceDefinitionDictionary(context, path, childSource);

            if (source.Definitions != null)
                foreach (var definition in source.Definitions)
                    BuildDefinitionDictionary(context, path, source, definition.Value);
        }

        static void BuildDefinitionTypes(LocalContext context, Source source, Definition definition)
        {
            if (definition.Sources != null)
                foreach (var definitionSource in definition.Sources)
                    BuildSourceTypes(context, definitionSource);

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionType(context, source, function);

            if (definition.Operators != null)
                foreach (var @operator in definition.Operators)
                    BuildOperatorType(context, source, @operator);
        }

        static void BuildSourceTypes(LocalContext context, Source source)
        {
            if (source.Definitions != null)
            {
                foreach (var definition in source.Definitions.Values)
                {
                    if (definition.Type.Arguments == null)
                        BuildDefinitionTypes(context, source, definition);
                }
            }

            if (source.Functions != null)
                foreach (var function in source.Functions)
                    BuildFunctionType(context, source, function);

            if (source.Operators != null)
                foreach (var @operator in source.Operators)
                    BuildOperatorType(context, source, @operator);

            if (source.Sources != null)
                foreach (var childSource in source.Sources)
                    BuildSourceTypes(context, childSource);
        }

        static void BuildFunctionType(LocalContext context, Source source, Function function)
        {
            var returnType = GetSingleType(context, source, function.Routine.Result);
            var parameterTypes = GetTypes(context, source, function.Routine.Input);
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes);
            context.GlobalContext.Types.Add(function.Name, functionType);
        }

        static void BuildOperatorType(LocalContext context, Source source, Operator @operator)
        {
            var returnType = GetSingleType(context, source, @operator.Routine.Result);
            var parameterTypes = new List<LLVMTypeRef>();
            if (@operator.Routine.Input != null)
                parameterTypes.AddRange(GetTypes(context, source, @operator.Routine.Input));
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes.ToArray());
            context.GlobalContext.Types.Add(@operator.Name, functionType);
        }

        static void BuildDefinitionValues(LocalContext context, Definition definition)
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
            {
                foreach (var definition in source.Definitions.Values)
                {
                    if (definition.Type.Arguments == null)
                        BuildDefinitionValues(context, definition);
                }
            }
        }

        static void BuildFunctionValue(LocalContext context, Function function)
        {
            var functionType = context.ResolveType(function.Name);
            context.GlobalContext.Values.Add(function.Name, context.GlobalContext.Module.AddFunction(function.Name, functionType));
        }

        static void BuildOperatorValue(LocalContext context, Operator @operator)
        {
            var functionType = context.ResolveType(@operator.Name);
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
                        throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Span);
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

            throw new CompilerException($"The name '{name.Path[0]}' has not been found.", name.Span);
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

        static LLVMTypeRef GetSingleType(LocalContext context, Source source, List<Parameter> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return GetType(context, source, result[0].TypeSpec);
        }

        static LLVMTypeRef GetType(LocalContext context, Source source, TypeSpec typeSpec)
        {
            if (typeSpec.Name == "Pointer")
                return GetPointerType(context, source, typeSpec);

            var qualifiedName = QualifyName(context, source, typeSpec.Name);
            var typeName = AddTypeArguments(qualifiedName, typeSpec.Arguments);
            var type = context.ResolveType(typeName);

            if (type == null)
            {
                type = CreateType(context, qualifiedName, context.GlobalContext.SourceDictionary[qualifiedName], typeSpec);
            }

            return type;
        }

        static LLVMTypeRef CreateType(LocalContext context, string qualifiedName, Source source, TypeSpec typeSpec)
        {
            if (!context.GlobalContext.DefinitionDictionary.ContainsKey(qualifiedName))
                throw new CompilerException($"Tht type \"{qualifiedName}\" was not found.", typeSpec.Span);
            var definition = context.GlobalContext.DefinitionDictionary[qualifiedName];
 
            if (typeSpec.Arguments != null)
            {
                if (definition.Type.Arguments != null)
                {
                    if (typeSpec.Arguments.Count != definition.Type.Arguments.Count)
                        throw new CompilerException($"The generic type {qualifiedName} needs {definition.Type.Arguments.Count} type arguments, but {typeSpec.Arguments.Count} type arguments were given.", typeSpec.Span);
                }
                else
                {
                    throw new CompilerException($"The type {qualifiedName} is not a generic type.", typeSpec.Span);
                }
            }
            else
            {
                if (definition.Type.Arguments != null)
                {
                    throw new CompilerException($"The type {qualifiedName} is a generic type. Generic arguments are missing.", typeSpec.Span);
                }
            }

            return CreateGenericType(context, source, typeSpec, definition);
        }

        static LLVMTypeRef CreateGenericType(LocalContext context, Source source, TypeSpec typeSpec, Definition definition)
        {
            if (definition.IsIntrinsic)
                return CreateInstrinsicType(definition.Type.Name, typeSpec.Span);

            if (definition.Structure == null)
                throw new CompilerException($"The type \"{definition.Type.Name}\" contains no data and can only be used as a namespace.", typeSpec.Span);

            if (definition.Structure.Members.Count == 1 && definition.Structure.Members[0].Name == null && definition.Structure.Members[0].Type.Arguments == null)
                return GetType(context, source, definition.Structure.Members[0].Type);

            return null;
        }

        static LLVMTypeRef CreateInstrinsicType(string name, Span span)
        {
            switch (name)
            {
                case "i1":
                    return LLVMTypeRef.Int1;
                case "i8":
                    return LLVMTypeRef.Int8;
                case "i32":
                    return LLVMTypeRef.Int32;
                case "i64":
                    return LLVMTypeRef.Int64;
                default:
                    throw new CompilerException($"The intrinsic type \"{name}\" is unknown.", span);
            }
        }

        static string QualifyName(LocalContext context, Source source, string name)
        {
            if (context.GlobalContext.Types.ContainsKey(name))
                return name;

            if (context.GlobalContext.DefinitionDictionary.ContainsKey(name))
                return name;

            if (source.Uses != null && source.Uses.ContainsKey(name))
                return source.Uses[name].Path;

            if (source.Usings != null)
            {
                foreach (var @using in source.Usings)
                {
                    var usingName = @using.Path;
                    if (!context.GlobalContext.DefinitionDictionary.ContainsKey(usingName))
                        throw new CompilerException($"The name space {usingName} was not found.", @using.Span);

                    var nameSpace = context.GlobalContext.DefinitionDictionary[usingName];
                    if (nameSpace.Definitions.ContainsKey(name))
                        return usingName + "." + name;
                }
            }

            return null;
        }

        static string AddTypeArguments(string name, List<TypeSpec> arguments)
        {
            if (arguments == null)
                return name;

            StringBuilder builder = new StringBuilder(name);
            builder.Append('[');
            bool first = true;
            foreach (var argument in arguments)
            {
                if (first)
                    first = false;
                else
                    builder.Append(',');
                builder.Append(AddTypeArguments(argument.Name, argument.Arguments));
            }
            name += "]";

            return name;
        }

        static LLVMTypeRef GetPointerType(LocalContext context, Source source, TypeSpec typeSpec)
        {
            var firstGenericArgument = typeSpec.Arguments[0];
            var pointerTarget = (firstGenericArgument.Name == "Pointer") ? GetPointerType(context, source, firstGenericArgument): GetType(context, source, firstGenericArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static LLVMTypeRef[] GetTypes(LocalContext context, Source source, List<Parameter> result)
        {
            if (result == null)
                return new LLVMTypeRef[] { };

            return result.ConvertAll(it => GetType(context, source, it.TypeSpec)).ToArray();

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