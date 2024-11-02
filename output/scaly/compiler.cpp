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
abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1\n\
@ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" \n\
'a string identifier' `a string fragment \\`\\n\\r\\t`"));
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
    };
}

void compiler::test() {
    test_lexer();
}

}