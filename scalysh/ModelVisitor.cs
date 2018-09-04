using System.Collections.Generic;

namespace scalysh
{
    class ProgramModel
    {
        public string name;
    }

    class ClassModel
    {
        public string name;
    }

    class FunctionModel
    {
        public string name;
    }

    class ModelVisitor : SyntaxVisitor
    {
        public ProgramModel Model;

        string currentFile;
        Stack<string> namespaceStack = new Stack<string>();
        Stack<ClassModel> classStack = new Stack<ClassModel>();

        Dictionary<string, ClassModel> classDictionary = new Dictionary<string, ClassModel>();
        Dictionary<string, FunctionModel> functionDictionary = new Dictionary<string, FunctionModel>();

        public override bool openProgram(ProgramSyntax programSyntax)
        {
            Model = new ProgramModel { name = programSyntax.name };
            return true;
        }

        public override bool openFile(FileSyntax fileSyntax)
        {
            currentFile = fileSyntax.fileName;
            return true;
        }

        public override bool openNamespace(NamespaceSyntax namespaceSyntax)
        {
            namespaceStack.Push(namespaceSyntax.name.name);
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    namespaceStack.Push(extension.name);

            namespaceSyntax.scope.accept(this);
            return false;
        }

        public override void closeNamespace(NamespaceSyntax namespaceSyntax)
        {
            namespaceStack.Pop();
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    namespaceStack.Pop();
        }

        public override bool openClass(ClassSyntax classSyntax)
        {
            var className = "";
            foreach (var name in namespaceStack)
                className += name + ".";

            className += classSyntax.name;

            ClassModel theClass = null;
            if (!classDictionary.ContainsKey(className))
            {
                theClass = new ClassModel() { name = className };
                classDictionary.Add(className, theClass);
            }
            else
            {
                theClass = classDictionary[className];
            }

            classStack.Push(theClass);

            if (classSyntax.contents != null)
                classSyntax.contents.accept(this);
            
            if (classSyntax.body != null)
                classSyntax.body.accept(this);

            return true;
        }

        public override void closeClass(ClassSyntax classSyntax) => classStack.Pop();

        public override bool openFunction(FunctionSyntax functionSyntax)
        {
            string functionName = null;
            var currentClass = classStack.Peek();
            if (currentClass != null)
            {
                functionName = currentClass.name + ".";
            }
            else
            {
                functionName = "";
            }

            functionName += functionSyntax.procedure.name;
            if (functionDictionary.ContainsKey(functionName))
                throw new ModelException($"Function {functionSyntax.procedure.name} already defined.", currentFile, functionSyntax.start.line, functionSyntax.start.column);
            var function = new FunctionModel() { name = functionName };
            functionDictionary.Add(functionName, function);
            return true;
        }

        public override void closeFunction(FunctionSyntax functionSyntax)
        {
        }
    }
}
