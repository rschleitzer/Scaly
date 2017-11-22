using System;

namespace scalysh
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

    public class ParserException : Exception
    {
        public int line;
        public int column;
        public ParserException(int line, int column)
        {
            this.line = line;
            this.column = column;
        }
    }

    public class CompilerException : Exception
    {
        public int line;
        public int column;
        public CompilerException(int line, int column)
        {
            this.line = line;
            this.column = column;
        }
    }
}