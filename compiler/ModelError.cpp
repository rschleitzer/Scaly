namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Position {
    size_t line;
    size_t column;
    Position(size_t line, size_t column) : line(line), column(column) {}
};

Position calculate_position_from_string(Region& _pr, Page* _rp, Page* _ep, const String& text) {
    Region _r(_pr);
    return Position(0, 0);
}

String build_file_name(Region& _pr, Page* _rp, const Text& text) {
    Region _r(_pr);
    switch (text._tag) {
        case Text::Program:
            return String(_rp, "<Program>");
        case Text::File:
            return String(_rp, text._File);
    }
}

struct IoModelError {
    enum {
        File,
    } _tag;
    union {
        struct FileError _File;
    };
    IoModelError(FileError file_error) : _File(file_error) {}

    String to_string(Region& _pr, Page* _rp) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case File:
                message_builder.append_string(_File.to_string(_r, _rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

Result<Position, IoModelError> calculate_position(Region& _pr, Page* _rp, Page* _ep, const Text& text, size_t offset) {
    Region _r(_pr);
    switch (text._tag) {
        case Text::Program:
            return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Error, ._Ok = calculate_position_from_string(_r, _rp, _ep, text._Program) };
        case Text::File: {
            auto text_result = File::read_to_string(_r, _r.page, _r.page, text._File);
            if (text_result._tag == Result<String, FileError>::Error) {
                return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Error, ._Error = IoModelError(text_result._Error) };
            }
            return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Error, ._Ok = calculate_position_from_string(_r, _rp, _ep, text_result._Ok) };
        }
    }
}

struct ParserModelError {
    ParserModelError(Text text, ParserError parser_error) : text(text), error(parser_error) {}

    Text text;
    ParserError error;

    String to_string(Region& _pr, Page* _rp) {
        Region _r(_pr);
        switch (error._tag) {
            case ParserError::OtherSyntax:
                return String(_rp, "An other syntax was expected here.");     
            case ParserError::InvalidSyntax:
                return build_error_message(_r, _rp,  error._InvalidSyntax);
        }
    }

    String build_error_message(Region& _pr, Page* _rp, InvalidSyntax invalid_syntax) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        message_builder.append_string(build_file_name(_r, _rp, this->text));
        message_builder.append_character(':');
        auto position_result = calculate_position(_r, _rp, _r.page, this->text, invalid_syntax.start);
        switch (position_result._tag) {
            case Result<Position, IoModelError>::Error:
                break;
            case Result<Position, IoModelError>::Ok:
                break;
        }
        message_builder.append_string(String(_rp, ": "));
        message_builder.append_string(invalid_syntax.expected);
        message_builder.append_string(String(_rp, " was expectec here."));

        return message_builder.to_string(_rp);
    }
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
    ModelError(ParserModelError parserError) : _tag(Parser),  _Parser(parserError) {}
    ModelError(ModelBuilderError modelBuilderError) : _tag(Builder), _Builder(modelBuilderError) {}
    enum {
        Io,
        Parser,
        Builder,
    } _tag;
    union {
        IoModelError _Io;
        ParserModelError _Parser;
        ModelBuilderError _Builder;
    };

    String to_string(Region& _pr, Page* _rp) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case Io:
                message_builder.append_string(_Io.to_string(_r, _rp));
            break;
            case Parser:
                message_builder.append_string(_Parser.to_string(_r, _rp));
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