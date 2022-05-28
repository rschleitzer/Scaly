namespace scaly {
namespace compiler {

struct ParserError : Object {
    ParserError(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

}
}
