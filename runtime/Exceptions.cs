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
        public ParserException(string message, Span span)
            :base(message, span)
        {
        }
    }

    public class ModelException : CompilerException
    {
        public string message;

        public ModelException(string message, Span span)
            :base(message, span)
        {
        }
    }

    public class CompilerException : Exception
    {
        public Span Span;

        public CompilerException(string message, Span span)
            : base(message)
        {
            Span = span;
        }
    }
}