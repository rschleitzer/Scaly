using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace scalyc
{
    public class Modeler
    {
        public static Model Build(string[] files)
        {
            var modules = new List<Module>();
            foreach (string file in files)
            {
                modules.Add(BuildModule(file, true));
            }

            return new Model { Modules = modules.ToArray() };
        }

        private static Module BuildModule(string file, bool isPublic)
        {
            var source = System.IO.File.ReadAllText(file);
            var fileSyntax = parseFile(file, source);
            return BuildModule(fileSyntax, isPublic);
        }

        static Module BuildModule(FileSyntax fileSyntax, bool isPublic)
        {
            var module = new Module { IsPublic = isPublic, Name = Path.GetFileNameWithoutExtension(fileSyntax.file_name), };
            var origin = Path.GetDirectoryName(fileSyntax.file_name);
            if (fileSyntax.declarations != null)
            {
                foreach (var declaration in fileSyntax.declarations)
                {
                    switch (declaration)
                    {
                        case PrivateSyntax privateSyntax:
                            HandlePrivate(module, origin, privateSyntax);
                            break;
                        case ModuleSyntax moduleSyntax:
                            HandleModule(module, moduleSyntax, origin, false);
                            break;
                        case DefinitionSyntax definitionSyntax:
                            HandleDefinition(module, definitionSyntax, false);
                            break;
                        case UseSyntax useSyntax:
                            HandleUse(module, useSyntax);
                            break;
                        default:
                            throw new NotImplementedException($"{declaration.GetType()} not implemented.");
                    }
                }
            }

            return module;
        }

        static void HandleUse(Module module, UseSyntax useSyntax)
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

            if (module.Uses == null)
                module.Uses = new Dictionary<string, string[]>();
            if (module.Uses.ContainsKey(lastPart))
                throw new Exception($"{lastPart} cannot be re-used. Module {module.Name} {useSyntax.start.line}, {useSyntax.start.column} - {useSyntax.end.line}, {useSyntax.end.column}.");
            module.Uses.Add(lastPart, pathBuilder.ToArray());
        }

        static void HandlePrivate(Module module, string origin, PrivateSyntax privateSyntax)
        {
            switch (privateSyntax.export)
            {
                case ModuleSyntax moduleSyntax:
                    HandleModule(module, moduleSyntax, origin, true);
                    break;
                case DefinitionSyntax definitionSyntax:
                    HandleDefinition(module, definitionSyntax, true);
                    break;
                default:
                    throw new NotImplementedException($"{privateSyntax.export.GetType()} not implemented.");
            }
        }

        static void HandleModule(Module module, ModuleSyntax moduleSyntax, string origin, bool isPublic)
        {
            var moduleFile = Path.Combine(origin, ConvertNameToPath(moduleSyntax.name) + ".scaly");

            var submodule = BuildModule(moduleFile, isPublic);
            if (module.Submodules == null)
                module.Submodules = new Dictionary<string, Module>();
            if (moduleSyntax.name.extensions == null)
            {
                if (module.Submodules.ContainsKey(moduleSyntax.name.name))
                    throw new CompilerException($"Module {moduleSyntax.name.name} already defined.", moduleSyntax.start.line, moduleSyntax.start.column);
                module.Submodules.Add(moduleSyntax.name.name, submodule);
            }
            else
            {
                var extensionEnumerator = moduleSyntax.name.extensions.ToList().GetEnumerator();
                bool nextExtensionAvailable = extensionEnumerator.MoveNext();
                var extension = extensionEnumerator.Current;
                while (true)
                {
                    nextExtensionAvailable = extensionEnumerator.MoveNext();
                    if (module.Submodules == null)
                        module.Submodules = new Dictionary<string, Module>();
                    if (nextExtensionAvailable)
                    {
                        if (!module.Submodules.ContainsKey(extension.name))
                        {
                            module = new Module { IsPublic = isPublic, Name = extension.name, };
                        }
                        else
                        {
                            module = module.Submodules[extension.name];
                        }
                        extension = extensionEnumerator.Current;
                    }
                    else
                    {
                        if (module.Submodules.ContainsKey(extension.name))
                            throw new CompilerException($"Module {extension.name} already defined.", moduleSyntax.start.line, moduleSyntax.start.column);
                        module.Submodules.Add(extension.name, submodule);
                        break;
                    }
                }
            }
        }


        static void HandleDefinition(Module module, DefinitionSyntax definitionSyntax, bool isPublic)
        {
            switch (definitionSyntax.concept)
            {
                case ClassSyntax classSyntax:
                    HandleClass(module, classSyntax, isPublic);
                    break;
                case UnionSyntax unionSyntax:
                    HandleUnion(module, unionSyntax, isPublic);
                    break;
                default:
                    throw new NotImplementedException($"{definitionSyntax.concept.GetType()} not implemented.");
            }
        }

        static void HandleClass(Module module, ClassSyntax classSyntax, bool isPublic)
        {
        }

        static void HandleUnion(Module module, UnionSyntax unionSyntax, bool isPublic)
        {
        }

        static string ConvertNameToPath(NameSyntax nameSyntax)
        {
            string path = nameSyntax.name;
            foreach (var extension in nameSyntax.extensions)
                path = Path.Combine(path, extension.name);
            return path;
        }

        static FileSyntax parseFile(string file_name, string text)
        {
            var parser = new Parser(text);

            try
            {
                return parser.parse_file(file_name);
            }
            catch (ParserException e)
            {
                throw new CompilerException(file_name, e.line, e.column);
            }
        }
    }

    public class Model
    {
        public Module[] Modules;

    }

    public class Module
    {
        public bool IsPublic;
        public string Name;
        public Dictionary<string, Module> Submodules;
        public Dictionary<string, string[]> Uses;
    }
}
