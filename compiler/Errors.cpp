namespace scaly {
namespace compiler {

struct OtherSyntaxParserError {};
struct InvalidSyntaxParserError {
    InvalidSyntaxParserError(size_t start, size_t end) : start(start), end(end) {}
    size_t start;
    size_t end;
};

struct ParserError : Object {
    ParserError(OtherSyntaxParserError other_syntax_parser_error) : _tag(OtherSyntax) { _other_syntax = other_syntax_parser_error; }
    ParserError(InvalidSyntaxParserError invalid_syntax_parser_error) : _tag(InvalidSyntax) { _invalid_syntax = invalid_syntax_parser_error; }
    enum {
        OtherSyntax,
        InvalidSyntax,
    } _tag;
    union {
        OtherSyntaxParserError _other_syntax;
        InvalidSyntaxParserError _invalid_syntax;
    };
};

}
}
