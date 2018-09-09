using System.Collections.Generic;

namespace scalyc
{
    class ProgramModel
    {
        public string name;
        public Dictionary<string, FileModel> files = new Dictionary<string, FileModel>();
        public Dictionary<string, NamespaceModel> namespaces = new Dictionary<string, NamespaceModel>();
        public Dictionary<string, ClassModel> classes = new Dictionary<string, ClassModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
        public HashSet<string> intrinsics = new HashSet<string>();
    }

    class FileModel
    {
        public string name;
        public List<UsingModel> usings = new List<UsingModel>();
        public Dictionary<string, DefineModel> defines = new Dictionary<string, DefineModel>();
    }

    class NamespaceModel
    {
        public string name;
        public NamespaceModel parentNamespace;
        public Dictionary<string, NamespaceModel> subNamespaces = new Dictionary<string, NamespaceModel>();
        public List<UsingModel> usings = new List<UsingModel>();
        public Dictionary<string, DefineModel> defines = new Dictionary<string, DefineModel>();
        public Dictionary<string, ClassModel> classes = new Dictionary<string, ClassModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
    }

    class UsingModel
    {
        public List<string> names = new List<string>();
    }

    class TypeModel
    {
        public List<string> names = new List<string>();
        public List<TypeModel> generics = new List<TypeModel>();
    }

    class ComponentModel
    {
        public string name;
        public List<TypeModel> types = new List<TypeModel>();
    }

    class StructureModel
    {
        public Dictionary<string, ComponentModel> components = new Dictionary<string, ComponentModel>();
    }

    class CallableModel
    {
        public StructureModel structure;
    }

    class BlockModel
    {

    }

    class LambdaModel : CallableModel
    {
        public BlockModel code;
    }

    class ClassModel : CallableModel
    {
        public string name;
        public NamespaceModel parentNamespace;
        public ClassModel parentClass;
        public Dictionary<string, ClassModel> subClasses = new Dictionary<string, ClassModel>();
        public List<AllocatorModel> allocators = new List<AllocatorModel>();
        public List<InitializerModel> initializers = new List<InitializerModel>();
        public Dictionary<string, MethodModel> methods = new Dictionary<string, MethodModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
    }

    class ClassCodeModel : LambdaModel
    {
        public ClassModel parentClass;
    }

    class AllocatorModel : ClassCodeModel
    {
    }

    class InitializerModel : ClassCodeModel
    {
    }

    class ProcedureModel : ClassCodeModel
    {
        public string name;
    }

    class RoutineModel : LambdaModel
    {
        public TypeModel output;
        public TypeModel error;
    }

    class MethodModel : ProcedureModel
    {
    }

    class FunctionModel : RoutineModel
    {
        public string name;
        public NamespaceModel parentNamespace;
    }

    class DefineModel
    {
        public string name;
        public List<TypeModel> types = new List<TypeModel>();
    }

    class ModelVisitor : SyntaxVisitor
    {
        public ProgramModel programModel;
        FileModel currentFile;

        Stack<NamespaceModel> namespaceStack = new Stack<NamespaceModel>();
        Stack<ClassModel> classStack = new Stack<ClassModel>();
        CallableModel currentCallable;
        StructureModel currentStructure;
        ComponentModel currentComponent;
        DefineModel currentDefine;
        RoutineModel currentRoutine;
        bool routineOutput;
        Stack<TypeModel> typeStack = new Stack<TypeModel>();
        Stack<TypeModel> genericStack = new Stack<TypeModel>();

        public override bool openProgram(ProgramSyntax programSyntax)
        {
            programModel = new ProgramModel { name = programSyntax.name };
            return true;
        }

        public override bool openFile(FileSyntax fileSyntax)
        {
            currentFile = new FileModel() { name = fileSyntax.fileName };
            programModel.files.Add(fileSyntax.fileName, currentFile);

            return true;
        }

        public override bool openUsing(UsingSyntax usingSyntax)
        {
            var theUsing = new UsingModel();
            theUsing.names.Add(usingSyntax.name.name);
            if (usingSyntax.name.extensions != null)
                foreach (var extension in usingSyntax.name.extensions)
                    theUsing.names.Add(extension.name);

            if (namespaceStack.Count > 0)
                namespaceStack.Peek().usings.Add(theUsing);
            else
                currentFile.usings.Add(theUsing);

            return false;
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

            foreach (var declaration in namespaceSyntax.declarations)
                declaration.accept(this);

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

        public override bool openBlock(BlockSyntax blockSyntax)
        {
            var blockModel = new BlockModel() { };
            return true;
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

            if (classSyntax.baseClass != null)
            {
                // base class to go here
            }

            currentCallable = theClass;

            if (classSyntax.contents != null)
                classSyntax.contents.accept(this);

            currentCallable = null;
            
            if (classSyntax.body != null)
                classSyntax.body.accept(this);

            classStack.Pop();

            if (classSyntax.name.extensions != null)
                foreach (var extension in classSyntax.name.extensions)
                    namespaceStack.Pop();

            return false;
        }

        public override bool openStructure(StructureSyntax structureSyntax)
        {
            currentStructure = new StructureModel();
            if (currentCallable != null)
                currentCallable.structure = currentStructure;

            return true;
        }

        public override void closeStructure(StructureSyntax structureSyntax)
        {
            currentStructure = null;
            currentCallable = null;
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

        public override bool openMethod(MethodSyntax methodSyntax)
        {
            var methodName = methodSyntax.procedure.name;

            if ((classStack.Count > 0) && (classStack.Peek().methods.ContainsKey(methodName))) 
                throw new ModelException($"Function {methodName} already defined in {classStack.Peek().name}.", currentFile.name, methodSyntax.start.line, methodSyntax.start.column);

            var theMethod = new MethodModel() {
                name = methodName,
                parentClass = classStack.Count > 0 ? classStack.Peek(): null
            };

            if (classStack.Count > 0)
                classStack.Peek().methods.Add(methodName, theMethod);

            currentCallable = theMethod;

            return true;
        }

        public override bool openInitializer(InitializerSyntax initializerSyntax)
        {
            var currentClass = classStack.Peek();
            var initializerModel = new InitializerModel() { parentClass = currentClass };
            currentClass.initializers.Add(initializerModel);
            currentCallable = initializerModel;

            return true;
        }

        public override bool openAllocator(AllocatorSyntax allocatorSyntax)
        {
            var currentClass = classStack.Peek();
            var allocatorModel = new AllocatorModel() { parentClass = currentClass };
            currentClass.allocators.Add(allocatorModel);
            currentCallable = allocatorModel;

            return true;
        }

        public override bool openFunction(FunctionSyntax functionSyntax)
        {
            var functionName = functionSyntax.procedure.name;

            if ((classStack.Count > 0) && (classStack.Peek().functions.ContainsKey(functionName))) 
                throw new ModelException($"Function {functionName} already defined in {classStack.Peek().name}.", currentFile.name, functionSyntax.start.line, functionSyntax.start.column);

            if (programModel.functions.ContainsKey(functionName))
                throw new ModelException($"Function {functionName} already defined.", currentFile.name, functionSyntax.start.line, functionSyntax.start.column);

            var theFunction = new FunctionModel() {
                name = functionName,
                parentNamespace = namespaceStack.Count > 0 && classStack.Count == 0 ? namespaceStack.Peek() : null
            };

            if (classStack.Count > 0)
                classStack.Peek().functions.Add(functionName, theFunction);
            else
                programModel.functions.Add(functionName, theFunction);

            currentCallable = theFunction;
            currentRoutine = theFunction;

            return true;
        }

        public override bool openRoutine(RoutineSyntax routineSyntax)
        {
            if (routineSyntax.output != null)
            {
                routineOutput = true;
                routineSyntax.output.accept(this);
                routineOutput = false;
            }

            if (routineSyntax.throwsClause != null)
            {
                routineSyntax.throwsClause.accept(this);
            }

            routineSyntax.body.accept(this);

            currentRoutine = null;
            return false;
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
                {
                    currentComponent.types.Add(typeModel);
                }
                else
                {
                    if (currentRoutine != null)
                    {
                        if (routineOutput)
                            currentRoutine.output = typeModel;
                        else
                            currentRoutine.error = typeModel;
                    }
                    else
                    {
                        if (currentDefine != null)
                        {
                            currentDefine.types.Add(typeModel);
                        }
                    }
                }
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

        public override void visitIntrinsic(IntrinsicSyntax intrinsicSyntax)
        {
            programModel.intrinsics.Add(intrinsicSyntax.name);
        }

        public override bool openDefine(DefineSyntax defineSyntax)
        {
            var name = defineSyntax.name.name;
            if (defineSyntax.name.extensions != null)
            {
                foreach (var extension in defineSyntax.name.extensions)
                {
                    AddNamespace(name);
                    name = extension.name;
                }
            }

            currentDefine = new DefineModel() { name = name };

            if (namespaceStack.Count > 0)
            {
                var currentNamespace = namespaceStack.Peek();
                if (currentNamespace.defines.ContainsKey(name))
                    throw new ModelException($"{name} already defined!", currentFile.name, defineSyntax.start.line, defineSyntax.start.column);
                currentNamespace.defines.Add(name, currentDefine);
            }
            else
            {
                if (currentFile.defines.ContainsKey(name))
                    throw new ModelException($"{name} already defined!", currentFile.name, defineSyntax.start.line, defineSyntax.start.column);
                currentFile.defines.Add(name, currentDefine);
            }

            return true;
        }

        public override void closeDefine(DefineSyntax defineSyntax)
        {
            currentDefine = null;
        }
    }
}
