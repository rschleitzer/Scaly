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
        public ParserException(string message, string file, ulong lineStart, ulong columnStart, ulong lineEnd, ulong columnEnd)
            :base(message, file, lineStart, columnStart, lineEnd, columnEnd)
        {
        }
    }

    public class ModelException : CompilerException
    {
        public string message;

        public ModelException(string message, string file, ulong lineStart, ulong columnStart, ulong lineEnd, ulong columnEnd)
            :base(message, file, lineStart, columnStart, lineEnd, columnEnd)
        {
        }
    }

    public class CompilerException : Exception
    {
        public string file;
        public ulong lineStart;
        public ulong columnStart;
        public ulong lineEnd;
        public ulong columnEnd;

        public CompilerException(string message, string file, ulong lineStart, ulong columnStart, ulong lineEnd, ulong columnEnd)
            : base(message)
        {
            this.file = file;
            this.lineStart = lineStart;
            this.columnStart = columnStart;
            this.lineEnd = lineEnd;
            this.columnEnd = columnEnd;
        }
    }
}