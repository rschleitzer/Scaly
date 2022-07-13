namespace scaly {
namespace compiler {
namespace model {

using namespace scaly::io;

struct Position {
    size_t line;
    size_t column;
    Position(size_t line, size_t column) : line(line), column(column) {}
};

String build_file_name(Region& _pr, Page* _rp, const Text& text) {
    Region _r(_pr);
    switch (text._tag) {
        case Text::Program:
            return String(_rp, "<Program>");
        case Text::File:
            return String(_rp, text._File);
    }
}

Position calculate_position_from_string(Region& _pr, Page* _rp, const String& text, size_t offset) {
    Region _r(_pr);
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

String build_hint_lines_from_string(Region& _pr, Page* _rp, const String& text, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    Region _r(_pr);
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
                if (current_column >= start_column && current_column < end_column)
                    indicator_builder.append_character('^');
                else
                    indicator_builder.append_character(' ');
            }
            current_column++;
        }
    }
    return output_builder.to_string(_rp);
}

Result<String, FileError> build_hint_lines(Region& _pr, Page* _rp, const Text& text, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    Region _r(_pr);
    switch (text._tag) {
        case Text::Program:
            return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Ok = build_hint_lines_from_string(_r, _rp, text._Program, start_offset, end_offset, start_position, end_position) };
        case Text::File: {
            auto text_result = File::read_to_string(_r, _r.page, _r.page, text._File);
            if (text_result._tag == Result<String, FileError>::Error) {
                return Result<String, FileError> { ._tag = Result<String, FileError>::Error, ._Error = FileError(text_result._Error) };
            }
            return Result<String, FileError> { ._tag = Result<String, FileError>::Ok, ._Ok = build_hint_lines_from_string(_r, _rp, text_result._Ok, start_offset, end_offset, start_position, end_position) };
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
            return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Error, ._Ok = calculate_position_from_string(_r, _rp, text._Program, offset) };
        case Text::File: {
            auto text_result = File::read_to_string(_r, _r.page, _r.page, text._File);
            if (text_result._tag == Result<String, FileError>::Error) {
                return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Error, ._Error = IoModelError(text_result._Error) };
            }
            return Result<Position, IoModelError> { ._tag = Result<Position, IoModelError>::Ok, ._Ok = calculate_position_from_string(_r, _rp, text_result._Ok, offset) };
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

    String to_string(Page* _rp, size_t number) {
        char str[22];
        sprintf(str, "%zd", number);
        return String(_rp, str);
    }

    String build_error_message(Region& _pr, Page* _rp, InvalidSyntax invalid_syntax) {
        Region _r(_pr);
        StringBuilder& message_builder = *new(alignof(StringBuilder), _r.page) StringBuilder();
        message_builder.append_string(build_file_name(_r, _rp, this->text));
        message_builder.append_character(':');
        auto position_result_start = calculate_position(_r, _rp, _r.page, this->text, invalid_syntax.start);
        if (position_result_start._tag == Result<Position, IoModelError>::Ok) {
            auto position_start = position_result_start._Ok;
            message_builder.append_string(to_string(_r.page, position_start.line));
            message_builder.append_character(':');
            message_builder.append_string(to_string(_r.page, position_start.column));
            message_builder.append_string(String(_rp, ": error: expected "));
            message_builder.append_string(invalid_syntax.expected);
            message_builder.append_character('.');
            message_builder.append_character('\n');
            auto position_result_end = calculate_position(_r, _rp, _r.page, this->text, invalid_syntax.end);
            if (position_result_end._tag == Result<Position, IoModelError>::Ok) {
                auto position_end = position_result_end._Ok;
                auto hint_lines_result = build_hint_lines(_r, _rp, this->text, invalid_syntax.start, invalid_syntax.end, position_start, position_end);
                if (hint_lines_result._tag == Result<String, FileError>::Ok) {
                    message_builder.append_string(hint_lines_result._Ok);
                }
            }
        }

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
                message_builder.append_string(String(_r.page, "A problem while building the model has occurred.\n"));
            break;
        }
        return message_builder.to_string(_rp);     
    }
};

}
}
}