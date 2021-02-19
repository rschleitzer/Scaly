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
        public TypeModel Type;
        public Dictionary<string, Definition> Definitions;
        public Dictionary<string, List<Function>> Functions;
        public List<Routine> Operators;
        public Dictionary<string, Implementation> Implementations;
        public List<Source> Sources;
    }

    public class Function
    {
        public string Name;
        public Routine Routine;
    }

    public class Routine
    {
    }

    public class Implementation
    {
        public string Name;
        public Dictionary<string, List<Function>> Functions;
        public List<Routine> Operators;
    }

    public class TypeModel
    {
        public string Name;
    }

    public class Source
    {
        public string FileName;
        public Dictionary<string, string[]> Uses;
        public List<string[]> Usings;
        public Dictionary<string, Definition> Definitions;
        public Dictionary<string, List<Function>> Functions;
        public List<Routine> Operators;
        public Dictionary<string, Implementation> Implementations;
        public List<Source> Sources;
    }

    public class Modeler
    {
        public static Definition BuildFiles(string[] files)
        {
            return new Definition { Sources = files.ToList().ConvertAll(it => BuildSource(it)) };
        }

        public static Definition BuildProgram(string program)
        {
            var fileSyntax = parseFile("", program);
            var source = BuildSource(fileSyntax);

            var runtime = BuildSource("scaly.scaly");
            return new Definition { Sources = new List<Source> { source, runtime } };
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
            var source = new Source { FileName = Path.GetFileName(fileSyntax.file_name) };
            var origin = Path.GetDirectoryName(fileSyntax.file_name);
            if (fileSyntax.declarations != null)
            {
                foreach (var declaration in fileSyntax.declarations)
                {
                    HandleDeclaration(source, origin, declaration);
                }
            }

            return source;
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
                        source.Functions = new Dictionary<string, List<Function>>();
                    HandleFunction(functionSyntax, source.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (source.Functions == null)
                        source.Functions = new Dictionary<string, List<Function>>();
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
                    if (source.Implementations == null)
                        source.Implementations = new Dictionary<string, Implementation>();
                    HandleImplement(implementSyntax, source.Implementations);
                    break;
                default:
                    throw new NotImplementedException($"{declaration.GetType()} not implemented.");
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
                    var definition = new Definition { IsPublic = isPublic, Type = new TypeModel { Name = moduleSyntax.name.name }, Definitions = new Dictionary<string, Definition>(), Sources = null };
                    definitions.Add(definition.Type.Name, definition);
                }

                switch (definitions[moduleSyntax.name.name])
                {
                    case Definition definition:
                        BuildModuleRecursive(moduleSyntax, Path.Combine(origin, definition.Type.Name), extensionEnumerator.Current.name, extensionEnumerator, definition, isPublic);
                        break;
                    default:
                        throw new CompilerException($"A concept with name {moduleSyntax.name.name} already defined.", moduleSyntax.start.line, moduleSyntax.start.column);
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
                        source.Functions = new Dictionary<string, List<Function>>();
                    HandleFunction(functionSyntax, source.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (source.Functions == null)
                        source.Functions = new Dictionary<string, List<Function>>();
                    HandleProcedure(procedureSyntax, source.Functions, false);
                    break;
                case ModuleSyntax moduleSyntax:
                    HandleModule(moduleSyntax, source.Definitions, source, origin, false);
                    break;
                default:
                    throw new NotImplementedException($"{privateSyntax.export.GetType()} not implemented.");
            }
        }

        static void HandleFunction(FunctionSyntax functionSyntax, Dictionary<string, List<Function>> functions, bool isPublic)
        {
            BuildFunction(functionSyntax.name, functionSyntax.generics, functionSyntax.routine, functions, isPublic, false);
        }

        static void HandleProcedure(ProcedureSyntax procedureSyntax, Dictionary<string, List<Function>> functions, bool isPublic)
        {
            BuildFunction(procedureSyntax.name, procedureSyntax.generics, procedureSyntax.routine, functions, isPublic, true);
        }

        static void BuildFunction(string name, GenericArgumentsSyntax generics, RoutineSyntax routine, Dictionary<string, List<Function>> functions, bool isPublic, bool isModifying)
        {
            if (!functions.ContainsKey(name))
                functions.Add(name, new List<Function>());
            var function = new Function { Name = name };
            functions[name].Add(function);
        }

        static void HandleOperator(OperatorSyntax operatorSyntax, List<Routine> operators)
        {
            var routine = new Routine();
            operators.Add(routine);
        }

        static void HandleImplement(ImplementSyntax implementSyntax, Dictionary<string, Implementation> implementations)
        {
            if (implementations.ContainsKey(implementSyntax.type.name.name))
                throw new CompilerException($"An implementation with name {implementSyntax.type.name.name} already defined.", implementSyntax.start.line, implementSyntax.start.column);

            var implementation = new Implementation { Name = implementSyntax.type.name.name };
            implementations.Add(implementation.Name, implementation);

            if (implementSyntax.functions != null)
            {
                foreach (var method in implementSyntax.functions)
                {
                    switch (method)
                    {
                        case FunctionSyntax functionSyntax:
                            if (implementation.Functions == null)
                                implementation.Functions = new Dictionary<string, List<Function>>();
                            HandleFunction(functionSyntax, implementation.Functions, false);
                            break;
                        case ProcedureSyntax procedureSyntax:
                            if (implementation.Functions == null)
                                implementation.Functions = new Dictionary<string, List<Function>>();
                            HandleProcedure(procedureSyntax, implementation.Functions, false);
                            break;
                        case OperatorSyntax operatorSyntax:
                            if (implementation.Operators == null)
                                implementation.Operators = new List<Routine>();
                            HandleOperator(operatorSyntax, implementation.Operators);
                            break;
                        default:
                            throw new NotImplementedException($"{method.GetType()} not implemented.");
                    }
                }
            }
        }

        static void HandleDefinition(DefinitionSyntax definitionSyntax, Dictionary<string, Definition> definitions, Source source, string origin, bool isPublic)
        {

            TypeModel typeModel = BuildType(definitionSyntax.type);
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
                throw new CompilerException($"Module {typeModel.Name} already defined.", definitionSyntax.start.line, definitionSyntax.start.column);

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
                        definition.Functions = new Dictionary<string, List<Function>>();
                    HandleFunction(functionSyntax, definition.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new Dictionary<string, List<Function>>();
                    HandleProcedure(procedureSyntax, definition.Functions, false);
                    break;
                case OperatorSyntax operatorSyntax:
                    if (definition.Operators == null)
                        definition.Operators = new List<Routine>();
                    HandleOperator(operatorSyntax, definition.Operators);
                    break;
                case ImplementSyntax implementSyntax:
                    if (definition.Implementations == null)
                        definition.Implementations = new Dictionary<string, Implementation>();
                    HandleImplement(implementSyntax, definition.Implementations);
                    break;
                default:
                    throw new NotImplementedException($"{declaration.GetType()} not implemented.");
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
                        definition.Functions = new Dictionary<string, List<Function>>();
                    HandleFunction(functionSyntax, definition.Functions, false);
                    break;
                case ProcedureSyntax procedureSyntax:
                    if (definition.Functions == null)
                        definition.Functions = new Dictionary<string, List<Function>>();
                    HandleProcedure(procedureSyntax, definition.Functions, false);
                    break;
                case ModuleSyntax moduleSyntax:
                    HandleModule(moduleSyntax, definition.Definitions, source, origin, false);
                    break;
                default:
                    throw new NotImplementedException($"{privateSyntax.export.GetType()} not implemented.");
            }
        }

        static TypeModel BuildType(TypeSyntax type)
        {
            return new TypeModel { Name = ExpandName(type.name) };
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

            try
            {
                var fileSyntax = parser.parse_file(file_name);
                if (!parser.is_at_end())
                    throw new CompilerException(file_name, parser.get_current_line(), parser.get_current_column());
                return fileSyntax;
            }
            catch (ParserException e)
            {
                throw new CompilerException(file_name, e.line, e.column);
            }
        }
    }

}
