using System;
using System.Collections.Generic;
using System.Text;

namespace Scaly.Compiler
{
    class Lexer
    {
        public Token token;
        char? character;
        CharEnumerator chars;
        public ulong previous_line;
        public ulong previous_column;
        public ulong line;
        public ulong column;

        public Lexer(string deck)
        {
            token = new EmptyToken();
            chars = deck.GetEnumerator();
            character = null;
            previous_line = 1;
            previous_column = 0;
            line = 1;
            column = 0;
            read_character();
            skip_whitespace(true);
        }

        void read_character()
        {
            if (chars.MoveNext())
                character = chars.Current;
            else
                character = null;

            switch (character)
            {
                case '\r':
                    break;

                case '\n':
                    column = 0;
                    line += 1;
                    break;

                default:
                    column++;
                    break;
            }
        }

        public void advance()
        {
            skip_whitespace(false);
            previous_line = line;
            previous_column = column;

            if (character == null)
                return;

            char c = (char)character;

            if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
            {
                token = scan_identifier();
                return;
            }

            if ((c >= '1') && (c <= '9'))
            {
                var value = new StringBuilder();
                value.Append(c);
                token = scan_numeric_literal(value);
                return;
            }

            switch (c)
            {
                case '\n':
                    token = scan_line_feed();
                    break;

                case ':':
                    read_character();
                    token = new Colon();
                    break;

                case ';':
                    read_character();
                    token = new Semicolon();
                    break;

                case '0':
                    {
                        var value = new StringBuilder();
                        value.Append(c);
                        token = scan_numeric_or_hex_literal(value);
                    }
                    break;

                case '@':
                    read_character();
                    token = scan_attribute();
                    break;

                case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                {
                    var operation = new StringBuilder();
                    operation.Append(c);
                    token = scan_operator(operation);
                    break;
                }

                case '\"': token = scan_string_literal(); break;
                case '\'': token = scan_string_identifier(); break;
                case '`':  token = scan_fragment_literal(); break;

                case '}': case ')': case ']': case '.': case '?':
                {
                    var punctuation_string = new StringBuilder();
                    punctuation_string.Append(c);
                    read_character();
                    token = new Punctuation(punctuation_string.ToString());
                    break;
                }

                case '{': case '(': case '[': case ',':
                {
                    var punctuation_string = new StringBuilder();
                    punctuation_string.Append(c);
                    read_character();
                    skip_whitespace(true);
                    token = new Punctuation(punctuation_string.ToString());
                    break;
                }

                default:
                    token = new InvalidToken();
                    break;
            }
        }

        public void empty() {
            token = new EmptyToken();
        }

        Token scan_line_feed()
        {
            while (true)
            {
                read_character();
                skip_whitespace(false);
                if (character == null)
                    return new InvalidToken();

                if (character == '\n')
                    continue;

                return new LineFeed();
            }
        }

        Token scan_identifier()
        {
            var name = new StringBuilder();
            while (true)
            {
                if (character == null)
                {
                    if (name.Length == 0)
                        return new InvalidToken();
                    else
                        return new Identifier(name.ToString());
                }
                else
                {
                    char c = (char)character;
                    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                    {
                        name.Append(c);
                        read_character();
                    }
                    else
                    {
                        return new Identifier(name.ToString());
                    }
                }
            }
        }

        Token scan_attribute()
        {
            var name = new StringBuilder();
            while (true)
            {
                if (character == null)
                {
                    if (name.Length == 0)
                        return new InvalidToken();
                    else
                        return new Attribute(name.ToString());
                }

                var c = (char)character;
                if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
                {
                    name.Append(c);
                    read_character();
                }
                else
                {
                    return new Attribute(name.ToString());
                }
            }
        }

        Token scan_operator(StringBuilder operation)
        {
            while (true)
            {
                read_character();
                if (character == null)
                {
                    if (operation.Length == 0)
                        return new InvalidToken();
                    else
                        return new Identifier(operation.ToString());
                }

                var c = (char)character;
                switch (c)
                {
                    case '+': case '-': case '*': case '/': case '=': case '%': case '&': case '|': case '^': case '~': case '<': case '>':
                        operation.Append(c);
                        break;

                    default:
                        return new Identifier(operation.ToString());
                }
            }
        }

        Token scan_string_literal()
        {
            var value = new StringBuilder();

            while (true)
            {
                read_character();

                if (character == null)
                    return new InvalidToken();

                var c = (char)character;
                switch (c)
                {
                    case '\"':
                        read_character();
                        return new StringLiteral(value.ToString());

                    case '\\':
                        read_character();
                        if (character == null)
                            return new InvalidToken();

                        var c2 = (char)character;
                        switch (c2)
                        {
                            case '\"': case '\\': case '\'':
                                value.Append('\\');
                                value.Append(c2);
                                break;
                            case 'n':
                                value.Append('\\');
                                value.Append('n');
                                break;
                            case 'r':
                                value.Append('\\');
                                value.Append('r');
                                break;
                            case 't':
                                value.Append('\\');
                                value.Append('t');
                                break;
                            case '0':
                                value.Append('\\');
                                value.Append('0');
                                break;
                            default:
                                return new InvalidToken();
                        }
                        break;

                    default:
                        value.Append(c);
                        break;
                }
            }
        }

        Token scan_string_identifier()
        {
            var value = new StringBuilder();

            while (true)
            {
                read_character();

                if (character == null)
                    return new InvalidToken();

                var c = (char)character;
                switch (c)
                {
                    case '\'':
                        read_character();
                        return new Identifier(value.ToString());
                    default:
                        value.Append(c);
                        break;
                }
            }
        }

        Token scan_fragment_literal()
        {
            var value = new StringBuilder();

            while (true)
            {
                read_character();

                if (character == null)
                    return new InvalidToken();

                var c = (char)character;
                switch (c)
                {
                    case '`':
                        read_character();
                        return new Fragment(value.ToString());

                    case '\\':
                        read_character();
                        if (character == null)
                            return new InvalidToken();

                        var c2 = (char)character;
                        switch (c2)
                        {
                            case '`':
                                value.Append(c2);
                                break;
                            default:
                                value.Append('\\');
                                value.Append(c2);
                                break;
                        }
                        break;

                    default:
                        value.Append(c);
                        break;
                }
            }
        }

        Token scan_numeric_or_hex_literal(StringBuilder value)
        {
            read_character();

            if (character == null)
                return new InvalidToken();

            var c = (char)character;

            if ((c >= '0') && (c <= '9'))
            {
                value.Append(c);
                return scan_numeric_literal(value);
            }

            switch (c)
            {
                case '.':
                    value.Append(c);
                    return scan_fraction(value);

                case 'E':
                case 'e':
                    value.Append(c);
                    return scan_exponent(value);

                case 'x':
                    return scan_hex_literal();

                default:
                    return new Integer(value.ToString());
            }
        }

        Token scan_numeric_literal(StringBuilder value)
        {
            while (true)
            {
                read_character();

                if (character == null)
                    return new Integer(value.ToString());

                var c = (char)character;

                if ((c >= '0') && (c <= '9'))
                {
                    value.Append(c);
                    continue;
                }

                switch (c)
                {
                    case '.':
                        value.Append(c);
                        return scan_fraction(value);

                    case 'E':
                    case 'e':
                        value.Append(c);
                        return scan_exponent(value);

                    default:
                        return new Integer(value.ToString());
                }
            }
        }

        Token scan_fraction(StringBuilder value)
        {
            while (true)
            {
                read_character();

                if (character == null)
                    return new FloatingPoint(value.ToString());

                var c = (char)character;

                if ((c >= '0') && (c <= '9'))
                {
                    value.Append(c);
                    continue;
                }

                switch (c)
                {
                    case 'E':
                    case 'e':
                        value.Append(c);
                        return scan_exponent(value);

                    default:
                        return new FloatingPoint(value.ToString());
                }
            }
        }

        Token scan_exponent(StringBuilder value)
        {
            while (true)
            {
                read_character();

                if (character == null)
                    return new FloatingPoint(value.ToString());

                var c = (char)character;

                if ((c >= '0') && (c <= '9'))
                {
                    value.Append(c);
                    continue;
                }

                return new FloatingPoint(value.ToString());
            }
        }

        Token scan_hex_literal()
        {
            var value = new StringBuilder();
            while (true)
            {
                read_character();

                if (character == null)
                    return new Hex(value.ToString());

                var c = (char)character;

                if (((c >= '0') && (c <= '9')) || (c >= 'A') && (c <= 'F') || ((c >= 'a') && (c <= 'f')))
                {
                    value.Append(c);
                    continue;
                }

                return new Hex(value.ToString());
            }
        }

        void skip_whitespace(bool line_feed)
        {
            while (true)
            {
                if (character == null)
                    return;

                var c = (char)character;

                switch(c)
                {
                    case ' ':
                        read_character();
                        continue;
                    case '\t':
                        read_character();
                        continue;
                    case '\r':
                        read_character();
                        continue;
                    case '\n':
                        if (line_feed)
                        {
                            read_character();
                            continue;
                        }
                        else
                        {
                            return;
                        }

                    case ';':
                        read_character();
                        if (character == null)
                            return;

                        var c2 = (char)character;
                        switch (c2)
                        {
                            case '*':
                                read_character();
                                handle_multi_line_comment();
                                break;

                            default:
                                read_character();
                                handle_single_line_comment();
                                break;
                        }
                        break;
                    default:
                        return;
                }
            }
        }


        void handle_single_line_comment()
        {
            while (true)
            {
                if (character == null)
                    return;

                var c = (char)character;

                switch (c)
                {
                    case '\t':
                        read_character();
                        continue;

                    case '\r':
                        read_character();
                        continue;

                    case '\n':
                        read_character();
                        return;

                    default:
                        read_character();
                        continue;
                }
            }
        }

        void handle_multi_line_comment()
        {
            while (true)
            {
                if (character == null)
                    return;
                var c = (char)character;

                switch (c)
                {
                    case ';':
                        read_character();
                        if (character == null)
                            return;
                        if (character == '*')
                            handle_multi_line_comment();
                        else
                            continue;
                        break;

                    case '*':
                        read_character();
                        if (character == null)
                            return;

                        c = (char)character;
                        if (c == ';')
                        {
                            read_character();
                            return;
                        }
                        break;

                    default:
                        read_character();
                        continue;
                }
            }
        }

        public bool parse_keyword(string fixed_string)
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case Identifier identifier:
                    var right_keyword = (identifier.name == fixed_string);
                    if (right_keyword)
                        empty();

                    return right_keyword;

                default:
                    return false;
            }
        }

        public string parse_identifier(HashSet<string> keywords)
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case Identifier identifier:
                    if (keywords.Contains(identifier.name))
                        return null;
                    empty();
                    return identifier.name;

                default:
                    return null;
            }
        }

        public string parse_attribute()
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case Attribute attribute:
                    empty();
                    return attribute.name;

                default:
                    return null;
            }
        }

        public bool parse_punctuation(string fixed_string)
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case Punctuation punctuation:
                    var ret = (punctuation.sign == fixed_string);
                    if (ret)
                        empty();

                    return ret;
                default:
                    return false;
            }
        }

        public Literal parse_literal()
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case StringLiteral stringLiteral:
                    empty();
                    return stringLiteral;

                case Integer integer:
                    empty();
                    return integer;

                case FloatingPoint floatingPoint:
                    empty();
                    return floatingPoint;

                case Hex hex:
                    empty();
                    return hex;

                case Fragment fragment:
                    empty();
                    return fragment;

                default:
                    return null;
            }
        }

        public bool parse_colon()
        {
            if (token is EmptyToken)
                advance();

            switch (token)
            {
                case Colon: case LineFeed:
                    empty();
                    return true;

                default:
                    return false;
            }
        }

        public bool is_at_end()
        {
            if (character == null)
                return true;

            return false;
        }

        public Position get_position()
        {
            return new Position
            {
                line = line,
                column = column,
            };
        }

        public Position get_previous_position()
        {
            return new Position
            {
                line = previous_line,
                column = previous_column,
            };
        }
    }

    public class Token
    {
    }

    public class EmptyToken : Token
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

    public class Attribute : Token
    {
        public string name;
        public Attribute(string name)
        {
            this.name = name;
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

    public class Fragment : Literal
    {
        public string value;
        public Fragment(string theString)
        {
            value = theString;
        }
    }

    public class Integer : Literal
    {
        public string value;
        public Integer(string theString)
        {
            value = theString;
        }
    }

    public class FloatingPoint : Literal
    {
        public string value;
        public FloatingPoint(string theString)
        {
            value = theString;
        }
    }

    public class Hex : Literal
    {
        public string value;
        public Hex(string theString)
        {
            value = theString;
        }
    }

    public class LineFeed : Token
    {
    }

    public class Colon : Token
    {
    }

    public class Semicolon : Token
    {
    }

    public class Position
    {
        public ulong line;
        public ulong column;
    }
}
