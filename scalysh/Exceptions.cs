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
}