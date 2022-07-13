#include "Compiler.cpp"

using namespace scaly::memory;
using namespace scaly::containers;
using namespace scaly::compiler;

void test_lexer() {
    {
        Region _r_1;
        Lexer& lexer = *new(alignof(Lexer), _r_1.page) Lexer(String(_r_1.page, ""));
        if (lexer.token._tag != Token::Empty)
            exit (-1);
        lexer.advance();
        if (lexer.token._tag != Token::Empty)
            exit (-2);
    }
    {
        Region _r_1;
        auto s = String(_r_1.page,
" \t\r\n;single line comment\n"
";*multi\nline\ncomment;*nested comment*;"
"comment end*;"
"abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1"
"@ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" "
"'a string identifier' `a string fragment \\`\\n\\r\\t`"
        );
        Lexer& lexer = *new(alignof(Lexer), _r_1.page) Lexer(String(_r_1.page, s));
        if (lexer.token._tag != Token::Empty)
            exit (-3);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-4);
        if (!String(_r_1.page, "abc_AZ0815_7").equals(lexer.token._Identifier.name))
            exit (-5);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-6);
        if (lexer.token._Literal._tag != LiteralToken::Integer)
            exit (-7);
        if (!String(_r_1.page, "42").equals(lexer.token._Literal._Integer.value))
            exit (-8);
        lexer.advance();
        if (lexer.token._tag != Token::LineFeed)
            exit (-9);
        lexer.advance();
        if (lexer.token._tag != Token::Colon)
            exit (-10);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-11);
        if (lexer.token._Literal._tag != LiteralToken::Integer)
            exit (-12);
        if (!String(_r_1.page, "0").equals(lexer.token._Literal._Integer.value))
            exit (-13);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-14);
        if (lexer.token._Literal._tag != LiteralToken::Integer)
            exit (-15);
        if (!String(_r_1.page, "012").equals(lexer.token._Literal._Integer.value))
            exit (-16);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-17);
        if (lexer.token._Literal._tag != LiteralToken::FloatingPoint)
            exit (-18);
        if (!String(_r_1.page, "0.34").equals(lexer.token._Literal._FloatingPoint.value))
            exit (-18);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-19);
        if (lexer.token._Literal._tag != LiteralToken::FloatingPoint)
            exit (-20);
        if (!String(_r_1.page, "0.56E12").equals(lexer.token._Literal._FloatingPoint.value))
            exit (-21);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-22);
        if (lexer.token._Literal._tag != LiteralToken::FloatingPoint)
            exit (-23);
        if (!String(_r_1.page, "0.78e13").equals(lexer.token._Literal._FloatingPoint.value))
            exit (-24);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-25);
        if (lexer.token._Literal._tag != LiteralToken::Hex)
            exit (-26);
        if (!String(_r_1.page, "0xaB").equals(lexer.token._Literal._Hex.value))
            exit (-27);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-28);
        if (lexer.token._Literal._tag != LiteralToken::Hex)
            exit (-29);
        if (!String(_r_1.page, "0xCdEf02").equals(lexer.token._Literal._Hex.value))
            exit (-30);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-31);
        if (lexer.token._Literal._tag != LiteralToken::Boolean)
            exit (-32);
        if (lexer.token._Literal._Boolean.value)
            exit (-33);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-34);
        if (lexer.token._Literal._tag != LiteralToken::Boolean)
            exit (-35);
        if (!lexer.token._Literal._Boolean.value)
            exit (-36);
        lexer.advance();
        if (lexer.token._tag != Token::Attribute)
            exit (-37);
        if (!String(_r_1.page, "ttribute").equals(lexer.token._Attribute.name))
            exit (-38);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-39);
        if (!String(_r_1.page, "+").equals(lexer.token._Identifier.name))
            exit (-40);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-41);
        if (!String(_r_1.page, "-").equals(lexer.token._Identifier.name))
            exit (-42);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-43);
        if (lexer.token._Literal._tag != LiteralToken::Integer)
            exit (-44);
        if (!String(_r_1.page, "0815").equals(lexer.token._Literal._Integer.value))
            exit (-45);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-46);
        if (!String(_r_1.page, "/*").equals(lexer.token._Identifier.name))
            exit (-47);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-46);
        if (!String(_r_1.page, "<>").equals(lexer.token._Identifier.name))
            exit (-47);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-48);
        if (lexer.token._Literal._tag != LiteralToken::String)
            exit (-49);
        if (!String(_r_1.page, "a string").equals(lexer.token._Literal._String.value))
            exit (-50);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-51);
        if (lexer.token._Literal._tag != LiteralToken::String)
            exit (-52);
        if (!String(_r_1.page, "\\\"\\n\\r\\t").equals(lexer.token._Literal._String.value))
            exit (-53);
        lexer.advance();
        if (lexer.token._tag != Token::Identifier)
            exit (-54);
        if (!String(_r_1.page, "a string identifier").equals(lexer.token._Identifier.name))
            exit (-55);
        lexer.advance();
        if (lexer.token._tag != Token::Literal)
            exit (-56);
        if (lexer.token._Literal._tag != LiteralToken::Fragment)
            exit (-57);
        if (!String(_r_1.page, "a string fragment \\`\\n\\r\\t").equals(lexer.token._Literal._Fragment.value))
            exit (-58);
    }
}

void test_parser() {
    Region _r;
    Parser& parser = *new (alignof(Parser), _r.page) Parser(_r.page, String(_r.page, "define a 1"));
    auto file_syntax = parser.parse_file(_r.page, _r.page);
    if (file_syntax._tag != Result<FileSyntax*, ParserError*>::Ok)
        exit(-1);
}

void test_generator() {
    Region _r;
    InitializeModule();

}

void test_compiler() {
    Region _r;
    StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder(String(_r.page, "test_compiler failed: "));
    auto error = compile_and_run_program(_r.page, String(_r.page, "0"), *new(alignof(Vector<String>), _r.page) Vector<String>(_r.page, 0));
    if (error != nullptr) {
        auto error_message = error->to_string(_r.page);
        print(_r.page, error_message);
        exit (-60);
    }

}

int main(int argc, char** argv) {
    test_lexer();
    test_parser();
    test_generator();
    test_compiler();
}
