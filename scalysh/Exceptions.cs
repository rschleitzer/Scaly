using System;

namespace scalyc
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
        public ParserException(string file, int line, int column)
            :base(file, line, column)
        {
        }
    }

    public class ModelException : CompilerException
    {
        public string message;

        public ModelException(string message, string file, int line, int column)
            :base(file, line, column)
        {
        }
    }

    public class CompilerException : Exception
    {
        public string file;
        public int line;
        public int column;

        public CompilerException(string file, int line, int column)
        {
            this.file = file;
            this.line = line;
            this.column = column;
        }
    }
}