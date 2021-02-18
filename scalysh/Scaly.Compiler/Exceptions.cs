using System;

namespace Scaly.Compiler
{
    public class InvalidOptionException : Exception
    {
        public InvalidOptionException(string option)
        {
            Option = option;
        }
        public string Option;
    };

    public class UnknownOptionException : Exception
    {
        public UnknownOptionException(string option)
        {
            _option = option;
        }
        string _option;

    };

    public class NoOutputOptionException : Exception
    {
    };

    public class NoFilesToCompileException : Exception
    {
    };

    public class ParserException : CompilerException
    {
        public ParserException(string file, ulong line, ulong column)
            :base(file, line, column)
        {
        }
    }

    public class ModelException : CompilerException
    {
        public string message;

        public ModelException(string message, string file, ulong line, ulong column)
            :base(file, line, column)
        {
        }
    }

    public class CompilerException : Exception
    {
        public string file;
        public ulong line;
        public ulong column;

        public CompilerException(string file, ulong line, ulong column)
        {
            this.file = file;
            this.line = line;
            this.column = column;
        }
    }
}