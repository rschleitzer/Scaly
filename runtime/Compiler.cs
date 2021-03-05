using System;
using System.Collections.Generic;

namespace Scaly.Compiler
{
    public class Compiler
    {
        public static void compileFiles(Options options)
        {
            var sources = Modeler.BuildFiles(options.files);
            Generator.GenerateProgram(sources, options.outputName);
        }

        public static int compileAndRunProgram(string program, string[] arguments)
        {
            var source = Modeler.BuildProgram(program);
            var runtime = Modeler.BuildRuntime();
            var main = Generator.JitProgram(new List<Source> { source, runtime });
            int ret = main(arguments.Length, arguments);
            return ret;
        }

        public static void Repl()
        {
            var file = "";
            while (true)
            {
                var deck = "";
                while (true)
                {
                    Console.Write("scalyc>");
                    var card = Console.ReadLine();
                    var ch = card[0];
                    if (ch == '%')
                        break;

                    while (true)
                    {
                        deck += card;
                        try
                        {
                            string result = EvaluateDeck(deck);
                            if (result.Length > 0)
                            {
                                Console.WriteLine(result);
                                break;
                            }
                            else
                            {
                                file += deck;
                                deck = "";
                                Console.WriteLine("Declaration added.");
                            }
                        }
                        catch (Exception e)
                        {
                            if (e.Message.Length != 0)
                            {
                                Console.WriteLine(e.Message);
                                return;
                            }
                        }
                        card = Console.ReadLine();
                    }
                }
            }
        }

        static string EvaluateDeck(string deck)
        {
            return deck;
        }
    }
}
