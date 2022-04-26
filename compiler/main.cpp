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
            exit (1);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Empty)
            exit (2);
    }
    {
        auto r_1 = Region::create(r);
        auto s = *String::from_c_string(r_1.page,
" \t\r\n;single line comment\n"
";*multi\nline\ncomment;*nested comment*;"
"comment end*;"
"abc_AZ0815_7 42 "
        );
        Lexer& lexer = *Lexer::create(r_1.page, s);
        if (lexer.token->tag != Token::Empty)
            exit (3);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Identifier)
            exit (4);
        if (!lexer.token->identifier.name.equals(*String::from_c_string(r_1.page, "abc_AZ0815_7")))
            exit (5);
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Literal)
            exit (6);
        if (lexer.token->literal.tag != LiteralToken::Integer)
            exit (7);
        if (!lexer.token->literal.integer.value.equals(*String::from_c_string(r_1.page, "42")))
            exit (8);
    }
}


int main(int argc, char** argv) {
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);

    test_lexer(root_page);
}
