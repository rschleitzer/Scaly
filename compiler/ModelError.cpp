namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Position {
    size_t line;
    size_t column;
    Position(size_t line, size_t column) : line(line), column(column) {}
};

Position calculate_position_from_string(Page* _rp, String text, size_t offset) {
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
    StringBuilder& line_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
    StringBuilder& indicator_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
    StringBuilder& output_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
    while (auto character = iterator.next()) {
        counter++;
        if (*character == '\n') {
            if (current_line >= start_line && current_line <= end_line) {
                line_builder.append('\n');
                output_builder.append(line_builder.to_string(_r.get_page()));
                indicator_builder.append('\n');
                output_builder.append(indicator_builder.to_string(_r.get_page()));
            }
            current_line++;
            current_column = 1;
            if (current_line > end_line) {
                return output_builder.to_string(_rp);
            }
            else {
                line_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
                indicator_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
            }

        }
        else {
            if (current_line >= start_line && current_line <= end_line) {
                line_builder.append(*character);
                if (counter >= start_offset && counter < end_offset)
                    indicator_builder.append('^');
                else
                    indicator_builder.append(' ');
            }
            current_column++;
        }
    }

    line_builder.append('\n');
    output_builder.append(line_builder.to_string(_r.get_page()));
    indicator_builder.append('\n');
    output_builder.append(indicator_builder.to_string(_r.get_page()));
    return output_builder.to_string(_rp);
}

Result<String, FileError> build_hint_lines(Page* _rp, String file, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    Region _r;
    auto text_result = File::read_to_string(_r.get_page(), _r.get_page(), file);
    if (text_result._tag == Result<String, FileError>::Error) {
        return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(text_result._Error) };
    }
    return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = build_hint_lines_from_string(_rp, text_result._Ok, start_offset, end_offset, start_position, end_position) };
}

Result<Position, FileError> calculate_position(Page* _rp, Page* _ep, String file, size_t offset) {
    Region _r;
    auto text_result = File::read_to_string(_r.get_page(), _r.get_page(), file);
    if (text_result._tag == Result<String, FileError>::Error) {
        return Result<Position, FileError> { ._tag = Result<Position, FileError>::Error, ._Error = FileError(text_result._Error) };
    }
    return Result<Position, FileError> { ._tag = Result<Position, FileError>::Ok, ._Ok = calculate_position_from_string(_rp, text_result._Ok, offset) };
}

String to_string(Page* _rp, size_t number) {
    char str[22];
    snprintf(str, 22, "%zd", number);
    return String(_rp, str);
}

void append_error_message_header(StringBuilder& builder, String file, size_t offset) {
    Region _r;
    builder.append(file);
    builder.append(':');
    auto position_result_start = calculate_position(_r.get_page(), _r.get_page(), file, offset);
    if (position_result_start._tag == Result<Position, FileError>::Ok) {
        auto position_start = position_result_start._Ok;
        builder.append(to_string(_r.get_page(), position_start.line));
        builder.append(':');
        builder.append(to_string(_r.get_page(), position_start.column));
        builder.append(": error: ");
    }
}

void append_hint_lines(StringBuilder& builder, String file, size_t start, size_t end) {
    Region _r;
    builder.append('\n');
    auto position_result_end = calculate_position(_r.get_page(), _r.get_page(), file, end);
    if (position_result_end._tag == Result<Position, FileError>::Ok) {
        auto position_end = position_result_end._Ok;
        auto position_result_start = calculate_position(_r.get_page(), _r.get_page(), file, start);
        if (position_result_start._tag == Result<Position, FileError>::Ok) {
            auto hint_lines_result = build_hint_lines(_r.get_page(), file, start, end, position_result_start._Ok, position_end);
            if (hint_lines_result._tag == Result<String, FileError>::Ok) {
                builder.append(hint_lines_result._Ok);
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
    IoModelError(FileError file_error) : _tag(File), _File(file_error) {}

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        switch (_tag) {
            case File:
                message_builder.append(_File.to_string(_rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

struct ParserModelError {
    ParserModelError(String file, ParserError parser_error) : file(file), error(parser_error) {}

    String file;
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
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, invalid_syntax.start);
        message_builder.append("Expected ");
        message_builder.append(invalid_syntax.expected);
        message_builder.append('.');
        append_hint_lines(message_builder, this->file, invalid_syntax.start, invalid_syntax.end);

        return message_builder.to_string(_rp);
    }
};

struct NotImplemented
{
    NotImplemented(String file, String name, Span span) : file(file), name(name), span(span) {}
    String file;
    String name;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("The ");
        message_builder.append(name);
        message_builder.append(" syntax cannot be processed by the modeler yet.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct DuplicateName {
    DuplicateName(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("This declaration already exists.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct NonFunctionSymbolExists {
    NonFunctionSymbolExists(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("This declaration already exists, but not as a function.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct FunctionSymbolExists {
    FunctionSymbolExists(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("This declaration already exists, but as a function.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct DeInitializerExists {
    DeInitializerExists(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("A deinitializer has already been defined.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};
struct InvalidConstant {
    InvalidConstant(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("This is an invalid constant.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct InvalidComponentName {
    InvalidComponentName(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("The component does not have an identifier as name.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

        return message_builder.to_string(_rp);
    }
};

struct ModuleRootMustBeConcept {
    ModuleRootMustBeConcept(String file, Span span) : file(file), span(span) {}
    String file;
    Span span;

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        append_error_message_header(message_builder, this->file, span.start);
        message_builder.append("The root definition of a module must be a concept.");
        append_hint_lines(message_builder, this->file, span.start, span.end);

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
    ModelBuilderError(ModuleRootMustBeConcept _ModuleRootMustBeConcept) : _tag(ModuleRootMustBeConcept), _ModuleRootMustBeConcept(_ModuleRootMustBeConcept) {}
    enum {
        NotImplemented,
        DuplicateName,
        NonFunctionSymbolExists,
        FunctionSymbolExists,
        DeInitializerExists,
        InvalidConstant,
        InvalidComponentName,
        ModuleRootMustBeConcept
    } _tag;
    union {
        struct NotImplemented _NotImplemented;
        struct DuplicateName _DuplicateName;
        struct NonFunctionSymbolExists _NonFunctionSymbolExists;
        struct FunctionSymbolExists _FunctionSymbolExists;
        struct DeInitializerExists _DeInitializerExists;
        struct InvalidConstant _InvalidConstant;
        struct InvalidComponentName _InvalidComponentName;
        struct ModuleRootMustBeConcept _ModuleRootMustBeConcept;
    };

    String to_string(Page* _rp) {
        Region _r;
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        switch (_tag) {
            case NotImplemented:
                message_builder.append(_NotImplemented.to_string(_rp));
            break;
            case DuplicateName:
                message_builder.append(_DuplicateName.to_string(_rp));
            break;
            case NonFunctionSymbolExists:
                message_builder.append(_NonFunctionSymbolExists.to_string(_rp));
            break;
            case FunctionSymbolExists:
                message_builder.append(_FunctionSymbolExists.to_string(_rp));
            break;
            case DeInitializerExists:
                message_builder.append(_DeInitializerExists.to_string(_rp));
            break;
            case InvalidConstant:
                message_builder.append(_InvalidConstant.to_string(_rp));
            break;
            case InvalidComponentName:
                message_builder.append(_InvalidComponentName.to_string(_rp));
            break;
            case ModuleRootMustBeConcept:
                message_builder.append(_ModuleRootMustBeConcept.to_string(_rp));
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
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.get_page()) StringBuilder();
        switch (_tag) {
            case Io:
                message_builder.append(_Io.to_string(_rp));
            break;
            case Parser:
                message_builder.append(_Parser.to_string(_rp));
            break;
            case Builder:
                message_builder.append(_Builder.to_string(_rp));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
}