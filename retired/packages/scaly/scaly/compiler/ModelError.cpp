#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


Position::Position(size_t line, size_t column) : line(line), column(column) {}

Position calculate_position_from_string(String text, size_t offset) {
    const auto iterator = text.get_iterator();
    size_t line = 1;
    size_t column = 1;
    size_t counter = 0;
    
    auto _character_iterator = text.get_iterator();
    while (auto _character = _character_iterator.next()) {
        auto character = *_character;{
            counter = counter+1;
            if (offset == counter) 
                return Position(line, column);
            if (character == '\n') {
                line = line+1;
                column = 1;
            }
            else {
                column++;
            };
        }
    };
    return Position(0, 0);
}

String build_hint_lines_from_string(Page* rp, String text, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    auto r = Region();
    size_t current_line = 1;
    size_t current_column = 1;
    size_t counter = 0;
    const auto start_line = start_position.line;
    const auto end_line = end_position.line;
    const auto start_column = start_position.column;
    const auto end_column = end_position.column;
    auto line_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
    auto indicator_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
    auto output_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
    
    auto _character_iterator = text.get_iterator();
    while (auto _character = _character_iterator.next()) {
        auto character = *_character;{
            counter++;
            if (character == '\n') {
                if (current_line>=start_line&&current_line<=end_line) {
                    (*line_builder).append('\n');
                    (*output_builder).append((*line_builder).to_string(r.get_page()));
                    (*indicator_builder).append('\n');
                    (*output_builder).append((*indicator_builder).to_string(r.get_page()));
                };
                current_line++;
                current_column = 1;
                if (current_line>end_line) {
                    return (*output_builder).to_string(rp);
                }
                else {
                    line_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
                    indicator_builder = new (alignof(StringBuilder), r.get_page()) StringBuilder();
                };
            }
            else {
                if (current_line>=start_line&&current_line<=end_line) {
                    (*line_builder).append(character);
                    if ((counter>=start_offset&&counter<end_offset)) {
                        (*indicator_builder).append('^');
                    }
                    else {
                        (*indicator_builder).append(' ');
                    };
                };
                current_column = current_column+1;
            };
        }
    };
    (*line_builder).append('\n');
    (*output_builder).append((*line_builder).to_string(r.get_page()));
    (*indicator_builder).append('\n');
    (*output_builder).append((*indicator_builder).to_string(r.get_page()));
    return (*output_builder).to_string(rp);
}

Result<String, FileError> build_hint_lines(Page* rp, Page* ep, String file, size_t start_offset, size_t end_offset, Position start_position, Position end_position) {
    auto r = Region();
    const auto _text_result = File::read_to_string(r.get_page(), r.get_page(), file);
    auto text = _text_result._Ok;
    if (_text_result._tag == Success::Error) {
        const auto _text_Error = _text_result._Error;
        switch (_text_Error._tag) {
        default:
            return Result<String, FileError>(_text_result._Error);

        }
    };
    return Result<String, FileError>(build_hint_lines_from_string(rp, text, start_offset, end_offset, start_position, end_position));
}

Result<Position, FileError> calculate_position(Page* rp, Page* ep, String file, size_t offset) {
    auto r = Region();
    const auto _text_result = File::read_to_string(r.get_page(), r.get_page(), file);
    auto text = _text_result._Ok;
    if (_text_result._tag == Success::Error) {
        const auto _text_Error = _text_result._Error;
        switch (_text_Error._tag) {
        default:
            return Result<Position, FileError>(_text_result._Error);

        }
    };
    return Result<Position, FileError>(calculate_position_from_string(text, offset));
}

String number_to_string(Page* rp, size_t number) {
    auto r = Region();
    auto str = (char*)(*r.get_page()).allocate(32, 1);
    snprintf(str, 22, "%zd", number);
    return String(rp, str);
}

void append_error_message_header(StringBuilder& builder, String file, size_t offset) {
    auto r = Region();
    builder.append(file);
    builder.append(':');
    {
        auto _result = calculate_position(r.get_page(), r.get_page(), file, offset);
        switch (_result._tag)
        {
            case Success::Ok:
            {
                auto position_start = _result._Ok;
                {
                    builder.append(number_to_string(r.get_page(), position_start.line));
                    builder.append(':');
                    builder.append(number_to_string(r.get_page(), position_start.column));
                    builder.append(": error: ");
                };
                break;
            }
            default:
                {
            };
        }
    };
}

void append_hint_lines(StringBuilder& builder, String file, size_t start, size_t end) {
    auto r = Region();
    builder.append('\n');
    {
        auto _result = calculate_position(r.get_page(), r.get_page(), file, end);
        switch (_result._tag)
        {
            case Success::Ok:
            {
                auto position_end = _result._Ok;
                {
                    {
                        auto _result = calculate_position(r.get_page(), r.get_page(), file, start);
                        switch (_result._tag)
                        {
                            case Success::Ok:
                            {
                                auto position_start = _result._Ok;
                                {
                                    {
                                        auto _result = build_hint_lines(r.get_page(), r.get_page(), file, start, end, position_start, position_end);
                                        switch (_result._tag)
                                        {
                                            case Success::Ok:
                                            {
                                                auto hint_lines = _result._Ok;
                                                builder.append(hint_lines);
                                                break;
                                            }
                                            default:
                                                {
                                            };
                                        }
                                    };
                                };
                                break;
                            }
                            default:
                                {
                            };
                        }
                    };
                };
                break;
            }
            default:
                {
            };
        }
    };
}

String IoModelError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case IoModelError::File:
            {
                auto file = _result._File;
                message_builder.append(file.to_string(rp));
                break;
            }
            default:
                {
            };
        }
    };
    return message_builder.to_string(rp);
}
IoModelError::IoModelError(struct FileError _File) : _tag(File), _File(_File) {}


ParserModelError::ParserModelError(String file, ParserError error) : file(file), error(error) {}

String ParserModelError::to_string(Page* rp) {
    auto r = Region();
    {
        auto _result = error;
        switch (_result._tag)
        {
            case ParserError::Different:
            {
                auto d = _result._Different;
                return String(rp, "An other syntax was expected here.");
                break;
            }
            case ParserError::Invalid:
            {
                auto i = _result._Invalid;
                return build_error_message(rp, i);
                break;
            }
        }
    };
}

String ParserModelError::build_error_message(Page* rp, InvalidSyntax invalid_syntax) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, invalid_syntax.start);
    message_builder.append("Expected ");
    message_builder.append(invalid_syntax.expected);
    message_builder.append('.');
    append_hint_lines(message_builder, file, invalid_syntax.start, invalid_syntax.end);
    return message_builder.to_string(rp);
}

NotImplemented::NotImplemented(String file, String name, Span span) : file(file), name(name), span(span) {}

String NotImplemented::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The ");
    message_builder.append(name);
    message_builder.append(" syntax cannot be processed by the modeler yet.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

DuplicateName::DuplicateName(String file, Span span) : file(file), span(span) {}

String DuplicateName::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("This declaration already exists.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

NonFunctionSymbolExists::NonFunctionSymbolExists(String file, Span span) : file(file), span(span) {}

String NonFunctionSymbolExists::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("This declaration already exists, but not as a function.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

FunctionSymbolExists::FunctionSymbolExists(String file, Span span) : file(file), span(span) {}

String FunctionSymbolExists::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("This declaration already exists, but as a function.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

DeInitializerExists::DeInitializerExists(String file, Span span) : file(file), span(span) {}

String DeInitializerExists::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("A deinitializer has already been defined.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

InvalidConstant::InvalidConstant(String file, Span span) : file(file), span(span) {}

String InvalidConstant::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("This is an invalid constant.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

InvalidComponentName::InvalidComponentName(String file, Span span) : file(file), span(span) {}

String InvalidComponentName::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The component does not have an identifier as name.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

ModuleRootMustBeConcept::ModuleRootMustBeConcept(String file, Span span) : file(file), span(span) {}

String ModuleRootMustBeConcept::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    append_error_message_header(message_builder, file, span.start);
    message_builder.append("The root definition of a module must be a concept.");
    append_hint_lines(message_builder, file, span.start, span.end);
    return message_builder.to_string(rp);
}

String ModelBuilderError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case ModelBuilderError::NotImplemented:
            {
                auto ni = _result._NotImplemented;
                message_builder.append(ni.to_string(rp));
                break;
            }
            case ModelBuilderError::DuplicateName:
            {
                auto dn = _result._DuplicateName;
                message_builder.append(dn.to_string(rp));
                break;
            }
            case ModelBuilderError::NonFunctionSymbolExists:
            {
                auto nfse = _result._NonFunctionSymbolExists;
                message_builder.append(nfse.to_string(rp));
                break;
            }
            case ModelBuilderError::FunctionSymbolExists:
            {
                auto fse = _result._FunctionSymbolExists;
                message_builder.append(fse.to_string(rp));
                break;
            }
            case ModelBuilderError::DeInitializerExists:
            {
                auto di = _result._DeInitializerExists;
                message_builder.append(di.to_string(rp));
                break;
            }
            case ModelBuilderError::InvalidConstant:
            {
                auto ic = _result._InvalidConstant;
                message_builder.append(ic.to_string(rp));
                break;
            }
            case ModelBuilderError::InvalidComponentName:
            {
                auto icn = _result._InvalidComponentName;
                message_builder.append(icn.to_string(rp));
                break;
            }
            case ModelBuilderError::ModuleRootMustBeConcept:
            {
                auto mrmbc = _result._ModuleRootMustBeConcept;
                message_builder.append(mrmbc.to_string(rp));
                break;
            }
        }
    };
    return message_builder.to_string(rp);
}
ModelBuilderError::ModelBuilderError(struct NotImplemented _NotImplemented) : _tag(NotImplemented), _NotImplemented(_NotImplemented) {}

ModelBuilderError::ModelBuilderError(struct DuplicateName _DuplicateName) : _tag(DuplicateName), _DuplicateName(_DuplicateName) {}

ModelBuilderError::ModelBuilderError(struct NonFunctionSymbolExists _NonFunctionSymbolExists) : _tag(NonFunctionSymbolExists), _NonFunctionSymbolExists(_NonFunctionSymbolExists) {}

ModelBuilderError::ModelBuilderError(struct FunctionSymbolExists _FunctionSymbolExists) : _tag(FunctionSymbolExists), _FunctionSymbolExists(_FunctionSymbolExists) {}

ModelBuilderError::ModelBuilderError(struct DeInitializerExists _DeInitializerExists) : _tag(DeInitializerExists), _DeInitializerExists(_DeInitializerExists) {}

ModelBuilderError::ModelBuilderError(struct InvalidConstant _InvalidConstant) : _tag(InvalidConstant), _InvalidConstant(_InvalidConstant) {}

ModelBuilderError::ModelBuilderError(struct InvalidComponentName _InvalidComponentName) : _tag(InvalidComponentName), _InvalidComponentName(_InvalidComponentName) {}

ModelBuilderError::ModelBuilderError(struct ModuleRootMustBeConcept _ModuleRootMustBeConcept) : _tag(ModuleRootMustBeConcept), _ModuleRootMustBeConcept(_ModuleRootMustBeConcept) {}


String ModelError::to_string(Page* rp) {
    auto r = Region();
    StringBuilder& message_builder = *new (alignof(StringBuilder), r.get_page()) StringBuilder();
    {
        auto _result = *this;
        switch (_result._tag)
        {
            case ModelError::Io:
            {
                auto io = _result._Io;
                message_builder.append(io.to_string(rp));
                break;
            }
            case ModelError::Parser:
            {
                auto p = _result._Parser;
                message_builder.append(p.to_string(rp));
                break;
            }
            case ModelError::Builder:
            {
                auto b = _result._Builder;
                message_builder.append(b.to_string(rp));
                break;
            }
        }
    };
    return message_builder.to_string(rp);
}
ModelError::ModelError(struct IoModelError _Io) : _tag(Io), _Io(_Io) {}

ModelError::ModelError(struct ParserModelError _Parser) : _tag(Parser), _Parser(_Parser) {}

ModelError::ModelError(struct ModelBuilderError _Builder) : _tag(Builder), _Builder(_Builder) {}


}
}