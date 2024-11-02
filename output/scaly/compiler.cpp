#include "../scaly.h"
namespace scaly {

void compiler::test_lexer() {
    {
        auto r = Region();
        auto lexer = Lexer(String(r.get_page(), ""));
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Empty:
                {
                    auto e = _result._Empty;
                    lexer.advance();
                    break;
                }
                default:
                    exit(1);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Empty:
                {
                    auto e = _result._Empty;
                    {
                    };
                    break;
                }
                default:
                    exit(2);
            }
        };
    };
    {
        auto r = Region();
        auto lexer = Lexer(String(r.get_page(), " \t\r\n\
;single line comment\n\
;*multi\n\
line\n\
comment;*nested comment*;\n\
comment end*;\n\
abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1 @ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" 'a string identifier' `a string fragment \\`\\n\\r\\t`"));
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Empty:
                {
                    auto e = _result._Empty;
                    lexer.advance();
                    break;
                }
                default:
                    exit(3);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Identifier:
                {
                    auto identifier = _result._Identifier;
                    {
                        if (String(r.get_page(), "abc_AZ0815_7").equals(identifier.name) == false) 
                            exit(4);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(5);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Integer:
                                {
                                    auto integer = _result._Integer;
                                    {
                                        if (String(r.get_page(), "42").equals(integer.value) == false) 
                                            exit(6);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(7);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(8);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Colon:
                {
                    auto c = _result._Colon;
                    lexer.advance();
                    break;
                }
                default:
                    exit(9);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Colon:
                {
                    auto c = _result._Colon;
                    lexer.advance();
                    break;
                }
                default:
                    exit(10);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Integer:
                                {
                                    auto integer = _result._Integer;
                                    {
                                        if (String(r.get_page(), "0").equals(integer.value) == false) 
                                            exit(11);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(12);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(13);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Integer:
                                {
                                    auto integer = _result._Integer;
                                    {
                                        if (String(r.get_page(), "012").equals(integer.value) == false) 
                                            exit(14);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(15);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(16);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::FloatingPoint:
                                {
                                    auto fp = _result._FloatingPoint;
                                    {
                                        if (String(r.get_page(), "0.34").equals(fp.value) == false) 
                                            exit(17);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(18);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(19);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::FloatingPoint:
                                {
                                    auto fp = _result._FloatingPoint;
                                    {
                                        if (String(r.get_page(), "0.56E12").equals(fp.value) == false) 
                                            exit(20);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(21);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(22);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::FloatingPoint:
                                {
                                    auto fp = _result._FloatingPoint;
                                    {
                                        if (String(r.get_page(), "0.78e13").equals(fp.value) == false) 
                                            exit(23);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(24);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(25);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Hex:
                                {
                                    auto hex = _result._Hex;
                                    {
                                        if (String(r.get_page(), "0xaB").equals(hex.value) == false) 
                                            exit(26);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(27);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(28);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Hex:
                                {
                                    auto hex = _result._Hex;
                                    {
                                        if (String(r.get_page(), "0xCdEf02").equals(hex.value) == false) 
                                            exit(29);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(30);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(31);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Boolean:
                                {
                                    auto b = _result._Boolean;
                                    {
                                        if (b.value) 
                                            exit(32);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(33);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(34);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Boolean:
                                {
                                    auto b = _result._Boolean;
                                    {
                                        if (b.value == false) 
                                            exit(35);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(36);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(37);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Attribute:
                {
                    auto a = _result._Attribute;
                    {
                        if (String(r.get_page(), "ttribute").equals(a.name) == false) 
                            exit(38);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(39);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Identifier:
                {
                    auto identifier = _result._Identifier;
                    {
                        if (String(r.get_page(), "+").equals(identifier.name) == false) 
                            exit(40);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(41);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Identifier:
                {
                    auto identifier = _result._Identifier;
                    {
                        if (String(r.get_page(), "-").equals(identifier.name) == false) 
                            exit(42);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(43);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Integer:
                                {
                                    auto integer = _result._Integer;
                                    {
                                        if (String(r.get_page(), "0815").equals(integer.value) == false) 
                                            exit(44);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(45);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(8);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Identifier:
                {
                    auto identifier = _result._Identifier;
                    {
                        if (String(r.get_page(), "/*").equals(identifier.name) == false) 
                            exit(46);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(47);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Identifier:
                {
                    auto identifier = _result._Identifier;
                    {
                        if (String(r.get_page(), "<>").equals(identifier.name) == false) 
                            exit(48);
                        lexer.advance();
                    };
                    break;
                }
                default:
                    exit(49);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::String:
                                {
                                    auto string = _result._String;
                                    {
                                        if (String(r.get_page(), "a string").equals(string.value) == false) 
                                            exit(50);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(51);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(52);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::String:
                                {
                                    auto string = _result._String;
                                    {
                                        if (String(r.get_page(), "\\\"\\n\\r\\t").equals(string.value) == false) 
                                            exit(53);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(54);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(55);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Character:
                                {
                                    auto character = _result._Character;
                                    {
                                        if (String(r.get_page(), "a string identifier").equals(character.value) == false) 
                                            exit(56);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(57);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(58);
            }
        };
        {
            auto _result = lexer.token;
            switch (_result._tag)
            {
                case Token::Literal:
                {
                    auto literal = _result._Literal;
                    {
                        {
                            auto _result = literal;
                            switch (_result._tag)
                            {
                                case LiteralToken::Fragment:
                                {
                                    auto fragment = _result._Fragment;
                                    {
                                        if (String(r.get_page(), "a string fragment \\`\\n\\r\\t").equals(fragment.value) == false) 
                                            exit(58);
                                        lexer.advance();
                                    };
                                    break;
                                }
                                default:
                                    exit(59);
                            }
                        };
                    };
                    break;
                }
                default:
                    exit(60);
            }
        };
    };
}

void compiler::test() {
    test_lexer();
}

}