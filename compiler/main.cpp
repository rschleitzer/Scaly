#include "Compiler.cpp"

using namespace scaly::memory;
using namespace scaly::containers;
using namespace scaly::compiler;

void test_lexer(Page* _rp) {
    auto r = Region::create_from_page(_rp);
    {
        auto r_1 = Region::create(r);
        Lexer& lexer = *Lexer::create(r_1.page, *String::from_c_string(r_1.page, "abc"));
        lexer.advance(r_1.page);
        if (lexer.token->tag != Token::Identifier)
            exit (1); 
    }
}


int main(int argc, char** argv) {
    auto heap = Heap::create();
    auto root_stack_bucket = StackBucket::create(&heap);
    auto root_page = Page::get(root_stack_bucket);

    test_lexer(root_page);
}
