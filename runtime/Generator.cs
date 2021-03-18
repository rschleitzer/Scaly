using LLVMSharp.Interop;
using Scaly.Compiler.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Scaly.Compiler
{
    internal class NameDictionary
    {
        public Dictionary<string, Definition> Definitions = new Dictionary<string, Definition>();
        public Dictionary<string, List<Function>> Functions = new Dictionary<string, List<Function>>();
        public Dictionary<string, Operator> Operators = new Dictionary<string, Operator>();
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
        public Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>> Values = new Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>>();
    }

    internal class Context
    {
        public GlobalContext Global;
        public Source Source;
        public Context ParentContext;
        public LLVMBuilderRef Builder;
        public IEnumerator<Operand> Operands;
        public KeyValuePair<TypeSpec, LLVMValueRef> TypedValue;
        public Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>> Values = new Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>>();
        public Dictionary<string, TypeSpec> GenericTypeDictionary;

        public KeyValuePair<TypeSpec, LLVMValueRef> ResolveValue(string name)
        {
            if (Values.ContainsKey(name))
                return Values[name];

            if (ParentContext != null)
            {
                var valueRef = ParentContext.ResolveValue(name);
                if (valueRef.Value != null)
                    return valueRef;
            }
            else
            {
                if (Global.Values.ContainsKey(name))
                    return Global.Values[name];
            }

            return new KeyValuePair<TypeSpec, LLVMValueRef>(null, null);
        }

        internal Definition ResolveDefinitionName(string name, Span span)
        {
            if (!Global.Dictionary.Definitions.ContainsKey(name))
                throw new CompilerException($"Tht definition \"{name}\" was not found.", span);

            return Global.Dictionary.Definitions[name];
        }

        internal List<Function> ResolveFunctionName(string name, Span span)
        {
            if (!Global.Dictionary.Functions.ContainsKey(name))
                throw new CompilerException($"Tht function \"{name}\" was not found.", span);

            return Global.Dictionary.Functions[name];
        }

        internal Operator ResolveOperatorName(string name, Span span)
        {
            if (!Global.Dictionary.Operators.ContainsKey(name))
                throw new CompilerException($"Tht function \"{name}\" was not found.", span);

            return Global.Dictionary.Operators[name];
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
            LLVMModuleRef module = LLVMModuleRef.CreateWithName("");
            var function = GenerateAndVerify(sources, module);
            var engine = module.CreateMCJITCompiler();
            var jitMain = engine.GetPointerToGlobal<Main>(function);
            return jitMain;
        }

        internal static void GenerateProgram(List<Source> sources, string outputName)
        {
            LLVMModuleRef module = LLVMModuleRef.CreateWithName("");
            GenerateAndVerify(sources, module);
            LLVM.InitializeAllTargetInfos();
            var triple = LLVMTargetRef.DefaultTriple;
            LLVMTargetRef target = LLVMTargetRef.GetTargetFromTriple(triple);
            LLVMTargetMachineRef targetMachine = target.CreateTargetMachine(triple, "", "",
                LLVMCodeGenOptLevel.LLVMCodeGenLevelDefault, LLVMRelocMode.LLVMRelocDefault,
                LLVMCodeModel.LLVMCodeModelDefault);
            targetMachine.EmitToFile(module, $"{outputName}.o", LLVMCodeGenFileType.LLVMObjectFile);
        }

        static LLVMValueRef GenerateAndVerify(List<Source> sources, LLVMModuleRef module)
        {
            var function = Generate(module, sources);
            module.Verify(LLVMVerifierFailureAction.LLVMPrintMessageAction);
            LLVM.InitializeNativeTarget();
            LLVM.InitializeNativeAsmParser();
            LLVM.InitializeNativeAsmPrinter();
            return function;
        }

        static LLVMValueRef Generate(LLVMModuleRef module, List<Source> sources)
        {
            var dictionary = new NameDictionary();
            {
                var context = new DictionaryContext { Dictionary = dictionary, Path = "" };
                foreach (var source in sources)
                {
                    context.Source = source;
                    CreateSourceDictionary(context);
                }
            }

            {
                var globalContext = new GlobalContext { Module = module, Dictionary = dictionary };
                var mainName = "main";
                var context = new Context { Global = globalContext, Source = globalContext.Dictionary.Sources[mainName] };
                var function = GetMainFunction(context, mainName);
                var functionValue = ResolveFunctionValue(context, function);
                return functionValue.Value;
            }
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> ResolveFunctionValue(Context context, Function function)
        {
            var functionName = QualifyFunctionName(context, function);
            if (!context.Global.Values.ContainsKey(functionName))
                CreateFunctionValue(context, function, functionName);
            var llvmFunction = context.Global.Values[function.Name];
            return llvmFunction;
        }

        static string QualifyFunctionName(Context context, Function function)
        {
            if ((function.Routine.Implementation == Implementation.Extern) || (function.Name == "main"))
                return function.Name;

            var functionNameBuilder = new StringBuilder();

            functionNameBuilder.Append(function.Name);
            functionNameBuilder.Append('(');
            var first = true;
            foreach (var parameter in function.Routine.Input)
            {
                if (first)
                    first = false;
                else
                    functionNameBuilder.Append(',');
                functionNameBuilder.Append(AddTypeArguments(parameter.TypeSpec.Name, parameter.TypeSpec.Arguments));
            }
            functionNameBuilder.Append(')');
            return functionNameBuilder.ToString();
        }

        static void CreateFunctionValue(Context context, Function function, string qualifiedName)
        {
            var functionType = ResolveFunctionType(context, function, qualifiedName);
            var functionValue = context.Global.Module.AddFunction(qualifiedName, functionType);
            context.Global.Values.Add(qualifiedName, new KeyValuePair<TypeSpec, LLVMValueRef>(function.Routine.Result[0].TypeSpec, functionValue));
            ImplementRoutine(context, function.Source, functionValue, function.Routine, null, function.Span);
        }

        static void ImplementRoutine(Context context, Source source, LLVMValueRef functionValue, Routine routine, TypeSpec operatorType, Span span)
        {
            switch (routine.Implementation)
            {
                case Implementation.Intern:
                    {
                        var block = functionValue.AppendBasicBlock(string.Empty);
                        using (var builder = context.Global.Module.Context.CreateBuilder())
                        {
                            var newContext = new Context { Global = context.Global, Builder = builder, Source = source, GenericTypeDictionary = context.GenericTypeDictionary };
                            builder.PositionAtEnd(block);
                            uint paramCount = 0;
                            if (routine.Input != null)
                            {
                                // function case
                                foreach (var parameter in routine.Input)
                                {
                                    newContext.Values.Add(parameter.Name, new KeyValuePair<TypeSpec, LLVMValueRef>(parameter.TypeSpec, functionValue.GetParam(paramCount)));
                                    paramCount++;
                                }
                            }
                            else
                            {
                                // operator case
                                newContext.Values.Add("this", new KeyValuePair<TypeSpec, LLVMValueRef>(operatorType, functionValue.GetParam(0)));
                            }
                            builder.BuildRet(BuildOperands(newContext, routine.Operation.Operands).Value);
                        }
                    }
                    break;
                case Implementation.Extern:
                    {
                        functionValue.Linkage = LLVMLinkage.LLVMExternalLinkage;
                        break;
                    }
                default:
                    throw new CompilerException($"The implementation type {routine.Implementation} is not implemented.", span);
            }
        }

        static LLVMTypeRef ResolveFunctionType(Context context, Function function, string qualifiedName)
        {
            if (context.Global.Types.ContainsKey(qualifiedName))
                return context.Global.Types[qualifiedName];
            var functionType = CreateFunctionType(context, function);
            context.Global.Types.Add(qualifiedName, functionType);
            return functionType;
        }

        static LLVMTypeRef CreateFunctionType(Context context, Function function)
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
            LLVMTypeRef type;
            if (context.GenericTypeDictionary != null && context.GenericTypeDictionary.ContainsKey(typeSpec.Name))
                typeSpec = context.GenericTypeDictionary[typeSpec.Name];
            var qualifiedName = QualifyName(context, typeSpec.Name);
            var typeName = AddTypeArguments(qualifiedName, typeSpec.Arguments);
            if (context.Global.Types.ContainsKey(typeName))
                return context.Global.Types[typeName];

            if (typeSpec.Name == "Pointer")
            {
                type = GetPointerType(context, typeSpec);
            }
            else
            {
                var newContext = new Context { Global = context.Global, Source = context.Global.Dictionary.Sources[qualifiedName] };
                type = CreateType(newContext, qualifiedName, typeSpec);
            }

            context.Global.Types.Add(typeName, type);
            return type;
        }

        static string QualifyName(Context context, string name)
        {
            if (context.Global.Types.ContainsKey(name))
                return name;

            if (context.Global.Dictionary.Definitions.ContainsKey(name))
                return name;

            if (context.Global.Dictionary.Functions.ContainsKey(name))
                return name;

            if (context.Global.Dictionary.Operators.ContainsKey(name))
                return name;

            if (context.Source.Uses != null && context.Source.Uses.ContainsKey(name))
                return context.Source.Uses[name].Path;

            if (context.Source.Usings != null)
            {
                foreach (var @using in context.Source.Usings)
                {
                    var usingName = @using.Path;

                    var definition = context.ResolveDefinitionName(usingName, @using.Span);
                    if (definition.Definitions != null && definition.Definitions.ContainsKey(name))
                        return usingName + "." + name;

                    if (definition.Operators != null && definition.Operators.ContainsKey(name))
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
            builder.Append(']');

            return builder.ToString();
        }

        static LLVMTypeRef CreateType(Context context, string qualifiedName, TypeSpec typeSpec)
        {
            var definition = context.ResolveDefinitionName(qualifiedName, typeSpec.Span);

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

        static LLVMTypeRef GetPointerType(Context context, TypeSpec typeSpec)
        {
            if (typeSpec.Arguments == null)
                throw new CompilerException("Pointer is a generic type which expects a type argument.", typeSpec.Span);
            if (typeSpec.Arguments.Count != 1)
                throw new CompilerException($"Pointer expects exactly one type argument, not {typeSpec.Arguments.Count}.", typeSpec.Span);
            var typeArgument = typeSpec.Arguments[0];
            var pointerTarget = (typeArgument.Name == "Pointer") ? GetPointerType(context, typeArgument) : ResolveType(context, typeArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> ResolveOperatorValue(Context context, TypeSpec operandType, Operator @operator)
        {
            var qualifiedName = QualifyOperatorName(context, operandType, @operator);
            if (!context.Global.Values.ContainsKey(qualifiedName))
                CreateOperatorValue(context, operandType, @operator, qualifiedName);
            var llvmFunction = context.Global.Values[qualifiedName];
            return llvmFunction;
        }

        static string QualifyOperatorName(Context context, TypeSpec operandType, Operator @operator)
        {
            var operatorNameBuilder = new StringBuilder();

            operatorNameBuilder.Append(QualifyName(context, @operator.Definition.Type.Name));
            operatorNameBuilder.Append('.');
            operatorNameBuilder.Append(@operator.Name);
            operatorNameBuilder.Append('[');
            AppendGenericArguments(context, operatorNameBuilder, operandType, @operator.Span);
            operatorNameBuilder.Append(']');
            return operatorNameBuilder.ToString();
        }

        static void AppendGenericArguments(Context context, StringBuilder builder, TypeSpec operandType, Span span)
        {
            builder.Append(QualifyName(context, operandType.Name));
            if (operandType.Arguments != null)
            {
                builder.Append('[');
                bool first = true;
                foreach (var type in operandType.Arguments)
                {
                    if (first)
                        first = false;
                    else
                        builder.Append(',');
                    AppendGenericArguments(context, builder, type, span);
                }
                builder.Append(']');
            }
        }

        static void CreateOperatorValue(Context context, TypeSpec operandType, Operator @operator, string qualifiedName)
        {
            Dictionary<string, TypeSpec> genericTypeDictionary = CreateOperatorGenericTypeDictionary(operandType, @operator);
            var newContext = new Context { Global = context.Global, Source = context.Source, ParentContext = context, GenericTypeDictionary = genericTypeDictionary };
            var operatorType = ResolveOperatorType(newContext, operandType, @operator, qualifiedName);
            var operatorFunction = context.Global.Module.AddFunction(qualifiedName, operatorType);
            var operatorValue = new KeyValuePair<TypeSpec, LLVMValueRef>(@operator.Routine.Result[0].TypeSpec, operatorFunction);
            context.Global.Values.Add(qualifiedName, operatorValue);
            ImplementRoutine(newContext, @operator.Definition.Source, operatorFunction, @operator.Routine, @operator.Definition.Type, @operator.Span);
        }

        static Dictionary<string, TypeSpec> CreateOperatorGenericTypeDictionary(TypeSpec operandTypeSpec, Operator @operator)
        {
            if (operandTypeSpec.Arguments == null)
            {
                if (@operator.Definition.Type.Arguments != null)
                    throw new CompilerException($"This operator acts on a generic operand, and a non-generic operand was provided.", @operator.Span);
                return null;
            }
            else
            {
                if (@operator.Definition.Type.Arguments == null)
                    throw new CompilerException($"This operator acts on a non-generic operand, and a generic operand was provided.", @operator.Span);

                if (operandTypeSpec.Arguments.Count != @operator.Definition.Type.Arguments.Count)
                    throw new CompilerException($"This operator has {@operator.Definition.Type.Arguments.Count} generic type arguments, and the operand has {operandTypeSpec.Arguments.Count}.", @operator.Span);

                var genericTypeDictionary = new Dictionary<string, TypeSpec>();
                var genericArgumentEnumerator = @operator.Definition.Type.Arguments.GetEnumerator();
                foreach (var argument in operandTypeSpec.Arguments)
                {
                    genericArgumentEnumerator.MoveNext();
                    var genericArgument = genericArgumentEnumerator.Current;
                    if (genericTypeDictionary.ContainsKey(genericArgument.Name))
                        throw new CompilerException($"The generic type argument {genericArgument.Name} was already given.", @operator.Span);
                    genericTypeDictionary.Add(genericArgument.Name, argument);
                }

                return genericTypeDictionary;
            }
        }

        static LLVMTypeRef ResolveOperatorType(Context context, TypeSpec operandType, Operator @operator, string qualifiedName)
        {
            if (context.Global.Types.ContainsKey(qualifiedName))
                return context.Global.Types[qualifiedName];
            var operatorFunctionType = CreateOperatorType(context, operandType, @operator);
            context.Global.Types.Add(qualifiedName, operatorFunctionType);
            return operatorFunctionType;
        }

        static LLVMTypeRef CreateOperatorType(Context context, TypeSpec operandTypeSpec, Operator @operator)
        {
            {
                var operandTypeName = QualifyName(context, operandTypeSpec.Name);
                var operatorDefinitionTypeName = QualifyName(context, @operator.Definition.Type.Name);
                if (operandTypeName != operatorDefinitionTypeName)
                    throw new CompilerException($"This operator expects a {operatorDefinitionTypeName}, and a {operandTypeName} was provided.", @operator.Span);
            }

            var operandType = ResolveType(context, operandTypeSpec);
            var parameterTypes = new LLVMTypeRef[] { operandType };
            var returnType = ResolveType(context, @operator.Routine.Result[0].TypeSpec);
            LLVMTypeRef operatorFunctionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes.ToArray());
            return operatorFunctionType;
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildOperands(Context context, List<Operand> operands)
        {
            var newContext = new Context { Global = context.Global, Source = context.Source, Builder = context.Builder, Operands = operands.GetEnumerator(), TypedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(null, null), GenericTypeDictionary = context.GenericTypeDictionary, ParentContext = context };
            BuildOperand(newContext);
            return newContext.TypedValue;
        }

        static void BuildOperand(Context context)
        {
            while (context.Operands.MoveNext())
            {
                var operand = context.Operands.Current;
                switch (operand.Expression)
                {
                    case IntegerConstant integerConstant:
                        context.TypedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(context.Global.Dictionary.Definitions["Integer"].Type, LLVMValueRef.CreateConstInt(LLVMTypeRef.Int32, (ulong)integerConstant.Value));
                        break;
                    case BooleanConstant booleanConstant:
                        context.TypedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(context.Global.Dictionary.Definitions["Boolean"].Type, LLVMValueRef.CreateConstInt(LLVMTypeRef.Int1, booleanConstant.Value ? 1UL : 0UL));
                        break;
                    case Name name:
                        BuildName(context, name);
                        break;
                    case Scope scope:
                        BuildScope(context, scope);
                        break;
                    case Model.Tuple @object:
                        if (context.TypedValue.Value == null)
                            throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Span);
                        context.TypedValue = BuildFunctionCall(context, context.TypedValue, @object);
                        break;
                    default:
                        throw new NotImplementedException($"BuildOperand for expression {operand.Expression.GetType()} not implemented.");
                }
            }
        }

        static void BuildName(Context context, Name name)
        {
            var valueRef = context.ResolveValue(name.Path);
            if (valueRef.Value != null)
            {
                context.TypedValue = valueRef;
                return;
            }

            var qualifiedName = QualifyName(context, name.Path);
            if (qualifiedName == null)
                throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);
            if (!context.Global.Dictionary.Sources.ContainsKey(qualifiedName))
                throw new CompilerException($"The name '{name.Path}' was not in the dictionary.", name.Span);

            List<Function> functions = null;
            if (context.Global.Dictionary.Functions.ContainsKey(qualifiedName))
                functions = context.Global.Dictionary.Functions[qualifiedName];

            Operator @operator = null;
            if (context.Global.Dictionary.Operators.ContainsKey(qualifiedName))
                @operator = context.Global.Dictionary.Operators[qualifiedName];

            if (@operator != null)
            {
                if (context.TypedValue.Value == null)
                    throw new CompilerException("An operator cannot act on nothing. It must follow an operand it can act upon.", @operator.Span);
                var operatorValue = ResolveOperatorValue(context, context.TypedValue.Key, @operator);
                BuildOperatorCall(context, operatorValue, context.TypedValue);
                return;
            }

            if (functions != null)
            {
                if (!context.Operands.MoveNext())
                    throw new CompilerException($"No function arguments for '{name.Path}' were given.", name.Span);

                var operand = context.Operands.Current;
                switch (operand.Expression)
                {
                    case Model.Tuple @object:
                        {
                            var function = FindMatchingFunction(context, functions, null, @object);
                            switch (function.Routine.Implementation)
                            {
                                case Implementation.Intern:
                                case Implementation.Extern:
                                    valueRef = ResolveFunctionValue(context, function);
                                    if (valueRef.Value == null)
                                        throw new CompilerException("No matching function has been found for the arguments.", @object.Span);
                                    context.TypedValue = BuildFunctionCall(context, valueRef, @object);
                                    return;
                                case Implementation.Instruction:
                                    context.TypedValue = BuildInstruction(context, null, function);
                                    return;
                                default:
                                    throw new CompilerException($"The '{function.Routine.Implementation}' is not implemented.", @name.Span);
                            }
                        }
                    case Vector vector:
                        if (!context.Operands.MoveNext())
                            throw new CompilerException($"No function arguments for '{name.Path}' were given.", name.Span);
                        var genericOperand = context.Operands.Current;
                        switch (genericOperand.Expression)
                        {
                            case Model.Tuple @object:
                                var function = FindMatchingFunction(context, functions, vector, @object);
                                switch (function.Routine.Implementation)
                                {
                                    case Implementation.Intern:
                                    case Implementation.Extern:
                                        valueRef = ResolveFunctionValue(context, function);
                                        if (valueRef.Value == null)
                                            throw new CompilerException("No matching function has been found for the arguments.", @object.Span);
                                        context.TypedValue = BuildFunctionCall(context, valueRef, @object);
                                        return;
                                    case Implementation.Instruction:
                                        context.TypedValue = BuildInstruction(context, vector, function);
                                        return;
                                    default:
                                        throw new CompilerException($"The '{function.Routine.Implementation}' is not implemented.", @name.Span);
                                }
                            default:
                                throw new CompilerException($"Only an object can be applied to function '{name.Path}'. Got an {operand.Expression.GetType()}.", @name.Span);
                        }
                    default:
                        throw new CompilerException($"Only an object can be applied to function '{name.Path}'. Got an {operand.Expression.GetType()}.", @name.Span);
                }
            }
            else
            {
                if (!context.Global.Dictionary.Definitions.ContainsKey(qualifiedName))
                    throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);
                var definition = context.Global.Dictionary.Definitions[qualifiedName];
                if (definition.Operands != null)
                {
                    // Constant
                    var constant = BuildOperands(context, definition.Operands);
                    context.Global.Values.Add(qualifiedName, constant);
                    context.TypedValue = constant;
                }
            }
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildInstruction(Context context, Vector vector, Function function)
        {
            switch (function.Name)
            {
                case "load":
                    {
                        var genericType = function.Routine.Result[0].TypeSpec;
                        if (!context.GenericTypeDictionary.ContainsKey(genericType.Name))
                            throw new CompilerException($"The generic type {genericType.Name} is not defined here.", function.Span);
                        return new KeyValuePair<TypeSpec, LLVMValueRef>(context.GenericTypeDictionary[genericType.Name], context.Builder.BuildLoad(context.ResolveValue("this").Value));
                    }
                case "trunc":
                    {
                        if (vector == null || vector.Components == null || vector.Components.Count != 1 || vector.Components == null || vector.Components[0].Count != 1)
                            throw new CompilerException($"The {function.Name} instruction needs one generic argument.", function.Span);
                        LLVMTypeRef destinationTypeRef;
                        TypeSpec destinationType;
                        switch (vector.Components[0][0].Expression)
                        {
                            case Name name:
                                var destinationName = QualifyName(context, name.Path);
                                switch (destinationName)
                                {
                                    case "LLVM.i32":
                                        destinationTypeRef = context.Global.Types[destinationName];
                                        destinationType = context.Global.Dictionary.Definitions[destinationName].Type;
                                        break;
                                    default:
                                        throw new CompilerException($"The {name.Path} is not a valid typen argument for trunc.", function.Span);
                                }
                                break;
                            default:
                                throw new CompilerException($"The {function.Name} instruction needs a literal generic argument.", function.Span);
                        }
                        return new KeyValuePair<TypeSpec, LLVMValueRef>(destinationType, context.Builder.BuildTrunc(context.ResolveValue("this").Value, destinationTypeRef));
                    }
                case "zext":
                    {
                        if (vector == null || vector.Components == null || vector.Components.Count != 1 || vector.Components == null || vector.Components[0].Count != 1)
                            throw new CompilerException($"The {function.Name} instruction needs one generic argument.", function.Span);
                        LLVMTypeRef destinationTypeRef;
                        TypeSpec destinationType;
                        switch (vector.Components[0][0].Expression)
                        {
                            case Name name:
                                var destinationName = QualifyName(context, name.Path);
                                switch (destinationName)
                                {
                                    case "LLVM.i32":
                                        destinationTypeRef = context.Global.Types[destinationName];
                                        destinationType = context.Global.Dictionary.Definitions[destinationName].Type;
                                        break;
                                    default:
                                        throw new CompilerException($"The {name.Path} is not a valid typen argument for trunc.", function.Span);
                                }
                                break;
                            default:
                                throw new CompilerException($"The {function.Name} instruction needs a literal generic argument.", function.Span);
                        }
                        return new KeyValuePair<TypeSpec, LLVMValueRef>(destinationType, context.Builder.BuildZExt(context.ResolveValue("this").Value, destinationTypeRef));
                    }
                default:
                    throw new CompilerException($"The instruction {function.Name} is not implemented.", function.Span);
            }
        }

        static Function FindMatchingFunction(Context context, List<Function> functions, Vector vector, Model.Tuple @object)
        {
            var functionsWithSameNumberOfArguments = functions.Where(it => it.Routine.Input.Count == @object.Components.Count).ToList();
            if (functionsWithSameNumberOfArguments.Count == 0)
                throw new CompilerException($"No overload of the function takes {@object.Components.Count} arguments.", @object.Span);
            if (functionsWithSameNumberOfArguments.Count > 1)
                throw new CompilerException($"More than one overload of the function takes {@object.Components.Count} arguments.", @object.Span);
            var function = functionsWithSameNumberOfArguments.First();
            return function;
        }

        static void BuildScope(Context context, Scope scope)
        {
            KeyValuePair<TypeSpec, LLVMValueRef> typedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(null, null);
            foreach (var operation in scope.Operations)
                context.TypedValue = BuildOperands(context, operation.Operands);

            if (scope.Binding != null)
                BuildBinding(context, scope.Binding);
        }

        static void BuildBinding(Context context, Binding binding)
        {
            var newContext = new Context { Global = context.Global, Source = context.Source, Operands = binding.Operation.Operands.GetEnumerator(), ParentContext = context };
            BuildOperand(newContext);
            newContext.Values.Add(binding.Identifier, context.TypedValue);
            foreach (var operation in binding.Operations)
                BuildOperands(newContext, operation.Operands);
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildFunctionCall(Context context, KeyValuePair<TypeSpec, LLVMValueRef> function, Model.Tuple @object)
        {
            var arguments = new List<LLVMValueRef>();
            foreach (var component in @object.Components)
            {
                context.TypedValue = BuildOperands(context, component.Value);
                arguments.Add(context.TypedValue.Value);
            }

            return new KeyValuePair<TypeSpec, LLVMValueRef>(function.Key, context.Builder.BuildCall(function.Value, arguments.ToArray()));
        }

        static void BuildOperatorCall(Context context, KeyValuePair<TypeSpec, LLVMValueRef> @operator, KeyValuePair<TypeSpec, LLVMValueRef> operand)
        {
            context.TypedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(@operator.Key, context.Builder.BuildCall(@operator.Value, new LLVMValueRef[] { operand.Value }));
        }

        static void CreateSourceDictionary(DictionaryContext context)
        {
            if (context.Source.Sources != null)
            {
                foreach (var source in context.Source.Sources)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = source, Path = context.Path, Definition = context.Definition };
                    CreateSourceDictionary(newContext);
                }
            }

            if (context.Source.Definitions != null)
            {
                foreach (var definition in context.Source.Definitions.Values)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = context.Source, Path = context.Path == "" ? definition.Type.Name : context.Path + "." + definition.Type.Name, Definition = definition };
                    CreateDefinitionDictionary(newContext);
                }
            }

            if (context.Source.Functions != null)
                foreach (var function in context.Source.Functions)
                    CreateFunctionDictionary(context, function);
        }

        static void CreateDefinitionDictionary(DictionaryContext context)
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
                    CreateSourceDictionary(newContext);
                }
            }

            if (context.Definition.Definitions != null)
            {
                foreach (var definition in context.Definition.Definitions.Values)
                {
                    var newContext = new DictionaryContext { Dictionary = context.Dictionary, Source = context.Source, Path = context.Path == "" ? definition.Type.Name : context.Path + "." + definition.Type.Name, Definition = definition };
                    CreateDefinitionDictionary(newContext);
                }
            }

            if (context.Definition.Functions != null)
                foreach (var function in context.Definition.Functions)
                    CreateFunctionDictionary(context, function);

            if (context.Definition.Operators != null)
                foreach (var @operator in context.Definition.Operators.Values)
                    CreateOperatorDictionary(context, @operator);
        }

        static void CreateFunctionDictionary(DictionaryContext context, Function function)
        {
            var path = context.Path == "" ? function.Name : context.Path + "." + function.Name;
            if (!context.Dictionary.Functions.ContainsKey(path))
                context.Dictionary.Functions.Add(path, new List<Function>());

            context.Dictionary.Functions[path].Add(function);
            context.Dictionary.Sources.Add(path, context.Source);
        }

        static void CreateOperatorDictionary(DictionaryContext context, Operator @operator)
        {
            var path = context.Path + "." + @operator.Name;
            if (context.Dictionary.Operators.ContainsKey(path))
                throw new CompilerException($"The operator {path} was already defined.", @operator.Span);
            context.Dictionary.Operators.Add(path, @operator);
            context.Dictionary.Sources.Add(path, context.Source);
        }

        static Function GetMainFunction(Context context, string mainName)
        {
            if (!context.Global.Dictionary.Functions.ContainsKey(mainName))
                throw new CompilerException("No main function found.", new Span { file = "", start = new Position { line = 0, column = 0 }, end = new Position { line = 0, column = 0 }, });
            var functions = context.Global.Dictionary.Functions[mainName];
            if (functions.Count == 0)
                throw new CompilerException("No main function found.", new Span { file = "", start = new Position { line = 0, column = 0 }, end = new Position { line = 0, column = 0 }, });
            if (functions.Count > 1)
                throw new CompilerException("More than one main function found.", new Span { file = "", start = new Position { line = 0, column = 0 }, end = new Position { line = 0, column = 0 }, });
            var function = functions.First();
            if (function.Routine.Result == null)
                throw new CompilerException("Main function has no return value. The return value must be Integer.", function.Span);
            if (function.Routine.Result.Count != 1)
                throw new CompilerException("Main function has the wrong number of results. It has to return exactly one Integer.", function.Span);
            var result = function.Routine.Result[0].TypeSpec;
            if (result.Name != "Integer")
                throw new CompilerException("Main function has the wrong return value type. The return value must be of type Integer.", function.Span);
            var argcName = "argument count";
            var argcType = "Integer";
            var argvName = "argument values";
            var argvType = "Pointer";
            var charType = "Byte";
            var parameterSentence = $"The parameters must be '{argcName}': {argcType}, '{argvName}': {argvType}.";
            if (function.Routine.Input == null)
                throw new CompilerException($"Main function has no parameters. {parameterSentence}", function.Span);
            if (function.Routine.Input.Count != 2)
                throw new CompilerException($"Main function has the wrong number of parameters. {parameterSentence}", function.Span);
            var argc = function.Routine.Input[0];
            if (argc.Name != argcName)
                throw new CompilerException($"The first parameter of the main function has the wrong name. {parameterSentence}", function.Span);
            if (argc.TypeSpec.Name != argcType)
                throw new CompilerException($"The first parameter of the main function has the wrong type. {parameterSentence}", function.Span);
            var argv = function.Routine.Input[1];
            if (argv.Name != argvName)
                throw new CompilerException($"The second parameter of the main function has the wrong name. {parameterSentence}", function.Span);
            if
            (
                argv.TypeSpec.Name == argvType
                && argv.TypeSpec.Arguments != null 
                && argv.TypeSpec.Arguments.Count == 1 
                && argv.TypeSpec.Arguments[0].Name == argvType
                && argv.TypeSpec.Arguments[0].Arguments != null 
                && argv.TypeSpec.Arguments[0].Arguments.Count == 1 
                && argv.TypeSpec.Arguments[0].Arguments[0].Name == charType
            )
                return function;
            else
                throw new CompilerException($"The second parameter of the main function has the wrong type. {parameterSentence}", function.Span);
        }
    }
}