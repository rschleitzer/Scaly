using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace scalysh
{
    class CVisitor : Visitor
    {
        StreamWriter Writer = null;
        string namespacePrefix = "";

        public override bool openProgram(Program program)
        {
            Writer = new StreamWriter($"{program.name}.c");
            return true;
        }

        public override void closeProgram(Program program)
        {
            Writer.Flush();
            Writer.BaseStream.Close();
        }

        public override bool openFile(File theFile)
        {
            Writer.WriteLine($"// {theFile.fileName}\n");
            return true;
        }

        public override bool openName(Name theName)
        {
            if (theName.parent is Namespace)
                return true;

            Writer.Write(theName.name);
            return true;
        }

        public override void visitExtension(Extension theExtension)
        {
            Writer.Write($"_dot_{theExtension.name}");
        }

        public override bool openNamespace(Namespace theNamespace)
        {
            string namespaceName = $"{theNamespace.name.name}_dot_";
            if (theNamespace.name.extensions != null)
                foreach (var extension in theNamespace.name.extensions)
                    namespaceName += $"{extension.name}_dot_";

            namespacePrefix = namespaceName;
            return true;
        }

        public override bool openClass(Class theClass)
        {
            Writer.Write($"struct {namespacePrefix}{theClass.name.name} {{\n");
            return true;
        }

        public override void closeClass(Class theClass)
        {
            Writer.Write("}\n");
        }

        public override bool openFunction(Function theFunction)
        {
            if (openTypeAnnotation(theFunction.procedure.routine.output))
                closeTypeAnnotation(theFunction.procedure.routine.output);
            
            Writer.Write(theFunction.procedure.name);

            return true;
        }

        public override void closeFunction(Function theFunction)
        {
        }

    }
}
