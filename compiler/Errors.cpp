namespace scaly {
namespace compiler {

struct OtherSyntax {};

struct InvalidSyntax {
    InvalidSyntax(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ParserError : Object {
    ParserError(OtherSyntax otherSyntax) : _tag(OtherSyntax) { _OtherSyntax = otherSyntax; }
    ParserError(InvalidSyntax invalidSyntax) : _tag(InvalidSyntax) { _InvalidSyntax = invalidSyntax; }
    enum {
        OtherSyntax,
        InvalidSyntax,
    } _tag;
    union {
        struct OtherSyntax _OtherSyntax;
        struct InvalidSyntax _InvalidSyntax;
    };
};

}
}
