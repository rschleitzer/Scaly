using System.Collections.Generic;

namespace scalysh
{
    public class Options {

        public string[] files;
        public string outputName;
        public string directory;

        Options(string[] input, string output, string dir)
        {
            files = input;
            outputName = output;
            directory = dir;
        }

        public static Options ParseArguments(string[] args) {
            var length = args.Length;
            string output = null;
            string dir = null;
            var input = new List<string>();
            var i = 0;
            while (i < length) {
                if ((length < 2) || ((args[i])[0] != '-')) {
                    input.Add(args[i]);
                    i++;
                    continue;
                }
                switch ((args[i])[1]) {
                    case 'o': {
                        i++;
                        if (i == length)
                            throw new InvalidOptionException(args[i - 1]);
                        else
                            output = args[i];
                        break;
                    }
                    case 'd': {
                        i++;
                        if (i == length)
                            throw new InvalidOptionException(args[i - 1]);
                        else
                            dir = args[i];
                        break;
                    }
                    default:
                        throw new UnknownOptionException(args[i]);
                }
                i++;
            }
            if (output == null)
                throw new NoOutputOptionException();
            if (input.Count == 0)
                throw new NoFilesToCompileException();

            return(new Options(input.ToArray(), output, dir));
        }
    }

}

