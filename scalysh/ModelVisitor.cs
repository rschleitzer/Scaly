using System.Collections.Generic;

namespace scalysh
{
    class ProgramModel
    {
        public string name;
    }

    class NamespaceModel
    {
        public string name;
    }

    class ClassModel
    {
        public string name;
    }

    class ModelVisitor : SyntaxVisitor
    {
        public ProgramModel Model;

        string currentFile;
        Stack<string> currentNamespace = new Stack<string>();
        Stack<ClassModel> currentClass = new Stack<ClassModel>();

        Dictionary<string, ClassModel> dictClasses = new Dictionary<string, ClassModel>();

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
            currentNamespace.Push(namespaceSyntax.name.name);
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    currentNamespace.Push(extension.name);

            namespaceSyntax.scope.accept(this);
            return false;
        }

        public override void closeNamespace(NamespaceSyntax namespaceSyntax)
        {
            currentNamespace.Pop();
            if (namespaceSyntax.name.extensions != null)
                foreach (var extension in namespaceSyntax.name.extensions)
                    currentNamespace.Pop();
        }

        public override bool openClass(ClassSyntax classSyntax)
        {
            var className = "";
            foreach (var name in currentNamespace)
                className += name + ".";

            className += classSyntax.name.name;
            if (classSyntax.name.extensions != null)
                foreach (var extension in classSyntax.name.extensions)
                    className += "." + extension;

            ClassModel theClass = null;
            if (!dictClasses.ContainsKey(className))
            {
                theClass = new ClassModel() { name = className };
                dictClasses.Add(className, theClass);
            }
            else
            {
                theClass = dictClasses[className];
            }

            currentClass.Push(theClass);

            if (classSyntax.contents != null)
                classSyntax.contents.accept(this);
            
            if (classSyntax.body != null)
                classSyntax.body.accept(this);

            return true;
        }

        public override void closeClass(ClassSyntax classSyntax)
        {
            currentClass.Pop();
        }
    }
}
