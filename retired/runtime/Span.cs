namespace Scaly.Compiler
{
    public class Position
    {
        public ulong line;
        public ulong column;
    }

    public class Span
    {
        public string file;
        public Position start;
        public Position end;
    }
}
