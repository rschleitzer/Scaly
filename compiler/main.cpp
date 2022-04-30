#include "Compiler.cpp"

using namespace scaly::memory;
using namespace scaly::containers;
using namespace scaly::compiler;

void test_lexer(Page* _rp) {
    auto r = Region::create_from_page(_rp);
    {
        auto r_1 = Region::create(r);
        Lexer& lexer = *Lexer::create(r_1.page, *String::from_c_string(r_1.page, ""));
        if (lexer.token->tag != Token::Empty)
            exit (-1);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Empty)
            exit (-2);
    }
    {
        auto r_1 = Region::create(r);
        auto s = *String::from_c_string(r_1.page,
" \t\r\n;single line comment\n"
";*multi\nline\ncomment;*nested comment*;"
"comment end*;"
"abc_AZ0815_7 42 \n : 0 012 0.34 0.56E12 0.78e13 0xaB 0B0 0B1"
        );
        Lexer& lexer = *Lexer::create(r_1.page, s);
        if (lexer.token->tag != Token::Empty)
            exit (-3);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Identifier)
            exit (-4);
        if (!lexer.token->identifier.name.equals(*String::from_c_string(r_1.page, "abc_AZ0815_7")))
            exit (-5);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-6);
        if (lexer.token->literal.tag != LiteralToken::Integer)
            exit (-7);
        if (!lexer.token->literal.integer.value.equals(*String::from_c_string(r_1.page, "42")))
            exit (-8);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::LineFeed)
            exit (-9);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Colon)
            exit (-10);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-11);
        if (lexer.token->literal.tag != LiteralToken::Integer)
            exit (-12);
        if (!lexer.token->literal.integer.value.equals(*String::from_c_string(r_1.page, "0")))
            exit (-13);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-14);
        if (lexer.token->literal.tag != LiteralToken::Integer)
            exit (-15);
        if (!lexer.token->literal.integer.value.equals(*String::from_c_string(r_1.page, "12")))
            exit (-16);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-17);
        if (lexer.token->literal.tag != LiteralToken::FloatingPoint)
            exit (-18);
        if (!lexer.token->literal.floatingPoint.value.equals(*String::from_c_string(r_1.page, "0.34")))
            exit (-18);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-19);
        if (lexer.token->literal.tag != LiteralToken::FloatingPoint)
            exit (-20);
        if (!lexer.token->literal.floatingPoint.value.equals(*String::from_c_string(r_1.page, "0.56E12")))
            exit (-21);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-22);
        if (lexer.token->literal.tag != LiteralToken::FloatingPoint)
            exit (-23);
        if (!lexer.token->literal.floatingPoint.value.equals(*String::from_c_string(r_1.page, "0.78e13")))
            exit (-24);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-25);
        if (lexer.token->literal.tag != LiteralToken::Hex)
            exit (-26);
        if (!lexer.token->literal.hex.value.equals(*String::from_c_string(r_1.page, "aB")))
            exit (-27);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (-28);
        if (lexer.token->literal.tag != LiteralToken::Boolean)
            exit (-29);
        if (!lexer.token->literal.boolean.value == '0')
            exit (-30);
        if (lexer.token->tag != Token::Literal)
            exit (-31);
        if (lexer.token->literal.tag != LiteralToken::Boolean)
            exit (-32);
        if (!lexer.token->literal.boolean.value == '1')
            exit (-33);
    }
}


int main(int argc, char** argv) {
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);

    test_lexer(root_page);
}
