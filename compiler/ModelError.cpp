namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct IoModelError {
    FileError file_error;
    IoModelError(FileError file_error) : file_error(file_error) {}
};

struct NotImplemented
{
    NotImplemented(Span _Span) : span(_Span) {}
    Span span;
};

struct DuplicateName {
    DuplicateName(Span span) : span(span) {}
    Span span;
};

struct NonFunctionSymbolExists {
    NonFunctionSymbolExists(Span span) : span(span) {}
    Span span;
};

struct FunctionSymbolExists {
    FunctionSymbolExists(Span span) : span(span) {}
    Span span;
};

struct InvalidConstant {
    InvalidConstant(Span span) : span(span) {}
    Span span;
};

struct ModelBuilderError {
    ModelBuilderError(NotImplemented _NotImplementedModelError) : _tag(NotImplemented), _NotImplemented(_NotImplementedModelError) {}
    ModelBuilderError(DuplicateName _DuplicateName) : _tag(DuplicateName), _DuplicateName(_DuplicateName) {}
    ModelBuilderError(NonFunctionSymbolExists _NonFunctionSymbolExists) : _tag(NonFunctionSymbolExists), _NonFunctionSymbolExists(_NonFunctionSymbolExists) {}
    ModelBuilderError(FunctionSymbolExists _FunctionSymbolExists) : _tag(FunctionSymbolExists), _FunctionSymbolExists(_FunctionSymbolExists) {}
    ModelBuilderError(InvalidConstant _InvalidConstant) : _tag(InvalidConstant), _InvalidConstant(_InvalidConstant) {}
    enum {
        NotImplemented,
        DuplicateName,
        NonFunctionSymbolExists,
        FunctionSymbolExists,
        InvalidConstant,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct DuplicateName _DuplicateName;
        struct NonFunctionSymbolExists _NonFunctionSymbolExists;
        struct FunctionSymbolExists _FunctionSymbolExists;
        struct InvalidConstant _InvalidConstant;
    };
};

struct ModelError : Object {
    ModelError(IoModelError ioModelError) : _tag(Io), _Io(ioModelError) {}
    ModelError(ParserError parserError) : _tag(Parser), _Parser(parserError) {}
    ModelError(ModelBuilderError modelBuilderError) : _tag(Builder), _Builder(modelBuilderError) {}
    enum {
        Io,
        Parser,
        Builder,
    } _tag;
    union {
        IoModelError _Io;
        ParserError _Parser;
        ModelBuilderError _Builder;
    };

    String to_string(Region& _pr, Page* _rp) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case Io:
                message_builder.append_string(String(_r.page, "A problem with I/O has occurred."));
            break;
            case Parser:
                message_builder.append_string(String(_r.page, "A parser error hase occurred."));
            break;
            case Builder:
                message_builder.append_string(String(_r.page, "A problem while building the model has occurred."));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
}