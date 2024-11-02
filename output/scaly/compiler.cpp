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
                        exit(-1);
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
                        exit(-2);
            }
        };
    };
    {
        auto r = Region();
        auto lexer = Lexer(String(r.get_page(), " \t\r\n;single line comment\n\
;*multi\nline\ncomment;*nested comment*;\n\
comment end*;\n\
abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1\n\
@ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" \n\
'a string identifier' `a string fragment \\`\\n\\r\\t`"));
    };
}

void compiler::test() {
    test_lexer();
}

}