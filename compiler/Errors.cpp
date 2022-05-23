namespace scaly {
namespace compiler {

struct ParserError : Object {
    ParserError(String text, size_t position) : text(text), position(position) {}
    String text;
    size_t position;
};

}
}
