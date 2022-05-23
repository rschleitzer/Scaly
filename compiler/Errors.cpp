namespace scaly {
namespace compiler {

struct ParserError : Object {
    ParserError(String text, size_t start, size_t end) : text(text), start(start), end(end) {}
    String text;
    size_t start;
    size_t end;
};

}
}
