﻿using LLVMSharp.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Scaly.Compiler
{
    internal class NameDictionary
    {
        public Dictionary<string, Definition> Definitions = new Dictionary<string, Definition>();
        public Dictionary<string, Source> Sources = new Dictionary<string, Source>();
    }

    internal class DictionaryContext
    {
        public NameDictionary Dictionary;
        public Source Source;
        public string Path;
        public Definition Definition;
    }

    internal class GlobalContext
    {
        public LLVMModuleRef Module;
        public NameDictionary Dictionary;
        public Dictionary<string, LLVMTypeRef> Types = new Dictionary<string, LLVMTypeRef>();
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();
    }

    internal class Context
    {
        public GlobalContext Global;
        public Source Source;
        public string Path = "";
        public Context ParentContext;
        public Dictionary<string, LLVMValueRef> Values = new Dictionary<string, LLVMValueRef>();

        public LLVMValueRef ResolveValue(string name)
        {
            if (Values.ContainsKey(name))
                return Values[name];

            if (ParentContext != null)
            {
                var valueRef = ParentContext.ResolveValue(name);
                if (valueRef != null)
                    return valueRef;
            }

            if (Global.Values.ContainsKey(name))
                return Global.Values[name];

            return null;
        }

        internal LLVMTypeRef ResolveType(string name)
        {
            if (Global.Types.ContainsKey(name))
                return Global.Types[name];

            return null;
        }

        internal Definition ResolveName(string name, Span span)
        {
            if (!Global.Dictionary.Definitions.ContainsKey(name))
                throw new CompilerException($"Tht type \"{name}\" was not found.", span);

            return Global.Dictionary.Definitions[name];
        }

        internal TypeSpec ResolveFunctionTypeSpec(string name)
        {
            return null;
        }
    }


    internal class Generator
    {
        public delegate int Main(int argc, string[] argv);

        internal static Main JitProgram(List<Source> sources)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName("JIT module");
            Generate(module, sources);
            VerifyAndInitialize(module);
            var engine = module.CreateMCJITCompiler();
            var function = module.GetNamedFunction("main");
            var jitMain = engine.GetPointerToGlobal<Main>(function);
            return jitMain;
        }

        internal static void GenerateProgram(List<Source> sources, string outputName)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName(string.Empty);
            Generate(module, sources);

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

        static void Generate(LLVMModuleRef module, List<Source> sources)
        {
            var dictionary = new NameDictionary();
            {
                var context = new DictionaryContext { Dictionary = dictionary, Path = "" };
                foreach (var source in sources)
                {
                    context.Source = source;
                    BuildSourceDictionary(context);
                }
            }

            {
                var globalContext = new GlobalContext { Module = module, Dictionary = dictionary };
                var context = new Context { Global = globalContext, ParentContext = null };
                foreach (var source in sources)
                {
                    context.Source = source;
                    BuildSourceFunctions(context);
                }
            }

            //BuildDefinitionTypes(localContext, definition);
            //BuildDefinitionValues(localContext, definition);
            //BuildFunctions(globalContext, null, definition);
        }

        static void BuildSourceFunctions(Context context)
        {
            if (context.Source.Functions != null)
            {
                foreach (var function in context.Source.Functions)
                {
                    BuildFunction(context, function);
                }
            }
        }

        static void BuildFunction(Context context, Function function)
        {
            using (var builder = context.Global.Module.Context.CreateBuilder())
            {
                var llvmFunction = ResolveFunctionValue(context, function);
                var block = llvmFunction.AppendBasicBlock(string.Empty);
                builder.PositionAtEnd(block);
                uint paramCount = 0;
                foreach (var parameter in function.Routine.Input)
                {
                    context.Values.Add(parameter.Name, llvmFunction.GetParam(paramCount));
                    paramCount++;
                }
                LLVMValueRef valueRef = null;
                foreach (var operation in function.Routine.Operations)
                    valueRef = BuildOperands(context, builder, operation.Operands);
                builder.BuildRet(valueRef);
            }
        }

        static LLVMValueRef ResolveFunctionValue(Context context, Function function)
        {
            var functionName = QualifyFunctionName(context, function);
            if (!context.Global.Values.ContainsKey(functionName))
                BuildFunctionValue(context, function);
            var llvmFunction = context.Global.Values[function.Name];
            return llvmFunction;
        }

        static void BuildFunctionValue(Context context, Function function)
        {
            var functionType = ResolveFunctionType(context, function);
            var functionName = QualifyFunctionName(context, function);
            context.Global.Values.Add(function.Name, context.Global.Module.AddFunction(functionName, functionType));
        }

        static LLVMTypeRef ResolveFunctionType(Context context, Function function)
        {
            var functionName = QualifyFunctionName(context, function);
            if (context.Global.Types.ContainsKey(functionName))
                return context.Global.Types[functionName];
            var functionType = BuildFunctionType(context, function);
            context.Global.Types.Add(functionName, functionType);
            return functionType;
        }

        static string QualifyFunctionName(Context context, Function function)
        {
            if ((function.Routine.Implementation == Implementation.Extern) || (context.Path == "" && function.Name == "main"))
                return function.Name;

            var functionNameBuilder = new StringBuilder();
            if (context.Path != "")
            {
                functionNameBuilder.Append(context.Path);
                functionNameBuilder.Append('.');
            }
            functionNameBuilder.Append(function.Name);
            functionNameBuilder.Append('(');
            var first = true;
            foreach (var parameter in function.Routine.Input)
            {
                if (first)
                    first = false;
                else
                    functionNameBuilder.Append(',');
                functionNameBuilder.Append(parameter.TypeSpec.Name);
            }
            functionNameBuilder.Append(')');
            return functionNameBuilder.ToString();
        }

        static LLVMTypeRef BuildFunctionType(Context context, Function function)
        {
            var returnType = GetSingleType(context, function.Routine.Result);
            var parameterTypes = GetTypes(context, function.Routine.Input);
            return LLVMTypeRef.CreateFunction(returnType, parameterTypes);
        }

        static LLVMTypeRef GetSingleType(Context context, List<Parameter> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return ResolveType(context, result[0].TypeSpec);
        }

        static LLVMTypeRef[] GetTypes(Context context, List<Parameter> result)
        {
            if (result == null)
                return new LLVMTypeRef[] { };

            return result.ConvertAll(it => ResolveType(context, it.TypeSpec)).ToArray();

        }

        static LLVMTypeRef ResolveType(Context context, TypeSpec typeSpec)
        {
            if (typeSpec.Name == "Pointer")
                return GetPointerType(context, typeSpec);

            var qualifiedName = QualifyName(context, typeSpec.Name);
            var typeName = AddTypeArguments(qualifiedName, typeSpec.Arguments);
            var type = context.ResolveType(typeName);
            if (type != null)
                return type;

            var newContext = new Context { Global = context.Global, Source = context.Global.Dictionary.Sources[qualifiedName] };
            return CreateType(newContext, qualifiedName, typeSpec);
        }

        static LLVMValueRef BuildOperands(Context context, LLVMBuilderRef builder, List<Operand> operands)
        {
            LLVMValueRef valueRef = null;
            var enumerator = operands.GetEnumerator();
            while (enumerator.MoveNext())
            {
                var operand = enumerator.Current;
                valueRef = BuildOperand(context, valueRef, builder, operand, enumerator);
            }
            return valueRef;
        }

        static void BuildSourceDictionary(DictionaryContext context)
        {
            if (context.Source.Sources != null)
            {
                foreach (var source in context.Source.Sources)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = source, Path = context.Path, Definition = context.Definition };
                    BuildSourceDictionary(newContext);
                }
            }

            if (context.Source.Definitions != null)
            {
                foreach (var definition in context.Source.Definitions.Values)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = context.Source, Path = context.Path == "" ? definition.Type.Name : context.Path + "." + definition.Type.Name, Definition = definition };
                    BuildDefinitionDictionary(newContext);
                }
            }

            if (context.Source.Functions != null)
                foreach (var function in context.Source.Functions)
                    BuildFunctionDictionary(context, function);
        }

        static void BuildDefinitionDictionary(DictionaryContext context)
        {
            if (context.Dictionary.Definitions.ContainsKey(context.Path))
                throw new CompilerException($"The definition {context.Path} was already defined.", context.Definition.Span);
            context.Dictionary.Definitions.Add(context.Path, context.Definition);
            context.Dictionary.Sources.Add(context.Path, context.Source);

            if (context.Definition.Sources != null)
            {
                foreach (var source in context.Definition.Sources)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = source, Path = context.Path, Definition = context.Definition };
                    BuildSourceDictionary(newContext);
                }
            }

            if (context.Definition.Definitions != null)
            {
                foreach (var definition in context.Definition.Definitions.Values)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = context.Source, Path = context.Path == "" ? definition.Type.Name : context.Path + "." + definition.Type.Name, Definition = definition };
                    BuildDefinitionDictionary(newContext);
                }
            }

            if (context.Definition.Functions != null)
                foreach (var function in context.Definition.Functions)
                    BuildFunctionDictionary(context, function);

            if (context.Definition.Operators != null)
                foreach (var @operator in context.Definition.Operators)
                    BuildOperatorDictionary(context, @operator);
        }

        static void BuildFunctionDictionary(DictionaryContext context, Function function)
        {
            var path = context.Path == "" ? function.Name : context.Path + "." + function.Name;
            if (context.Dictionary.Definitions.ContainsKey(path))
                return;
            context.Dictionary.Definitions.Add(path, context.Definition);
            context.Dictionary.Sources.Add(path, context.Source);
        }

        static void BuildOperatorDictionary(DictionaryContext context, Operator @operator)
        {
            var path = context.Path + "." + @operator.Name;
            if (context.Dictionary.Definitions.ContainsKey(path))
                throw new CompilerException($"The operator {path} was already defined.", @operator.Span);
            context.Dictionary.Definitions.Add(path, context.Definition);
            context.Dictionary.Sources.Add(path, context.Source);
        }

        static void BuildDefinitionTypes(Context context, Definition definition)
        {
            if (definition.Sources != null)
            {
                foreach (var source in definition.Sources)
                {
                    var newContext = new Context { Global = context.Global, Source = source };
                    BuildSourceTypes(newContext);
                }
            }

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionType(context, function);

            if (definition.Operators != null)
                foreach (var @operator in definition.Operators)
                    BuildOperatorType(context, @operator);
        }

        static void BuildSourceTypes(Context context)
        {
            if (context.Source.Definitions != null)
            {
                foreach (var definition in context.Source.Definitions.Values)
                {
                    if (definition.Type.Arguments == null)
                        BuildDefinitionTypes(context, definition);
                }
            }

            if (context.Source.Functions != null)
                foreach (var function in context.Source.Functions)
                    BuildFunctionType(context, function);

            if (context.Source.Operators != null)
                foreach (var @operator in context.Source.Operators)
                    BuildOperatorType(context, @operator);

            if (context.Source.Sources != null)
            {
                foreach (var source in context.Source.Sources)
                {
                    var newContext = new Context { Global = context.Global, Source = source };
                    BuildSourceTypes(newContext);
                }
            }
        }

        static void BuildOperatorType(Context context, Operator @operator)
        {
            var returnType = GetSingleType(context, @operator.Routine.Result);
            var parameterTypes = new List<LLVMTypeRef>();
            if (@operator.Routine.Input != null)
                parameterTypes.AddRange(GetTypes(context, @operator.Routine.Input));
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes.ToArray());
            context.Global.Types.Add(@operator.Name, functionType);
        }

        static void BuildSourceValues(Context context)
        {
            if (context.Source != null)
            {
                if (context.Source.Functions != null)
                    foreach (var function in context.Source.Functions)
                        BuildFunctionValue(context, function);

                if (context.Source.Operators != null)
                    foreach (var @operator in context.Source.Operators)
                        BuildOperatorValue(context, @operator);

                if (context.Source.Sources != null)
                {
                    foreach (var source in context.Source.Sources)
                    {
                        var newContext = new Context { Global = context.Global, Source = source };
                        BuildSourceValues(newContext);
                    }
                }

                if (context.Source.Definitions != null)
                {
                    foreach (var definition in context.Source.Definitions.Values)
                    {
                        if (definition.Type.Arguments == null)
                            BuildDefinitionValues(context, definition);
                    }
                }
            }
        }

        static void BuildDefinitionValues(Context context, Definition definition)
        {
            if (definition.Sources != null)
            {
                foreach (var source in definition.Sources)
                {
                    var newContext = new Context { Global = context.Global, Source = source };
                    BuildSourceValues(newContext);
                }
            }

            if (definition.Functions != null)
                foreach (var function in definition.Functions)
                    BuildFunctionValue(context, function);

            if (definition.Operators != null)
                foreach (var @operator in definition.Operators)
                    BuildOperatorValue(context, @operator);
        }

        static void BuildOperatorValue(Context context, Operator @operator)
        {
            var functionType = context.ResolveType(@operator.Name);
            context.Global.Values.Add(@operator.Name, context.Global.Module.AddFunction(@operator.Name, functionType));
        }

        static LLVMValueRef BuildFunctionCall(Context context, LLVMValueRef function, LLVMBuilderRef builder, Object @object)
        {
            var arguments = new List<LLVMValueRef>();
            foreach (var component in @object.Components)
            {
                LLVMValueRef valueRef = BuildOperands(context, builder, component.Value);
                arguments.Add(valueRef);
            }
            return builder.BuildCall(function, arguments.ToArray());
        }

        static TypeSpec GetOperandType(Context context, LLVMBuilderRef builder, TypeSpec previousTypeSpec, Operand operand)
        {
            switch (operand.Expression)
            {
                //case IntegerConstant integerConstant:
                //    valueRef = LLVMValueRef.CreateConstInt(LLVMTypeRef.Int32, (ulong)integerConstant.Value);
                //    break;
                case Name name:
                    return GetNameType(context, previousTypeSpec, name);
                case Scope scope:
                    return GetScopeType(context, builder, previousTypeSpec, scope);
                //case Object @object:
                //    if (valueRef == null)
                //        throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Span);
                //    valueRef = BuildFunctionCall(context, valueRef, @object);
                //    break;
                default:
                    throw new NotImplementedException($"GetOperandType for expression {operand.Expression.GetType()} not implemented.");
            }
        }

        static TypeSpec GetNameType(Context context, TypeSpec previousTypeSpec, Name name)
        {
            if (QualifyName(context, name.Path) != null)
                throw new NotImplementedException("Object creation not yet supported.");

            throw new NotImplementedException();
        }

        static TypeSpec GetScopeType(Context context, LLVMBuilderRef builder, TypeSpec previousTypeSpec, Scope scope)
        {
            TypeSpec typeSpec = null;
            foreach (var operation in scope.Operations)
                typeSpec = GetOperandsType(context, builder, operation.Operands);

            if (scope.Binding != null)
                typeSpec = GetBindingType(context, builder, scope.Binding);

            return typeSpec;
        }

        static TypeSpec GetBindingType(Context context, LLVMBuilderRef builder, Binding binding)
        {
            var newContext = new Context { Global = context.Global, Source = context.Source, ParentContext = context };
            newContext.Values.Add(binding.Identifier, BuildOperands(context, builder, binding.Operation.Operands));
            TypeSpec typeSpec = null;
            foreach (var operation in binding.Operations)
                typeSpec = GetOperandsType(newContext, builder, operation.Operands);

            return typeSpec;
        }

        static TypeSpec GetOperandsType(Context context, LLVMBuilderRef builder, List<Operand> operands)
        {
            TypeSpec typeSpec = null;
            foreach (var operand in operands)
            {
                typeSpec = GetOperandType(context, builder, typeSpec, operand);
            }
            return typeSpec;
        }

        static LLVMValueRef BuildOperand(Context context, LLVMValueRef valueRef, LLVMBuilderRef builder, Operand operand, IEnumerator<Operand> operands)
        {
            switch (operand.Expression)
            {
                case IntegerConstant integerConstant:
                    valueRef = LLVMValueRef.CreateConstInt(LLVMTypeRef.Int32, (ulong)integerConstant.Value);
                    break;
                case Name name:
                    valueRef = BuildName(context, name, operands);
                    break;
                case Scope scope:
                    valueRef = BuildScope(context, builder, scope);
                    break;
                case Object @object:
                    if (valueRef == null)
                        throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Span);
                    valueRef = BuildFunctionCall(context, valueRef, builder, @object);
                    break;
                default:
                    throw new NotImplementedException($"BuildOperand for expression {operand.Expression.GetType()} not implemented.");
            }
            return valueRef;
        }

        static LLVMValueRef BuildName(Context context, Name name, IEnumerator<Operand> operands)
        {
            var valueRef = context.ResolveValue(name.Path);
            if (valueRef != null)
                return valueRef;

            var qualifiedName = QualifyName(context, name.Path);
            if (!context.Global.Dictionary.Sources.ContainsKey(qualifiedName))
                throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);

            var definition = context.Global.Dictionary.Definitions[qualifiedName];
            var source = context.Global.Dictionary.Sources[qualifiedName];
            List<Function> functions = null;
            if (definition == null)
                functions = source.Functions.Where(it => it.Name == name.Path).ToList();
            else
                functions = definition.Functions.Where(it => it.Name == name.Path).ToList();
            if (functions.Count == 0)
                throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);

            throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);
        }

        static LLVMValueRef BuildScope(Context context, LLVMBuilderRef builder, Scope scope)
        {
            LLVMValueRef valueRef = null;
            foreach (var operation in scope.Operations)
                valueRef = BuildOperands(context, builder, operation.Operands);

            if (scope.Binding != null)
                valueRef = BuildBinding(context, builder, scope.Binding);

            return valueRef;
        }

        static LLVMValueRef BuildBinding(Context context, LLVMBuilderRef builder, Binding binding)
        {
            var newContext = new Context { Global = context.Global, Source = context.Source, ParentContext = context };
            newContext.Values.Add(binding.Identifier, BuildOperands(context, builder, binding.Operation.Operands));
            LLVMValueRef valueRef = null;
            foreach (var operation in binding.Operations)
                valueRef = BuildOperands(newContext, builder, operation.Operands);

            return valueRef;
        }

        static string QualifyName(Context context, string name)
        {
            if (context.Global.Types.ContainsKey(name))
                return name;

            if (context.Global.Dictionary.Definitions.ContainsKey(name))
                return name;

            if (context.Source.Uses != null && context.Source.Uses.ContainsKey(name))
                return context.Source.Uses[name].Path;

            if (context.Source.Usings != null)
            {
                foreach (var @using in context.Source.Usings)
                {
                    var usingName = @using.Path;
                    var nameSpace = context.ResolveName(usingName, @using.Span);
                    if (nameSpace.Definitions.ContainsKey(name))
                        return usingName + "." + name;
                }
            }

            return null;
        }

        static LLVMTypeRef CreateType(Context context, string qualifiedName, TypeSpec typeSpec)
        {
            var definition = context.ResolveName(qualifiedName, typeSpec.Span);
 
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

            return CreateGenericType(context, typeSpec, definition);
        }

        static LLVMTypeRef CreateGenericType(Context context, TypeSpec typeSpec, Definition definition)
        {
            if (definition.IsIntrinsic)
                return CreateInstrinsicType(definition.Type.Name, typeSpec.Span);

            if (definition.Structure == null)
                throw new CompilerException($"The type \"{definition.Type.Name}\" contains no data and can only be used as a namespace.", typeSpec.Span);

            if (definition.Structure.Members.Count == 1 && definition.Structure.Members[0].Name == null && definition.Structure.Members[0].Type.Arguments == null)
                return ResolveType(context, definition.Structure.Members[0].Type);

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

        static LLVMTypeRef GetPointerType(Context context, TypeSpec typeSpec)
        {
            var firstGenericArgument = typeSpec.Arguments[0];
            var pointerTarget = (firstGenericArgument.Name == "Pointer") ? GetPointerType(context, firstGenericArgument): ResolveType(context, firstGenericArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
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