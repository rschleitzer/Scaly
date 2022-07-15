namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Position {
    size_t line;
    size_t column;
    Position(size_t line, size_t column) : line(line), column(column) {}
};

String build_file_name(Page* _rp, const Text& text) {
    switch (text._tag) {
        case Text::Program:
            return String(_rp, "<Program>");
        case Text::File:
            return String(_rp, text._File);
    }
}

Position calculate_position_from_string(Page* _rp, const String& text, size_t offset) {
    StringIterator iterator(text);
    size_t line = 1;
    size_t column = 1;
    size_t counter = 0;
    while (auto character = iterator.next()) {
        counter++;
        if (offset == counter)
            return Position(line, column);
        if (*character == '\n') {
            line++;
            column = 1;
        }
        else {
            column++;
        }
    }
    return Position(0, 0);
}

String build_hint_lines_from_string(Page* _rp, const String& text, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    Region _r;
    StringIterator iterator(text);
    size_t current_line = 1;
    size_t current_column = 1;
    size_t counter = 0;
    auto start_line = start_position.line;
    auto end_line = end_position.line;
    auto start_column = start_position.column;
    auto end_column = end_position.column;
    StringBuilder& line_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    StringBuilder& indicator_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    StringBuilder& output_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
    while (auto character = iterator.next()) {
        counter++;
        if (*character == '\n') {
            if (current_line >= start_line && current_line <= end_line) {
                line_builder.append_character('\n');
                output_builder.append_string(line_builder.to_string(_r.page));
                indicator_builder.append_character('\n');
                output_builder.append_string(indicator_builder.to_string(_r.page));
            }
            current_line++;
            current_column = 1;
            if (current_line > end_line) {
                return output_builder.to_string(_rp);
            }
            else {
                line_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
                indicator_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
            }

        }
        else {
            if (current_line >= start_line && current_line <= end_line) {
                line_builder.append_character(*character);
                if (counter >= start_offset && counter < end_offset)
                    indicator_builder.append_character('^');
                else
                    indicator_builder.append_character(' ');
            }
            current_column++;
        }
    }

    line_builder.append_character('\n');
    output_builder.append_string(line_builder.to_string(_r.page));
    indicator_builder.append_character('\n');
    output_builder.append_string(indicator_builder.to_string(_r.page));
    return output_builder.to_string(_rp);
}

Result<String, FileError> build_hint_lines(Page* _rp, const Text& text, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    Region _r;
    switch (text._tag) {
        case Text::Program:
            return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = build_hint_lines_from_string(_rp, text._Program, start_offset, end_offset, start_position, end_position) };
        case Text::File: {
            auto text_result = File::read_to_string(_r.page, _r.page, text._File);
            if (text_result._tag == Result<String, FileError>::Error) {
                return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(text_result._Error) };
            }
            return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = build_hint_lines_from_string(_rp, text_result._Ok, start_offset, end_offset, start_position, end_position) };
        }
    }
}

Result<Position, FileError> calculate_position(Page* _rp, Page* _ep, const Text& text, size_t offset) {
    Region _r;
    switch (text._tag) {
        case Text::Program:
            return Result<Position, FileError> { ._tag = Result<Position, FileError>::Ok, ._Ok = calculate_position_from_string(_rp, text._Program, offset) };
        case Text::File: {
            auto text_result = File::read_to_string(_r.page, _r.page, text._File);
            if (text_result._tag == Result<String, FileError>::Error) {
                return Result<Position, FileError> { ._tag = Result<Position, FileError>::Error, ._Error = FileError(text_result._Error) };
            }
            return Result<Position, FileError> { ._tag = Result<Position, FileError>::Ok, ._Ok = calculate_position_from_string(_rp, text_result._Ok, offset) };
        }
    }
}

String to_string(Page* _rp, size_t number) {
    char str[22];
    sprintf(str, "%zd", number);
    return String(_rp, str);
}

void append_error_message_header(StringBuilder& builder, const Text& text, size_t offset) {
    Region _r;
    builder.append_string(build_file_name(_r.page, text));
    builder.append_character(':');
    auto position_result_start = calculate_position(_r.page, _r.page, text, offset);
    if (position_result_start._tag == Result<Position, FileError>::Ok) {
        auto position_start = position_result_start._Ok;
        builder.append_string(to_string(_r.page, position_start.line));
        builder.append_character(':');
        builder.append_string(to_string(_r.page, position_start.column));
        builder.append_string(String(_r.page, ": error: "));
    }
}

void append_hint_lines(StringBuilder& builder, const Text& text, size_t start, size_t end) {
    Region _r;
    builder.append_character('\n');
    auto position_result_end = calculate_position(_r.page, _r.page, text, end);
    if (position_result_end._tag == Result<Position, FileError>::Ok) {
        auto position_end = position_result_end._Ok;
        auto position_result_start = calculate_position(_r.page, _r.page, text, start);
        if (position_result_start._tag == Result<Position, FileError>::Ok) {
            auto hint_lines_result = build_hint_lines(_r.page, text, start, end, position_result_start._Ok, position_end);
            if (hint_lines_result._tag == Result<String, FileError>::Ok) {
                builder.append_string(hint_lines_result._Ok);
            }
        }
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

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case File:
                message_builder.append_string(_File.to_string(_rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

struct ParserModelError {
    ParserModelError(Text text, ParserError parser_error) : text(text), error(parser_error) {}

    Text text;
    ParserError error;

    String to_string(Page* _rp) {
        Region _r;
        switch (error._tag) {
            case ParserError::OtherSyntax:
                return String(_rp, "An other syntax was expected here.");     
            case ParserError::InvalidSyntax:
                return build_error_message(_rp,  error._InvalidSyntax);
        }
    }

    String build_error_message(Page* _rp, InvalidSyntax invalid_syntax) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, invalid_syntax.start);
        message_builder.append_string(String(_rp, "Expected "));
        message_builder.append_string(invalid_syntax.expected);
        message_builder.append_character('.');
        append_hint_lines(message_builder, this->text, invalid_syntax.start, invalid_syntax.end);

        return message_builder.to_string(_rp);
    }
};

struct NotImplemented
{
    NotImplemented(Text text, String name, Span span) : text(text), name(name), span(span) {}
    Text text;
    String name;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "The "));
        message_builder.append_string(name);
        message_builder.append_string(String(_rp, " syntax cannot be processed by the modeler yet."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct DuplicateName {
    DuplicateName(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "This declaration already exists."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct NonFunctionSymbolExists {
    NonFunctionSymbolExists(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "This declaration already exists, but not as a function."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct FunctionSymbolExists {
    FunctionSymbolExists(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "This declaration already exists, but as a function."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct DeInitializerExists {
    DeInitializerExists(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "A deinitializer has already been defined."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};
struct InvalidConstant {
    InvalidConstant(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "This is an invalid constant."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct InvalidComponentName {
    InvalidComponentName(Text text, Span span) : text(text), span(span) {}
    Text text;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        append_error_message_header(message_builder, this->text, span.start);
        message_builder.append_string(String(_rp, "The component does not have an identifier as name."));
        append_hint_lines(message_builder, this->text, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct ModelBuilderError {
    ModelBuilderError(NotImplemented _NotImplementedModelError) : _tag(NotImplemented), _NotImplemented(_NotImplementedModelError) {}
    ModelBuilderError(DuplicateName _DuplicateName) : _tag(DuplicateName), _DuplicateName(_DuplicateName) {}
    ModelBuilderError(NonFunctionSymbolExists _NonFunctionSymbolExists) : _tag(NonFunctionSymbolExists), _NonFunctionSymbolExists(_NonFunctionSymbolExists) {}
    ModelBuilderError(FunctionSymbolExists _FunctionSymbolExists) : _tag(FunctionSymbolExists), _FunctionSymbolExists(_FunctionSymbolExists) {}
    ModelBuilderError(DeInitializerExists _DeInitializerExists) : _tag(DeInitializerExists), _DeInitializerExists(_DeInitializerExists) {}
    ModelBuilderError(InvalidConstant _InvalidConstant) : _tag(InvalidConstant), _InvalidConstant(_InvalidConstant) {}
    ModelBuilderError(InvalidComponentName _InvalidComponentName) : _tag(InvalidComponentName), _InvalidComponentName(_InvalidComponentName) {}
    enum {
        NotImplemented,
        DuplicateName,
        NonFunctionSymbolExists,
        FunctionSymbolExists,
        DeInitializerExists,
        InvalidConstant,
        InvalidComponentName,
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct DuplicateName _DuplicateName;
        struct NonFunctionSymbolExists _NonFunctionSymbolExists;
        struct FunctionSymbolExists _FunctionSymbolExists;
        struct DeInitializerExists _DeInitializerExists;
        struct InvalidConstant _InvalidConstant;
        struct InvalidComponentName _InvalidComponentName;
    };

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case NotImplemented:
                message_builder.append_string(_NotImplemented.to_string(_rp));
            break;
            case DuplicateName:
                message_builder.append_string(_DuplicateName.to_string(_rp));
            break;
            case NonFunctionSymbolExists:
                message_builder.append_string(_NonFunctionSymbolExists.to_string(_rp));
            break;
            case FunctionSymbolExists:
                message_builder.append_string(_FunctionSymbolExists.to_string(_rp));
            break;
            case DeInitializerExists:
                message_builder.append_string(_DeInitializerExists.to_string(_rp));
            break;
            case InvalidConstant:
                message_builder.append_string(_InvalidConstant.to_string(_rp));
            break;
            case InvalidComponentName:
                message_builder.append_string(_InvalidComponentName.to_string(_rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
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

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        switch (_tag) {
            case Io:
                message_builder.append_string(_Io.to_string(_rp));
            break;
            case Parser:
                message_builder.append_string(_Parser.to_string(_rp));
            break;
            case Builder:
                message_builder.append_string(_Builder.to_string(_rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
}