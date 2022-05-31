namespace scaly {
namespace compiler {

struct OtherSyntaxParserError {};
struct InvalidSyntaxParserError {
    InvalidSyntaxParserError(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ParserError : Object {
    ParserError(OtherSyntaxParserError _OtherSyntaxParserError) : _tag(OtherSyntax) { _OtherSyntax = _OtherSyntaxParserError; }
    ParserError(InvalidSyntaxParserError _InvalidSyntaxParserError) : _tag(InvalidSyntax) { _InvalidSyntax = _InvalidSyntaxParserError; }
    enum {
        OtherSyntax,
        InvalidSyntax,
    } _tag;
    union {
        OtherSyntaxParserError _OtherSyntax;
        InvalidSyntaxParserError _InvalidSyntax;
    };
};

}
}
