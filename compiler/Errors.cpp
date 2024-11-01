namespace scaly {
namespace compiler {

struct DifferentSyntax {};

struct InvalidSyntax {
    InvalidSyntax(size_t start, size_t end, String expected) : start(start), end(end), expected(expected) {}
    size_t start;
    size_t end;
    String expected;
};

struct ParserError : Object {
    ParserError(DifferentSyntax differentSyntax) : _tag(DifferentSyntax) { _DifferentSyntax = differentSyntax; }
    ParserError(InvalidSyntax invalidSyntax) : _tag(InvalidSyntax) { _InvalidSyntax = invalidSyntax; }
    enum {
        DifferentSyntax,
        InvalidSyntax,
    } _tag;
    union {
        struct DifferentSyntax _DifferentSyntax;
        struct InvalidSyntax _InvalidSyntax;
    };
};

}
}
