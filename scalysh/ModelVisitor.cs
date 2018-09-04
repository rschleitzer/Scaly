using System.Collections.Generic;

namespace scalysh
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
        public Dictionary<string, ClassModel> subClasses = new Dictionary<string, ClassModel>();
        public Dictionary<string, FunctionModel> functions = new Dictionary<string, FunctionModel>();
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
            NamespaceModel theNamespace = null;
            NamespaceModel currentNamespace = null;

            if (namespaceStack.Count > 0)
            {
                currentNamespace = namespaceStack.Peek();
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
            currentNamespace = theNamespace;

            if (namespaceSyntax.name.extensions != null)
            {
                foreach (var extension in namespaceSyntax.name.extensions)
                {

                    NamespaceModel subNamespaceModel = null;
                    if (currentNamespace.subNamespaces.ContainsKey(extension.name))
                    {
                        subNamespaceModel = currentNamespace.subNamespaces[extension.name];
                    }
                    else 
                    {
                        subNamespaceModel = new NamespaceModel() { name = extension.name, parentNamespace = currentNamespace  };
                        currentNamespace.subNamespaces.Add(extension.name, subNamespaceModel);
                    }

                    namespaceStack.Push(subNamespaceModel);
                    currentNamespace = subNamespaceModel;
                    name = extension.name;
                }
            }

            namespaceSyntax.scope.accept(this);

            namespaceStack.Pop();
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    namespaceStack.Pop();
            return false;
        }

        public override bool openClass(ClassSyntax classSyntax)
        {
            ClassModel theClass = null;

            var name = classSyntax.name;
            if (classStack.Count > 0)
            {
                ClassModel currentClass = classStack.Peek();
                if (currentClass.subClasses.ContainsKey(name))
                {
                    theClass = currentClass.subClasses[name];
                }
                else
                {
                    theClass = new ClassModel() { name = name, parentClass = currentClass };
                    currentClass.subClasses.Add(name, theClass);
                }
            }
            else
            {
                if (namespaceStack.Count > 0)
                {
                    NamespaceModel currentNamespace = namespaceStack.Peek();
                    if (currentNamespace.classes.ContainsKey(name))
                    {
                        theClass = currentNamespace.classes[name];
                    }
                    else
                    {
                        theClass = new ClassModel() { name = name, parentNamespace = currentNamespace };
                        currentNamespace.classes.Add(name, theClass);
                    }
                }
                else
                {
                    if (programModel.classes.ContainsKey(name))
                    {
                        theClass = programModel.classes[name];
                    }
                    else
                    {
                        theClass = new ClassModel() { name = name};
                        programModel.classes.Add(name, theClass);
                    }
                   
                }
            }

            classStack.Push(theClass);

            if (classSyntax.contents != null)
                classSyntax.contents.accept(this);
            
            if (classSyntax.body != null)
                classSyntax.body.accept(this);

            classStack.Pop();
            return false;
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
