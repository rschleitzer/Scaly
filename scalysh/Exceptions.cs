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
        public string file;
        public int line;
        public int column;
        public ParserException(string file, int line, int column)
        {
            this.file = file;
            this.line = line;
            this.column = column;
        }
    }

    public class CompilerException : ParserException
    {
        public CompilerException(string file, int line, int column)
            :base(file, line, column)
        {
        }
    }
}