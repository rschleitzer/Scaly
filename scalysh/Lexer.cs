using System;
using System.Collections.Generic;
using System.Text;

namespace scalysh
{
    class Lexer
    {
        public Token token;
        public string text;
        public int position;
        public int end;
        public int previousLine;
        public int previousColumn;
        public int line;
        public int column;

        public Lexer(string theText)
        {
            token = null;
            text = theText;
            end = text.Length;
            position = 0;
            previousLine = 1;
            previousColumn = 0;
            line = 1;
            column = 0;
            advance();
        }

        public void advance()
        {
            skipWhitespace();
            previousLine = line;
            previousColumn = column;

            if (position == end)
            {
                token = new EofToken();
                return;
            }

            char c = text[position];

            if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
            {
                token = scanIdentifier();
                return;
            }

            if ((c >= '0') && (c <= '9'))
            {
                token = scanNumericLiteral();
                return;
            }

            switch (c)
            {
                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                    token = scanOperator();
                    break;

                case '\"':
                    token = scanStringLiteral();
                    break;

                case '\'':
                    token = scanCharacterLiteral();
                    break;

                case '{': case '}': case '(': case ')': case '[': case ']': case '.': case ',': case ':': case ';': case '?': case '!': case '@': case '#': case '$': case '_': case '`':
                {
                    token = new Punctuation(new string(c, 1));
                    position = position + 1;
                    column = column + 1;
                    break;
                }

                default:
                    token = new InvalidToken();
                    break;
            }
        }

        Identifier scanIdentifier()
        {
            string name = new string(text[position], 1);

            do
            {
                position = position + 1;
                column = column + 1;

                if (position == end)
                    return (new Identifier(name));

                char c = text[position];
                if (((c >= 'a') && (c <= 'z')) ||
                    ((c >= 'A') && (c <= 'Z')) ||
                    ((c >= '0') && (c <= '9')) ||
                     (c == '_'))
                    name = name + c;
                else
                    return (new Identifier(name));
            }
            while (true);
        }

        Identifier scanOperator()
        {
            string operation = new string(text[position], 1);

            do
            {
                position = position + 1;
                column = column + 1;

                if (position == end)
                    return (new Identifier(operation));

                switch (text[position])
                {
                    case '/': case '=': case '-': case '+': case '!': case '*': case '%': case '<': case '&': case '|': case '^': case '~':
                        operation = operation + text[position];
                        break;

                    default:
                        return (new Identifier(operation));
                }
            }
            while (true);
        }

        public Token scanStringLiteral()
        {
            var value = "";

            do
            {
                position = position + 1;
                column = column + 1;

                if (position == end)
                    return (new InvalidToken());

                switch (text[position])
                {
                    case '\"':
                    {
                        position = position + 1;
                        column = column + 1;
                        return (new StringLiteral(value));
                    }
                    case '\\':
                    {
                        position = position + 1;
                        column = column + 1;
                        switch (text[position])
                        {
                            case '\"': case '\\': case '\'':
                            {
                                value = value + '\\';
                                value = value + text[position];
                                break;
                            }
                            case 'n': value = value + "\\n"; break;
                            case 'r': value = value + "\\r"; break;
                            case 't': value = value + "\\t"; break;
                            case '0': value = value + "\\0"; break;
                            default: return (new InvalidToken());
                        }
                        break;
                    }
                    default:
                        value = value + text[position];
                        break;
                }
            }
            while (true);
        }

        Token scanCharacterLiteral()
        {
            var value = "";

            do
            {
                position = position + 1;
                column = column + 1;

                if (position == end)
                    return (new InvalidToken());

                switch (text[position])
                {
                    case '\'':
                    {
                        position = position + 1;
                        column = column + 1;
                        return (new CharacterLiteral(value));
                    }
                    case '\\':
                    {
                        position = position + 1;
                        column = column + 1;
                        switch (text[position])
                        {
                            case '\"': case '\\': case '\'':
                            {
                                value = value + '\\';
                                value = value + text[position];
                                break;
                            }
                            case 'n': value = value + "\\n"; break;
                            case 'r': value = value + "\\r"; break;
                            case 't': value = value + "\\t"; break;
                            case '0': value = value + "\\0"; break;
                            default: return (new InvalidToken());
                        }
                        break;
                    }
                    default:
                        value = value + text[position];
                        break;
                }
            }
            while (true);
        }

        NumericLiteral scanNumericLiteral()
        {
            var value = new string(text[position], 1);

            do
            {
                position = position + 1;
                column = column + 1;

                if (position == end)
                    return (new NumericLiteral(value));

                var c = text[position];
                if ((c >= '0') && (c <= '9'))
                    value = value + text[position];
                else
                    return (new NumericLiteral(value));
            }
            while (true);
        }

        void skipWhitespace()
        {
            do
            {
                if (position == end)
                    return;

                switch (text[position])
                {
                    case ' ':
                    {
                        position = position + 1;
                        column = column + 1;
                        continue;
                    }

                    case '\t':
                    {
                        position = position + 1;
                        column = column + 4;
                        continue;
                    }

                    case '\r':
                    {
                        position = position + 1;
                        continue;
                    }

                    case '\n': {
                        position = position + 1;
                        column = 1;
                        line = line + 1;
                        continue;
                    }

                    case '/':
                    {
                        position = position + 1;
                        column = column + 1;

                        if (position == end)
                            return;

                        if (text[position] == '/')
                        {
                            handleSingleLineComment();
                        }
                        else
                        {
                            if (text[position] == '*')
                                handleMultiLineComment();
                            else
                                return;
                        }
                            break;
                    }
                    default:
                        return;
                }
            }
            while (true);
        }

        void handleSingleLineComment()
        {
            do
            {
                if (position == end)
                    return;

                switch (text[position])
                {
                    case '\t':
                    {
                        position = position + 1;
                        column = column + 4;
                        continue;
                    }

                    case '\r':
                    {
                        position = position + 1;
                        continue;
                    }

                    case '\n':
                    {
                        position = position + 1;
                        column = 1;
                        line = line + 1;
                        return;
                    }

                    default:
                    {
                        position = position + 1;
                        column = column + 1;
                        continue;
                    }
                }
            }
            while (true);
        }

        void handleMultiLineComment()
        {
            do
            {
                if (position == end)
                    return;

                switch (text[position])
                {
                    case '/':
                    {
                        position = position + 1;
                        column = column + 1;

                        if (position == end)
                        {
                            return;
                        }
                        else
                        {
                            if (text[position] == '*')
                                handleMultiLineComment();
                            else
                                return;
                        }
                        break;
                    }

                    case '*':
                    {
                        position = position + 1;
                        column = column + 1;

                            if (position == end)
                            {
                                return;
                            }
                            else
                            {
                                if (text[position] == '/')
                                {
                                    position = position + 1;
                                    column = column + 1;
                                    return;
                                }
                            }
                        break;
                    }

                    case '\t':
                    {
                        position = position + 1;
                        column = column + 4;
                        continue;
                    }

                    case '\r':
                    {
                        position = position + 1;
                        continue;
                    }

                    case '\n':
                    {
                        position = position + 1;
                        column = 1;
                        line = line + 1;
                        return;
                    }
                    default:
                    {
                        position = position + 1;
                        column = column + 1;
                        continue;
                    }
                }
            }
            while (true);
        }

        public bool parseKeyword(string fixedString)
        {
            if (!(token is Identifier))
                return false;

            Identifier identifier = token as Identifier;

            return (identifier.name == fixedString);
        }

        public string parseIdentifier()
        {
            if (!(token is Identifier))
                return null;

            Identifier identifier = token as Identifier;

            return identifier.name;
        }

        public bool parsePunctuation(string fixedString)
        {
            if (!(token is Punctuation))
                return false;

            Punctuation punctuation = token as Punctuation;

            return punctuation.sign == fixedString;
        }

        public Literal parseLiteral()
        {
            if (!(token is Literal))
                return null;

            if (token is StringLiteral)
            {
                StringLiteral stringLiteral = token as StringLiteral;
                return new StringLiteral(stringLiteral.value);
            }

            if (token is CharacterLiteral)
            {
                CharacterLiteral characterLiteral = token as CharacterLiteral;
                return new CharacterLiteral(characterLiteral.value);
            }

            if (token is NumericLiteral)
            {
                NumericLiteral numericLiteral = token as NumericLiteral;
                return new NumericLiteral(numericLiteral.value);
            }

            return null;
        }

        public bool isAtEnd()
        {
            return position == end;
        }

        public Position getPosition()
        {
            return new Position(line, column);
        }

        public Position getPreviousPosition()
        {
            return new Position(previousLine, previousColumn);
        }
    }

    public struct Position
    {
        public int line;
        public int column;

        public Position(int line, int column)
        {
            this.line = line;
            this.column = column;
        }

        public Position(Position position)
        {
            this.line = position.line;
            this.column = position.column;
        }
    }

    public class Token
    {
    }

    public class EofToken : Token
    {
    }

    public class InvalidToken : Token
    {
    }

    public class Identifier : Token
    {
        public string name;
        public Identifier(string name)
        {
            this.name = name;
        }
    }

    public class Literal : Token
    {
    }

    public class StringLiteral : Literal
    {
        public string value;
        public StringLiteral(string theString)
        {
            value = theString;
        }
    }

    public class CharacterLiteral : Literal
    {
        public string value;
        public CharacterLiteral(string theString)
        {
            value = theString;
        }
    }

    public class NumericLiteral : Literal
    {
        public string value;
        public NumericLiteral(string theValue)
        {
            value = theValue;
        }
    }

    public class Punctuation : Token
    {
        public string sign;
        public Punctuation(string theSign)
        {
            sign = theSign;
        }
    }
}
