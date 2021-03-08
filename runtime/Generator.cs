using LLVMSharp.Interop;
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
        public string Path = "";
        public Context ParentContext;
        public LLVMBuilderRef Builder;
        public IEnumerator<Operand> Operands;
        public KeyValuePair<TypeSpec, LLVMValueRef> TypedValue;
        public Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>> Values = new Dictionary<string, KeyValuePair<TypeSpec, LLVMValueRef>>();

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

            if (Global.Values.ContainsKey(name))
                return Global.Values[name];

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
                    if (context.Source.Functions != null)
                    {
                        foreach (var function in context.Source.Functions)
                        {
                            BuildFunction(context, null, function);
                        }
                    }
                }
            }
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildFunction(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, Function function)
        {
            var functionValue = ResolveFunctionValue(context, genericTypeDictionary, function);
            var block = functionValue.Value.AppendBasicBlock(string.Empty);
            using (var builder = context.Global.Module.Context.CreateBuilder())
            {
                var newContext = new Context { Global = context.Global, Builder = builder, Source = context.Source, };
                builder.PositionAtEnd(block);
                uint paramCount = 0;
                foreach (var parameter in function.Routine.Input)
                {
                    newContext.Values.Add(parameter.Name, new KeyValuePair<TypeSpec, LLVMValueRef>(parameter.TypeSpec, functionValue.Value.GetParam(paramCount)));
                    paramCount++;
                }
                builder.BuildRet(BuildOperands(newContext, function.Routine.Operation.Operands).Value);
            }
            return functionValue;
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> ResolveFunctionValue(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, Function function)
        {
            var functionName = QualifyFunctionName(context, function);
            if (!context.Global.Values.ContainsKey(functionName))
                BuildFunctionValue(context, genericTypeDictionary, function);
            var llvmFunction = context.Global.Values[function.Name];
            return llvmFunction;
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> ResolveOperatorValue(Context context, TypeSpec operandType, Operator @operator)
        {
            var qualifiedName = QualifyOperatorName(context, operandType, @operator);
            if (!context.Global.Values.ContainsKey(qualifiedName))
                BuildOperatorValue(context, operandType, @operator, qualifiedName);
            var llvmFunction = context.Global.Values[qualifiedName];
            return llvmFunction;
        }

        static void BuildFunctionValue(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, Function function)
        {
            var functionType = ResolveFunctionType(context, genericTypeDictionary, function);
            var functionName = QualifyFunctionName(context, function);
            context.Global.Values.Add(function.Name, new KeyValuePair<TypeSpec, LLVMValueRef>(null, context.Global.Module.AddFunction(functionName, functionType)));
        }

        static LLVMTypeRef ResolveFunctionType(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, Function function)
        {
            var functionName = QualifyFunctionName(context, function);
            if (context.Global.Types.ContainsKey(functionName))
                return context.Global.Types[functionName];
            var functionType = BuildFunctionType(context, genericTypeDictionary, function);
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

        static string QualifyOperatorName(Context context, TypeSpec operandType, Operator @operator)
        {
            var operatorNameBuilder = new StringBuilder();
            if (context.Path != "")
            {
                operatorNameBuilder.Append(context.Path);
                operatorNameBuilder.Append('.');
            }

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

        static LLVMTypeRef BuildFunctionType(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, Function function)
        {
            var returnType = GetSingleType(context, null, function.Routine.Result);
            var parameterTypes = GetTypes(context, genericTypeDictionary, function.Routine.Input);
            return LLVMTypeRef.CreateFunction(returnType, parameterTypes);
        }

        static LLVMTypeRef GetSingleType(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, List<Parameter> result)
        {
            if (result == null)
                return LLVMTypeRef.Void;

            return ResolveType(context, genericTypeDictionary, result[0].TypeSpec);
        }

        static LLVMTypeRef[] GetTypes(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, List<Parameter> result)
        {
            if (result == null)
                return new LLVMTypeRef[] { };

            return result.ConvertAll(it => ResolveType(context, genericTypeDictionary, it.TypeSpec)).ToArray();
        }

        static LLVMTypeRef ResolveType(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, TypeSpec typeSpec)
        {
            LLVMTypeRef type;
            if (genericTypeDictionary != null && genericTypeDictionary.ContainsKey(typeSpec.Name))
                typeSpec = genericTypeDictionary[typeSpec.Name];
            var qualifiedName = QualifyName(context, typeSpec.Name);
            var typeName = AddTypeArguments(qualifiedName, typeSpec.Arguments);
            if (context.Global.Types.ContainsKey(typeName))
                return context.Global.Types[typeName];

            if (typeSpec.Name == "Pointer")
            {
                type = GetPointerType(context, genericTypeDictionary, typeSpec);
            }
            else
            {
                var newContext = new Context { Global = context.Global, Source = context.Global.Dictionary.Sources[qualifiedName] };
                type = CreateType(newContext, qualifiedName, typeSpec);
            }

            context.Global.Types.Add(typeName, type);
            return type;
        }

        static LLVMTypeRef GetPointerType(Context context, Dictionary<string, TypeSpec> genericTypeDictionary, TypeSpec typeSpec)
        {
            if (typeSpec.Arguments == null)
                throw new CompilerException("Pointer is a generic type which expects a type argument.", typeSpec.Span);
            if (typeSpec.Arguments.Count != 1)
                throw new CompilerException($"Pointer expects exactly one type argument, not {typeSpec.Arguments.Count}.", typeSpec.Span);
            var typeArgument = typeSpec.Arguments[0];
            var pointerTarget = (typeArgument.Name == "Pointer") ? GetPointerType(context, genericTypeDictionary, typeArgument) : ResolveType(context, genericTypeDictionary, typeArgument);
            return LLVMTypeRef.CreatePointer(pointerTarget, 0);
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildOperands(Context context, List<Operand> operands)
        {
            var newContext = new Context { Global = context.Global, Builder = context.Builder, Operands = operands.GetEnumerator(), TypedValue = new KeyValuePair<TypeSpec, LLVMValueRef>(null, null), ParentContext = context, Source = context.Source };
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
                    case Name name:
                        BuildName(context, name);
                        break;
                    case Scope scope:
                        BuildScope(context, scope);
                        break;
                    case Object @object:
                        if (context.TypedValue.Value == null)
                            throw new CompilerException("Objects are currently only supported as parameter lists for function calls.", @object.Span);
                        context.TypedValue = BuildFunctionCall(context, @object);
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
            }

            if (functions.Count == 0)
                throw new CompilerException($"The name '{name.Path}' has not been found.", name.Span);

            if (!context.Operands.MoveNext())
                throw new CompilerException($"No function arguments for '{name.Path}' were given.", name.Span);

            var operand = context.Operands.Current;
            switch (operand.Expression)
            {
                case Object @object:
                    valueRef = FindMatchingFunction(context, functions, @object);
                    if (valueRef.Value == null)
                        throw new CompilerException("No matching function has been found for the arguments.", @object.Span);
                    BuildFunctionCall(context, @object);
                    return;
                default:
                    throw new CompilerException($"Only an object can be applied to function '{name.Path}'. Got an {operand.Expression.GetType()}.", @name.Span);
            }
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> FindMatchingFunction(Context context, List<Function> functions, Object @object)
        {
            var functionsWithSameNumberOfArguments = functions.Where(it => it.Routine.Input.Count == @object.Components.Count).ToList();
            if (functionsWithSameNumberOfArguments.Count == 0)
                throw new CompilerException($"No overload of the function takes {@object.Components.Count} arguments.", @object.Span);
            if (functionsWithSameNumberOfArguments.Count > 1)
                throw new CompilerException($"More than one overload of the function takes {@object.Components.Count} arguments.", @object.Span);
            var function = functionsWithSameNumberOfArguments.First();
            var functionValue = ResolveFunctionValue(context, null, function);
            return functionValue;
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

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildFunctionCall(Context context, Object @object)
        {
            var arguments = new List<LLVMValueRef>();
            KeyValuePair<TypeSpec, LLVMValueRef> function = context.TypedValue;
            foreach (var component in @object.Components)
            {
                BuildOperands(context, component.Value);
                arguments.Add(context.TypedValue.Value);
            }

            return new KeyValuePair<TypeSpec, LLVMValueRef>(null, context.Builder.BuildCall(function.Value, arguments.ToArray()));
        }

        static KeyValuePair<TypeSpec, LLVMValueRef> BuildOperatorCall(Context context, KeyValuePair<TypeSpec, LLVMValueRef> @operator, KeyValuePair<TypeSpec, LLVMValueRef> operand)
        {
            return new KeyValuePair<TypeSpec, LLVMValueRef>(null, context.Builder.BuildCall(@operator.Value, new LLVMValueRef[] { operand.Value }));
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
                foreach (var @operator in context.Definition.Operators.Values)
                    BuildOperatorDictionary(context, @operator);
        }

        static void BuildFunctionDictionary(DictionaryContext context, Function function)
        {
            var path = context.Path == "" ? function.Name : context.Path + "." + function.Name;
            if (!context.Dictionary.Functions.ContainsKey(path))
                context.Dictionary.Functions.Add(path, new List<Function>());

            context.Dictionary.Functions[path].Add(function);
            context.Dictionary.Sources.Add(path, context.Source);
        }

        static void BuildOperatorDictionary(DictionaryContext context, Operator @operator)
        {
            var path = context.Path + "." + @operator.Name;
            if (context.Dictionary.Operators.ContainsKey(path))
                throw new CompilerException($"The operator {path} was already defined.", @operator.Span);
            context.Dictionary.Operators.Add(path, @operator);
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

            if (definition.Type.Arguments == null)
            {
                if (definition.Functions != null)
                    foreach (var function in definition.Functions)
                        BuildFunctionType(context, null, function);

                if (definition.Operators != null)
                    foreach (var @operator in definition.Operators.Values)
                        BuildOperatorType(context, @operator.Definition.Type, @operator);
            }
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
                    BuildFunctionType(context, null, function);

            if (context.Source.Sources != null)
            {
                foreach (var source in context.Source.Sources)
                {
                    var newContext = new Context { Global = context.Global, Source = source };
                    BuildSourceTypes(newContext);
                }
            }
        }

        static LLVMTypeRef BuildOperatorType(Context context, TypeSpec operandTypeSpec, Operator @operator)
        {
            {
                var operandTypeName = QualifyName(context, operandTypeSpec.Name);
                var operatorDefinitionTypeName = QualifyName(context, @operator.Definition.Type.Name);
                if (operandTypeName != operatorDefinitionTypeName)
                    throw new CompilerException($"This operator expects a {operatorDefinitionTypeName}, and a {operandTypeName} was provided.", @operator.Span);
            }

            Dictionary<string, TypeSpec> genericTypeDictionary = null;
            if (operandTypeSpec.Arguments == null)
            {
                if (@operator.Definition.Type.Arguments != null)
                    throw new CompilerException($"This operator acts on a generic operand, and a non-generic operand was provided.", @operator.Span);
            }
            else
            {

                if (@operator.Definition.Type.Arguments == null)
                    throw new CompilerException($"This operator acts on a non-generic operand, and a generic operand was provided.", @operator.Span);

                if (operandTypeSpec.Arguments.Count != @operator.Definition.Type.Arguments.Count)
                    throw new CompilerException($"This operator has {@operator.Definition.Type.Arguments.Count} generic type arguments, and the operand has {operandTypeSpec.Arguments.Count}.", @operator.Span);

                genericTypeDictionary = new Dictionary<string, TypeSpec>();
                var genericArgumentEnumerator = @operator.Definition.Type.Arguments.GetEnumerator();
                foreach (var argument in operandTypeSpec.Arguments)
                {
                    genericArgumentEnumerator.MoveNext();
                    var genericArgument = genericArgumentEnumerator.Current;
                    if (genericTypeDictionary.ContainsKey(genericArgument.Name))
                        throw new CompilerException($"The generic type argument {genericArgument.Name} was already given.", @operator.Span);
                    genericTypeDictionary.Add(genericArgument.Name, argument);
                }
            }

            var operandType = ResolveType(context, genericTypeDictionary, operandTypeSpec);
            var parameterTypes = new LLVMTypeRef[] { operandType };
            var returnType = ResolveType(context, genericTypeDictionary, @operator.Routine.Result[0].TypeSpec);
            var functionType = LLVMTypeRef.CreateFunction(returnType, parameterTypes.ToArray());
            context.Global.Types.Add(@operator.Name, functionType);
            return functionType;
        }

        static void BuildSourceValues(Context context)
        {
            if (context.Source != null)
            {
                if (context.Source.Functions != null)
                    foreach (var function in context.Source.Functions)
                        BuildFunctionValue(context, null, function);

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
            if (definition.Type.Arguments == null)
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
                        BuildFunctionValue(context, null, function);

                if (definition.Operators != null)
                    foreach (var @operator in definition.Operators.Values)
                        BuildOperatorValue(context, definition.Type, @operator, QualifyOperatorName(context, @operator.Definition.Type, @operator));
            }
        }

        static void BuildOperatorValue(Context context, TypeSpec operandType, Operator @operator, string qualifiedName)
        {
            var operatorType = ResolveOperatorType(context, operandType, @operator, qualifiedName);
            context.Global.Values.Add(qualifiedName, new KeyValuePair<TypeSpec, LLVMValueRef>(null, context.Global.Module.AddFunction(qualifiedName, operatorType)));
        }

        static LLVMTypeRef ResolveOperatorType(Context context, TypeSpec operandType, Operator @operator, string qualifiedName)
        {
            if (context.Global.Types.ContainsKey(qualifiedName))
                return context.Global.Types[qualifiedName];
            var operatorType = BuildOperatorType(context, operandType, @operator);
            context.Global.Types.Add(qualifiedName, operatorType);
            return operatorType;
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
            BuildOperands(newContext, binding.Operation.Operands);
            newContext.Values.Add(binding.Identifier, newContext.TypedValue);
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
                return ResolveType(context, null, definition.Structure.Members[0].Type);

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
            builder.Append(']');

            return builder.ToString();
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