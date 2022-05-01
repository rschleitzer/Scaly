namespace scaly::compiler {
    
using namespace scaly::containers;

struct Parser : Object {
    Lexer* lexer;
    String file_name;
    static Parser* create(Page* _rp, String& deck) {
        auto parser = new(alignof(Parser), _rp) Parser();
        parser->lexer = Lexer::create(_rp, deck);
        return parser;
    }
};

}