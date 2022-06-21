#include "Compiler.cpp"

using namespace scaly::memory;
using namespace scaly::containers;
using namespace scaly::compiler;

void test_lexer(Page* _rp) {
    auto r = Region(_rp);
    {
        auto _r_1 = Region::create(r);
        Lexer& lexer = *new(alignof(Lexer), _r_1.page) Lexer(String(_r_1.page, ""));
        if (lexer.token->_tag != Token::Empty)
            exit (-1);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Empty)
            exit (-2);
    }
    {
        auto _r_1 = Region::create(r);
        auto s = String(_r_1.page,
" \t\r\n;single line comment\n"
";*multi\nline\ncomment;*nested comment*;"
"comment end*;"
"abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0xCdEf02 0B0 0B1"
"@ttribute + -0815 /* <> \"a string\" \"\\\"\\n\\r\\t\" "
"'a string identifier' `a string fragment \\`\\n\\r\\t`"
        );
        Lexer& lexer = *new(alignof(Lexer), _r_1.page) Lexer(s);
        if (lexer.token->_tag != Token::Empty)
            exit (-3);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-4);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "abc_AZ0815_7")))
            exit (-5);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-6);
        if (lexer.token->_Literal._tag != LiteralToken::Integer)
            exit (-7);
        if (!lexer.token->_Literal._Integer.value.equals(String(_r_1.page, "42")))
            exit (-8);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::LineFeed)
            exit (-9);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Colon)
            exit (-10);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-11);
        if (lexer.token->_Literal._tag != LiteralToken::Integer)
            exit (-12);
        if (!lexer.token->_Literal._Integer.value.equals(String(_r_1.page, "0")))
            exit (-13);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-14);
        if (lexer.token->_Literal._tag != LiteralToken::Integer)
            exit (-15);
        if (!lexer.token->_Literal._Integer.value.equals(String(_r_1.page, "12")))
            exit (-16);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-17);
        if (lexer.token->_Literal._tag != LiteralToken::FloatingPoint)
            exit (-18);
        if (!lexer.token->_Literal._FloatingPoint.value.equals(String(_r_1.page, "0.34")))
            exit (-18);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-19);
        if (lexer.token->_Literal._tag != LiteralToken::FloatingPoint)
            exit (-20);
        if (!lexer.token->_Literal._FloatingPoint.value.equals(String(_r_1.page, "0.56E12")))
            exit (-21);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-22);
        if (lexer.token->_Literal._tag != LiteralToken::FloatingPoint)
            exit (-23);
        if (!lexer.token->_Literal._FloatingPoint.value.equals(String(_r_1.page, "0.78e13")))
            exit (-24);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-25);
        if (lexer.token->_Literal._tag != LiteralToken::Hex)
            exit (-26);
        if (!lexer.token->_Literal._Hex.value.equals(String(_r_1.page, "aB")))
            exit (-27);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-28);
        if (lexer.token->_Literal._tag != LiteralToken::Hex)
            exit (-29);
        if (!lexer.token->_Literal._Hex.value.equals(String(_r_1.page, "CdEf02")))
            exit (-30);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-31);
        if (lexer.token->_Literal._tag != LiteralToken::Boolean)
            exit (-32);
        if (lexer.token->_Literal._Boolean.value)
            exit (-33);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-34);
        if (lexer.token->_Literal._tag != LiteralToken::Boolean)
            exit (-35);
        if (!lexer.token->_Literal._Boolean.value)
            exit (-36);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Attribute)
            exit (-37);
        if (!lexer.token->_Attribute.name.equals(String(_r_1.page, "ttribute")))
            exit (-38);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-39);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "+")))
            exit (-40);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-41);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "-")))
            exit (-42);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-43);
        if (lexer.token->_Literal._tag != LiteralToken::Integer)
            exit (-44);
        if (!lexer.token->_Literal._Integer.value.equals(String(_r_1.page, "815")))
            exit (-45);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-46);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "/*")))
            exit (-47);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-46);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "<>")))
            exit (-47);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-48);
        if (lexer.token->_Literal._tag != LiteralToken::String)
            exit (-49);
        if (!lexer.token->_Literal._String.value.equals(String(_r_1.page, "a string")))
            exit (-50);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-51);
        if (lexer.token->_Literal._tag != LiteralToken::String)
            exit (-52);
        if (!lexer.token->_Literal._String.value.equals(String(_r_1.page, "\"\n\r\t")))
            exit (-53);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Identifier)
            exit (-54);
        if (!lexer.token->_Identifier.name.equals(String(_r_1.page, "a string identifier")))
            exit (-55);
        lexer.advance(_r_1);
        if (lexer.token->_tag != Token::Literal)
            exit (-56);
        if (lexer.token->_Literal._tag != LiteralToken::Fragment)
            exit (-57);
        if (!lexer.token->_Literal._Fragment.value.equals(String(_r_1.page, "a string fragment `\n\r\t")))
            exit (-58);
    }
}

void test_parser(Region& _pr)
{
    auto _r = Region::create(_pr);
    Parser& parser = *new (alignof(Parser), _r.page) Parser(_r, _r.page, String(_r.page, "define a 1"));
    auto file_syntax = parser.parse_file(_r, _r.page, _r.page);
    if (file_syntax._tag != Result<FileSyntax*, ParserError*>::Ok)
        exit(-1);
}

void test_generator(Region& _pr)
{
    auto r = Region::create(_pr);
    InitializeModule();

}

void test_compiler(Region& _pr)
{
    auto r = Region::create(_pr);
    if (compile_and_run_program(_pr, String(r.page, "0"), *new(alignof(Vector<String>), r.page) Vector<String>(r.page, 0)) != 0)
        exit (-60);

}

int main(int argc, char** argv) {
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);
    auto region = Region(root_page);

    test_lexer(root_page);
    test_parser(region);
    test_generator(region);
    test_compiler(region);
}
