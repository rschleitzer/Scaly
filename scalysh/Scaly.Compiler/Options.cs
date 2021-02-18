using System.Collections.Generic;

namespace scalyc
{
    public class Options {

        public string[] files;
        public string outputName;

        Options(string[] input, string output)
        {
            files = input;
            outputName = output;
        }

        public static Options ParseArguments(string[] args) {
            var length = args.Length;
            string output = null;
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
                    default:
                        throw new UnknownOptionException(args[i]);
                }
                i++;
            }
            if (output == null)
                throw new NoOutputOptionException();
            if (input.Count == 0)
                throw new NoFilesToCompileException();

            return(new Options(input.ToArray(), output));
        }
    }

}

