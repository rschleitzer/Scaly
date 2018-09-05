using System.Collections.Generic;

namespace scalyc
{
    class ProgramModel
    {
        public string name;
        public Dictionary<string, NamespaceModel> namespaces = new Dictionary<string, NamespaceModel>();
        public Dictionary<string, ClassModel> classes = new Dictionary<string, ClassModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
    }

    class NamespaceModel
    {
        public string name;
        public NamespaceModel parentNamespace;
        public Dictionary<string, NamespaceModel> subNamespaces = new Dictionary<string, NamespaceModel>();
        public Dictionary<string, ClassModel> classes = new Dictionary<string, ClassModel>();
    }    

    class ClassModel
    {
        public string name;
        public NamespaceModel parentNamespace;
        public ClassModel parentClass;
        public List<AllocatorModel> allocators = new List<AllocatorModel>();
        public Dictionary<string, ClassModel> subClasses = new Dictionary<string, ClassModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
    }

    class AllocatorModel
    {
        public ClassModel parentClass;
        public StructureModel structure;
    }

    class StructureModel
    {
        public Dictionary<string, ComponentModel> components = new Dictionary<string, ComponentModel>();
    }

    class ComponentModel
    {
        public string name;
        public List<TypeModel> types = new List<TypeModel>();
    }

    class TypeModel
    {
        public List<string> names = new List<string>();
        public List<TypeModel> generics = new List<TypeModel>();
    }

    class FunctionModel
    {
        public string name;
        public NamespaceModel parentNamespace;
        public ClassModel parentClass;
    }

    class ModelVisitor : SyntaxVisitor
    {
        public ProgramModel programModel;

        string currentFile;
        Stack<NamespaceModel> namespaceStack = new Stack<NamespaceModel>();
        Stack<ClassModel> classStack = new Stack<ClassModel>();
        AllocatorModel currentAllocator;
        StructureModel currentStructure;
        ComponentModel currentComponent;
        Stack<TypeModel> typeStack = new Stack<TypeModel>();
        Stack<TypeModel> genericStack = new Stack<TypeModel>();


        public override bool openProgram(ProgramSyntax programSyntax)
        {
            programModel = new ProgramModel { name = programSyntax.name };
            return true;
        }

        public override bool openFile(FileSyntax fileSyntax)
        {
            currentFile = fileSyntax.fileName;
            return true;
        }

        public override bool openNamespace(NamespaceSyntax namespaceSyntax)
        {
            var name = namespaceSyntax.name.name;
            NamespaceModel currentNamespace = null;

            NamespaceModel theNamespace = AddNamespace(namespaceSyntax.name.name);
            currentNamespace = theNamespace;

            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    AddNamespace(extension.name);

            foreach (var statement in namespaceSyntax.statements)
                statement.accept(this);

            namespaceStack.Pop();
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    namespaceStack.Pop();

            return false;
        }

        NamespaceModel AddNamespace(string name)
        {
            NamespaceModel theNamespace = null;
            if (namespaceStack.Count > 0)
            {
                var currentNamespace = namespaceStack.Peek();
                if (currentNamespace.subNamespaces.ContainsKey(name))
                {
                    theNamespace = currentNamespace.subNamespaces[name];
                }
                else
                {
                    theNamespace = new NamespaceModel() { name = name, parentNamespace = currentNamespace };
                    currentNamespace.subNamespaces.Add(name, theNamespace);
                }
            }
            else
            {
                if (programModel.namespaces.ContainsKey(name))
                {
                    theNamespace = programModel.namespaces[name];
                }
                else
                {
                    theNamespace = new NamespaceModel() { name = name };
                    programModel.namespaces.Add(name, theNamespace);
                }
            }

            namespaceStack.Push(theNamespace);

            return theNamespace;
        }

        public override bool openClass(ClassSyntax classSyntax)
        {
            ClassModel theClass = null;

            var name = classSyntax.name.name;
            if (classSyntax.name.extensions != null)
            {
                foreach (var extension in classSyntax.name.extensions)
                {
                    AddNamespace(name);
                    name = extension.name;
                }
            }

            ClassModel currentClass = null;
            NamespaceModel currentNamespace = null;

            if (classStack.Count > 0)
            {
                currentClass = classStack.Peek();
                if (currentClass.subClasses.ContainsKey(name))
                {
                    theClass = currentClass.subClasses[name];
                }
            }
            else
            {
                if (namespaceStack.Count > 0)
                {
                    currentNamespace = namespaceStack.Peek();
                    if (currentNamespace.classes.ContainsKey(name))
                    {
                        theClass = currentNamespace.classes[name];
                    }
                }
                else
                {
                    if (programModel.classes.ContainsKey(name))
                    {
                        theClass = programModel.classes[name];
                    }
                }
            }

            if (theClass == null)
            {
                theClass = new ClassModel() { name = name, parentClass = currentClass, parentNamespace = currentNamespace };

                if (currentClass != null)
                    currentClass.subClasses.Add(name, theClass);

                else if (currentNamespace != null)
                    currentNamespace.classes.Add(name, theClass);

                else
                    programModel.classes.Add(name, theClass);
            }

            classStack.Push(theClass);

            if (classSyntax.contents != null)
                classSyntax.contents.accept(this);
            
            if (classSyntax.body != null)
                classSyntax.body.accept(this);

            classStack.Pop();

            if (classSyntax.name.extensions != null)
                foreach (var extension in classSyntax.name.extensions)
                    namespaceStack.Pop();

            return false;
        }

        public override bool openAllocator(AllocatorSyntax allocatorSyntax)
        {
            var currentClass = classStack.Peek();
            currentAllocator = new AllocatorModel() { parentClass = currentClass };
            currentClass.allocators.Add(currentAllocator);

            return true;
        }

        public override void closeAllocator(AllocatorSyntax allocatorSyntax)
        {
            currentAllocator = null;
        }

        public override bool openStructure(StructureSyntax structureSyntax)
        {
            currentStructure = new StructureModel();
            if (currentAllocator != null)
                currentAllocator.structure = currentStructure;

            return true;
        }

        public override void closeStructure(StructureSyntax structureSyntax)
        {
            currentStructure = null;
        }

        public override bool openComponent(ComponentSyntax componentSyntax)
        {
            currentComponent = new ComponentModel() { name = componentSyntax.name };
            if (currentStructure != null)
                currentStructure.components.Add(componentSyntax.name, currentComponent);

            return true;
        }

        public override void closeComponent(ComponentSyntax componentSyntax)
        {
            currentComponent = null;
        }

        public override bool openType(TypeSyntax typeSyntax)
        {
            var typeModel = new TypeModel();
            typeModel.names.Add(typeSyntax.name.name);
            if (typeSyntax.name.extensions != null)
                foreach (var extension in typeSyntax.name.extensions)
                    typeModel.names.Add(extension.name);

            if (genericStack.Count > 0)
            {
                var currentGeneric = genericStack.Peek();
                currentGeneric.generics.Add(typeModel);
            }
            else 
            {
                if (currentComponent != null)
                    currentComponent.types.Add(typeModel);
            }

            typeStack.Push(typeModel);

            return true;
        }

        public override void closeType(TypeSyntax typeSyntax)
        {
            typeStack.Pop();
        }

        public override bool openGenericArguments(GenericArgumentsSyntax genericArgumentSyntax)
        {
            var currentType = typeStack.Peek();

            genericStack.Push(currentType);

            return true;
        }

        public override void closeGenericArguments(GenericArgumentsSyntax genericArgumentsSyntax)
        {
            genericStack.Pop();
        }

        public override bool openFunction(FunctionSyntax functionSyntax)
        {
            var functionName = functionSyntax.procedure.name;

            if ((classStack.Count > 0) && (classStack.Peek().functions.ContainsKey(functionName))) 
                throw new ModelException($"Function {functionName} already defined in {classStack.Peek().name}.", currentFile, functionSyntax.start.line, functionSyntax.start.column);

            if (programModel.functions.ContainsKey(functionName))
                throw new ModelException($"Function {functionName} already defined.", currentFile, functionSyntax.start.line, functionSyntax.start.column);

            var function = new FunctionModel() {
                name = functionName,
                parentClass = classStack.Count > 0 ? classStack.Peek(): null,
                parentNamespace = namespaceStack.Count > 0 && classStack.Count == 0 ? namespaceStack.Peek() : null
            };

            if (classStack.Count > 0)
                classStack.Peek().functions.Add(functionName, function);
            else
                programModel.functions.Add(functionName, function);

            return true;
        }

        public override void closeFunction(FunctionSyntax functionSyntax)
        {
        }
    }
}
