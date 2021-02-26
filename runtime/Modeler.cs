using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Scaly.Compiler
{
    public class Definition
    {
        public bool IsPublic;
        public TypeSpec Type;
        public Dictionary<string, Definition> Definitions;
        public List<Function> Functions;
        public List<Routine> Operators;
        public Dictionary<string, Implement> Implements;
        public List<Source> Sources;
    }

    public class Function
    {
        public string Name;
        public Routine Routine;
    }

    public class Routine
    {
        public List<Property> Input;
        public List<Property> Result;
        public TypeSpec Error;
        public List<Operation> Operations;
    }

    public class Property
    {
        public string Name;
        public TypeSpec TypeSpec;
    }

    public class TypeSpec
    {
        public TypeSyntax Syntax;
        public string Name;
        public List<TypeSpec> Arguments;
    }


    public class Implement
    {
        public string Name;
        public List<Function> Functions;
        public List<Routine> Operators;
    }

    public class Source
    {
        public string FileName;
        public Dictionary<string, string[]> Uses;
        public List<string[]> Usings;
        public Dictionary<string, Definition> Definitions;
        public List<Function> Functions;
        public List<Routine> Operators;
        public Dictionary<string, Implement> Implements;
        public List<Source> Sources;
        public List<Operation> Operations;
    }

    public abstract class Expression { }

    public class Name : Expression
    {
        public NameSyntax Syntax;
        public List<string> Path;
    }

    public class Object : Expression
    {
        public List<Component> Components;
    }

    public class Component
    {
        public string Name;
        public List<Operand> Value;
    }

    public enum BindingType
    {
        Let,
        Var,
        Mutable
    }

    public class Scope : Expression
    {
        public List<Operation> Operations;
        public Binding Binding;
    }

    public class Binding : Expression
    {
        public BindingType BindingType;
        public string Identifier;
        public Operation Operation;
        public List<Operation> Operations;
    }

    public class Operation : Expression
    {
        public List<Operand> Operands = new List<Operand>();
    }

    public class Operand
    {
        public Expression Expression;
        public List<Postfix> Postfixes;
    }

    public abstract class Postfix { }

    public class MemberAccess
    {
        public string Name;
    }

    public class Catcher
    {
        public List<Catch> Catches;
        public Drop Drop;
    }

    public class Catch
    {
        public Operation Condition;
        public Operation Handler;
    }

    public class Drop
    {
        public Operation Handler;
    }

    public class Is
    {
        public List<Operand> Condition;
    }

    public enum IntegerType
    {
        Integer32,
    }

    public class IntegerConstant : Expression
    {
        public IntegerType Type;
        public int Value;
    }

    public class Modeler
    {
        public static Definition BuildFiles(string[] files)
        {
            var sources = files.ToList().ConvertAll(it => BuildSource(it));
            sources.Add(BuildRuntime());
            return new Definition { Sources = sources };
        }

        public static Source BuildRuntime()
        {
            var runtime = BuildSource("scaly.scaly");
            return runtime;
        }

        public static Source BuildProgram(string program)
        {
            var fileSyntax = parseFile("", program);
            var source = BuildSource(fileSyntax);
            var operations = source.Operations;
            source = new Source
            {
                FileName = source.FileName,
                Uses = source.Uses,
                Usings = source.Usings,
                Definitions = source.Definitions,
                Functions = source.Functions,
                Operators = source.Operators,
                Implements = null,
                Sources = source.Sources,
                Operations = null,
            };

            if (source.Functions == null)
                source.Functions = new List<Function>();
            source.Functions.Add(MakeMainFunction(operations));
            return source;
        }

        static Function MakeMainFunction(List<Operation> operations)
        {
            var main = new Function
            {
                Name = "main",
                Routine = new Routine
                {
                    Input = new List<Property>()
                    {
                        new Property { Name = "argument count", TypeSpec = new TypeSpec { Name = "Integer" } },
                        new Property { Name = "argument values", TypeSpec = new TypeSpec { Name = "Pointer", Arguments = new List<TypeSpec> { new TypeSpec { Name = "Pointer", Arguments = new List<TypeSpec> { new TypeSpec { Name = "Byte" } } } } } },
                    },
                    Result = new List<Property>() { new Property { TypeSpec = new TypeSpec { Name = "Integer" } } },
                    Operations = operations
                }
            };

            return main;
        }

        static Source BuildSource(string file)
        {
            var text = System.IO.File.ReadAllText(file);
            return BuildSource(text, file);
        }

        private static Source BuildSource(string text, string file)
        {
            var fileSyntax = parseFile(file, text);
            return BuildSource(fileSyntax);
        }

        static Source BuildSource(FileSyntax fileSyntax)
        {
            var source = new Source { FileName = Path.GetFileName(fileSyntax.file) };
            var origin = Path.GetDirectoryName(fileSyntax.file);
            if (fileSyntax.declarations != null)
                foreach (var declaration in fileSyntax.declarations)
                    HandleDeclaration(source, origin, declaration);

            if (fileSyntax.statements != null)
            {
                if (source.Operations == null)
                    source.Operations = new List<Operation>();
                foreach (var statement in fileSyntax.statements)
                    HandleStatement(source.Operations, statement);
            }

            return source;
        }

        static void HandleStatement(List<Operation> operations, object statement)
        {
            switch (statement)
            {
                case OperationSyntax operationSyntax:
                    HandleOperation(operations, operationSyntax);
                    break;

                default:
                    throw new NotImplementedException($"{statement.GetType()} is not implemented.");
            }
        }

        static void HandleOperation(List<Operation> operations, OperationSyntax operationSyntax)
        {
            operations.Add(BuildOperation(operationSyntax));
        }

        static Operand BuildOperand(OperandSyntax operandSyntax)
        {
            var expression = BuildExpression(operandSyntax.expression);
            List<Postfix> postfixes = null;
            if (operandSyntax.postfixes != null)
            {
                foreach (var postfix in operandSyntax.postfixes)
                {
                    postfixes.Add(BuildPostfix(postfix));
                }
            }
            return new Operand { Expression = expression, Postfixes = postfixes };
        }

        static Expression BuildExpression(object expression)
        {
            switch (expression)
            {
                case NameSyntax nameSyntax:
                    return BuildName(nameSyntax);
                case LiteralSyntax literalSyntax:
                    return BuildConstant(literalSyntax);
                case BlockSyntax blockSyntax:
                    return BuildBlock(blockSyntax);
                case ObjectSyntax objectSyntax:
                    return BuildObject(objectSyntax);
                default:
                    throw new NotImplementedException($"{expression.GetType()} is not implemented.");
            }
        }

        static Expression BuildName(NameSyntax nameSyntax)
        {
            var components = new List<string>();
            components.Add(nameSyntax.name);
            if (nameSyntax.extensions != null)
                components.AddRange(nameSyntax.extensions.ToList().ConvertAll(it => it.name));

            return new Name { Path = components, Syntax = nameSyntax };
        }

        static Expression BuildBlock(BlockSyntax blockSyntax)
        {
            var scope = new Scope();
            if (blockSyntax.statements != null)
            {
                scope.Operations = new List<Operation>();
                foreach (var statement in blockSyntax.statements)
                {
                    HandleStatement(scope.Operations, statement);
                }
            }

            return scope;
        }

        static Expression BuildObject(ObjectSyntax objectSyntax)
        {
            var @object = new Object { };
            if (objectSyntax.components != null)
                @object.Components = objectSyntax.components.ToList().ConvertAll(it => BuildComponent(it));
            return @object;
        }

        static Component BuildComponent(ComponentSyntax componentSyntax)
        {
            var component = new Component();
            if (componentSyntax.value != null)
            {
                if (componentSyntax.operands.Length != 1)
                    throw new CompilerException("Only one component supported currently.", componentSyntax.file, componentSyntax.start.line, componentSyntax.start.column, componentSyntax.end.line, componentSyntax.end.column);
            }
            else
            {
            }
            return component;
        }

        static Expression BuildConstant(LiteralSyntax literalSyntax)
        {
            switch (literalSyntax.literal)
            {
                case Integer integer:
                    return new IntegerConstant { Type = IntegerType.Integer32, Value = int.Parse(integer.value) };
                default:
                    throw new NotImplementedException($"{literalSyntax.literal.GetType()} is not implemented.");
            }
        }

        static Postfix BuildPostfix(object postfix)
        {
            switch (postfix)
            {
                default:
                    throw new NotImplementedException($"{postfix.GetType()} is not implemented.");
            }
        }

        static void HandleDeclaration(Source source, string origin, object declaration)
        {
            switch (declaration)
            {
                case PrivateSyntax privateSyntax:
                    HandlePrivate(source, origin, privateSyntax);
                    break;
                case DefinitionSyntax definitionSyntax:
                    if (source.Definitions == null)
                        source.Definitions = new Dictionary<string, Definition>();
                    HandleDefinition(definitionSyntax, source.Definitions, source, origin, true);
                    break;
                case FunctionSyntax functionSyntax:
                    if (source.Functions == null)
                        source.Functions = new List<Function>();
                    HandleFunction(functionSyntax, source.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (source.Functions == null)
                        source.Functions = new List<Function>();
                    HandleProcedure(procedureSyntax, source.Functions, false);
                    break;
                case OperatorSyntax operatorSyntax:
                    if (source.Operators == null)
                        source.Operators = new List<Routine>();
                    HandleOperator(operatorSyntax, source.Operators);
                    break;
                case ModuleSyntax moduleSyntax:
                    if (source.Definitions == null)
                        source.Definitions = new Dictionary<string, Definition>();
                    HandleModule(moduleSyntax, source.Definitions, source, origin, true);
                    break;
                case UseSyntax useSyntax:
                    HandleUse(source, useSyntax);
                    break;
                case ImplementSyntax implementSyntax:
                    if (source.Implements == null)
                        source.Implements = new Dictionary<string, Implement>();
                    HandleImplement(implementSyntax, source.Implements);
                    break;
                default:
                    throw new NotImplementedException($"{declaration.GetType()} is not implemented.");
            }
        }

        static void HandleModule(ModuleSyntax moduleSyntax, Dictionary<string, Definition> definitions, Source source, string origin, bool isPublic)
        {
            var extensionEnumerator = moduleSyntax.name.extensions.ToList().GetEnumerator();
            var extensionAvailable = extensionEnumerator.MoveNext();
            if (extensionAvailable)
            {
                if (definitions == null)
                    definitions = new Dictionary<string, Definition>();
                if (!definitions.ContainsKey(moduleSyntax.name.name))
                {
                    var definition = new Definition { IsPublic = isPublic, Type = new TypeSpec { Name = moduleSyntax.name.name }, Definitions = new Dictionary<string, Definition>(), Sources = null };
                    definitions.Add(definition.Type.Name, definition);
                }

                switch (definitions[moduleSyntax.name.name])
                {
                    case Definition definition:
                        BuildModuleRecursive(moduleSyntax, Path.Combine(origin, definition.Type.Name), extensionEnumerator.Current.name, extensionEnumerator, definition, isPublic);
                        break;
                    default:
                        throw new CompilerException($"A concept with name {moduleSyntax.name.name} already defined.", moduleSyntax.file, moduleSyntax.start.line, moduleSyntax.start.column, moduleSyntax.end.line, moduleSyntax.end.column);
                }
            }
            else
            {
                var moduleFile = Path.Combine(Path.Combine(origin, moduleSyntax.name.name) + ".scaly");
                var moduleSource = BuildSource(moduleFile);
                source.Sources.Add(moduleSource);
            }
        }

        static void BuildModuleRecursive(ModuleSyntax moduleSyntax, string path, string name, List<ExtensionSyntax>.Enumerator extensionEnumerator, Definition definition, bool isPublic)
        {
            var extensionAvailable = extensionEnumerator.MoveNext();
            if (extensionAvailable)
            {
                BuildModuleRecursive(moduleSyntax, Path.Combine(path, name), extensionEnumerator.Current.name, extensionEnumerator, definition, isPublic);
            }
            else
            {
                var moduleFile = Path.Combine(Path.Combine(path, name) + ".scaly");
                var moduleSource = BuildSource(moduleFile);
                if (definition.Sources == null)
                    definition.Sources = new List<Source>();
                definition.Sources.Add(moduleSource);
            }
        }

        static void HandleUse(Source source, UseSyntax useSyntax)
        {
            var pathBuilder = new List<string>{ useSyntax.name.name };
            var lastPart = useSyntax.name.name;
            if (useSyntax.name.extensions != null)
            {
                foreach (var extension in useSyntax.name.extensions)
                {
                    pathBuilder.Add(extension.name);
                    lastPart = extension.name;
                }
            }

            if (lastPart == "*")
            {
                if (source.Usings == null)
                    source.Usings = new List<string[]>();
                source.Usings.Add(pathBuilder.ToArray());
            }
            else
            {
                if (source.Uses == null)
                    source.Uses = new Dictionary<string, string[]>();
                if (source.Uses.ContainsKey(lastPart))
                    throw new Exception($"{lastPart} cannot be re-used. Module {source.FileName} at {useSyntax.start.line}, {useSyntax.start.column} - {useSyntax.end.line}, {useSyntax.end.column}.");
                source.Uses.Add(lastPart, pathBuilder.ToArray());
            }
        }

        static void HandlePrivate(Source source, string origin, PrivateSyntax privateSyntax)
        {
            switch (privateSyntax.export)
            {
                case DefinitionSyntax definitionSyntax:
                    HandleDefinition(definitionSyntax, source.Definitions, source, origin, false);
                    break;
                case FunctionSyntax functionSyntax:
                    if (source.Functions == null)
                        source.Functions = new List<Function>();
                    HandleFunction(functionSyntax, source.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (source.Functions == null)
                        source.Functions = new List<Function>();
                    HandleProcedure(procedureSyntax, source.Functions, false);
                    break;
                case ModuleSyntax moduleSyntax:
                    HandleModule(moduleSyntax, source.Definitions, source, origin, false);
                    break;
                default:
                    throw new NotImplementedException($"{privateSyntax.export.GetType()} is not implemented.");
            }
        }

        static void HandleFunction(FunctionSyntax functionSyntax, List<Function> functions, bool isPublic)
        {
            BuildFunction(functionSyntax.name, functionSyntax.generics, functionSyntax.routine, functions, isPublic, false);
        }

        static void HandleProcedure(ProcedureSyntax procedureSyntax, List<Function> functions, bool isPublic)
        {
            BuildFunction(procedureSyntax.name, procedureSyntax.generics, procedureSyntax.routine, functions, isPublic, true);
        }

        static void BuildFunction(string name, GenericArgumentsSyntax generics, RoutineSyntax routine, List<Function> functions, bool isPublic, bool isModifying)
        {
            var function = new Function { Name = name };
            function.Routine = BuildRoutine(routine);
            functions.Add(function);
        }

        static Routine BuildRoutine(RoutineSyntax routineSyntax)
        {
            var routine = new Routine {};
            if (routineSyntax.parameters != null)
                routine.Input = BuildParameters(routineSyntax.parameters);

            if (routineSyntax.returns != null)
                routine.Result = BuildParameters(routineSyntax.returns.parameters);

            if (routineSyntax.throws != null)
                routine.Error = BuildType(routineSyntax.throws.type);

            switch (routineSyntax.implementation)
            {
                case OperationSyntax operationSyntax:
                    routine.Operations = new List<Operation> { BuildOperation(operationSyntax) };
                    break;
                case ExternSyntax _:
                    break;
                case SetSyntax _:
                    break;
                default:
                    throw new NotImplementedException($"{routineSyntax.implementation.GetType()} is not implemented.");
            }

            return routine;
        }

        static Operation BuildOperation(OperationSyntax operationSyntax)
        {
            var operation = new Operation { Operands = new List<Operand>() };
            foreach (var operand in operationSyntax.operands)
            {
                operation.Operands.Add(BuildOperand(operand));
            }
            return operation;
        }

        static List<Property> BuildParameters(object parameters)
        {
            switch (parameters)
            {
                case ParametersSyntax parametersSyntax:
                    if (parametersSyntax.properties != null)
                        return parametersSyntax.properties.ToList().ConvertAll(it => BuildProperty(it));
                    else
                        return null;
                case PropertySyntax propertySyntax:
                    return new List<Property> { BuildProperty(propertySyntax) };
                case TypeSyntax typeSyntax:
                    return new List<Property> { new Property { TypeSpec = BuildTypeSpec(typeSyntax) } };
                default:
                    throw new NotImplementedException($"{parameters.GetType()} is not implemented.");
            }
        }

        static Property BuildProperty(PropertySyntax propertySyntax)
        {
            var property = new Property { Name = propertySyntax.name };
            
            if (propertySyntax.annotation != null)
                property.TypeSpec = BuildTypeSpec(propertySyntax.annotation.spec);

            return property;
        }

        static TypeSpec BuildTypeSpec(object spec)
        {
            switch (spec)
            {
                case TypeSyntax typeSyntax:
                    return BuildType(typeSyntax);
                default:
                    throw new NotImplementedException($"{spec.GetType()} is not implemented.");
            }
        }

        static TypeSpec BuildType(TypeSyntax typeSyntax)
        {
            var typeSpec = new TypeSpec { Name = typeSyntax.name.name, Syntax = typeSyntax };
            if (typeSyntax.generics != null)
                typeSpec.Arguments = typeSyntax.generics.generics.ToList().ConvertAll(it => BuildType(it.spec));

            return typeSpec;
        }

        static void HandleOperator(OperatorSyntax operatorSyntax, List<Routine> operators)
        {
            var routine = new Routine();
            operators.Add(routine);
        }

        static void HandleImplement(ImplementSyntax implementSyntax, Dictionary<string, Implement> implementations)
        {
            if (implementations.ContainsKey(implementSyntax.type.name.name))
                throw new CompilerException($"An implementation with name {implementSyntax.type.name.name} already defined.", implementSyntax.file, implementSyntax.start.line, implementSyntax.start.column, implementSyntax.end.line, implementSyntax.end.column);

            var implementation = new Implement { Name = implementSyntax.type.name.name };
            implementations.Add(implementation.Name, implementation);

            if (implementSyntax.methods != null)
            {
                foreach (var method in implementSyntax.methods)
                {
                    switch (method)
                    {
                        case FunctionSyntax functionSyntax:
                            if (implementation.Functions == null)
                                implementation.Functions = new List<Function>();
                            HandleFunction(functionSyntax, implementation.Functions, false);
                            break;
                        case ProcedureSyntax procedureSyntax:
                            if (implementation.Functions == null)
                                implementation.Functions = new List<Function>();
                            HandleProcedure(procedureSyntax, implementation.Functions, false);
                            break;
                        case OperatorSyntax operatorSyntax:
                            if (implementation.Operators == null)
                                implementation.Operators = new List<Routine>();
                            HandleOperator(operatorSyntax, implementation.Operators);
                            break;
                        default:
                            throw new NotImplementedException($"{method.GetType()} is not implemented.");
                    }
                }
            }
        }

        static void HandleDefinition(DefinitionSyntax definitionSyntax, Dictionary<string, Definition> definitions, Source source, string origin, bool isPublic)
        {

            TypeSpec typeModel = BuildType(definitionSyntax.type);
            BodySyntax bodySyntax = null;
            switch (definitionSyntax.concept)
            {
                case ClassSyntax classSyntax:
                    bodySyntax = classSyntax.body;
                    break;

                case NamespaceSyntax namespaceSyntax:
                    bodySyntax = namespaceSyntax.body;
                    break;

                case UnionSyntax unionSyntax:
                    bodySyntax = unionSyntax.body;
                    break;
            }

            if (definitions.ContainsKey(typeModel.Name))
                throw new CompilerException($"Module {typeModel.Name} already defined.", definitionSyntax.file, definitionSyntax.start.line, definitionSyntax.start.column, definitionSyntax.end.line, definitionSyntax.end.column);

            var definition = new Definition { };
            definitions.Add(typeModel.Name, definition);

            if ((bodySyntax != null) && (bodySyntax.declarations != null))
            {
                foreach (var declaration in bodySyntax.declarations)
                {
                    HandleDeclaration(definition, source, origin, declaration);
                }
            }
        }

        static void HandleDeclaration(Definition definition, Source source, string origin, object declaration)
        {
            switch (declaration)
            {
                case PrivateSyntax privateSyntax:
                    HandlePrivate(definition, source, origin, privateSyntax);
                    break;
                case DefinitionSyntax definitionSyntax:
                    if (definition.Definitions == null)
                        definition.Definitions = new Dictionary<string, Definition>();
                    HandleDefinition(definitionSyntax, definition.Definitions, source, origin, true);
                    break;
                case FunctionSyntax functionSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new List<Function>();
                    HandleFunction(functionSyntax, definition.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new List<Function>();
                    HandleProcedure(procedureSyntax, definition.Functions, false);
                    break;
                case OperatorSyntax operatorSyntax:
                    if (definition.Operators == null)
                        definition.Operators = new List<Routine>();
                    HandleOperator(operatorSyntax, definition.Operators);
                    break;
                case ImplementSyntax implementSyntax:
                    if (definition.Implements == null)
                        definition.Implements = new Dictionary<string, Implement>();
                    HandleImplement(implementSyntax, definition.Implements);
                    break;
                default:
                    throw new NotImplementedException($"{declaration.GetType()} is not implemented.");
            }
        }

        static void HandlePrivate(Definition definition, Source source, string origin, PrivateSyntax privateSyntax)
        {
            switch (privateSyntax.export)
            {
                case DefinitionSyntax definitionSyntax:
                    if (definition.Definitions == null)
                        definition.Definitions = new Dictionary<string, Definition>();
                    HandleDefinition(definitionSyntax, definition.Definitions, source, origin, false);
                    break;
                case FunctionSyntax functionSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new List<Function>();
                    HandleFunction(functionSyntax, definition.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new List<Function>();
                    HandleProcedure(procedureSyntax, definition.Functions, false);
                    break;
                case ModuleSyntax moduleSyntax:
                    HandleModule(moduleSyntax, definition.Definitions, source, origin, false);
                    break;
                default:
                    throw new NotImplementedException($"{privateSyntax.export.GetType()} is not implemented.");
            }
        }

        static string ExpandName(NameSyntax name)
        {
            var ret = name.name;
            if (name.extensions != null)
                ret += string.Join('.', name.extensions.ToList());
            return ret;
        }

        static FileSyntax parseFile(string file_name, string text)
        {
            var parser = new Parser(text);

            var fileSyntax = parser.parse_file(file_name);
            if (!parser.is_at_end())
                throw new CompilerException("Unexpected content at end of file.", file_name, parser.get_current_line(), parser.get_current_column(), parser.get_current_line(), parser.get_current_column());
            return fileSyntax;
        }
    }
}
