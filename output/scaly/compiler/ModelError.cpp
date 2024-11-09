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

String to_string(Page* rp, size_t number) {
    auto str = "                      ";
    snprintf((char*)str, 22, "%zd", number);
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
                    builder.append(to_string(r.get_page(), position_start.line));
                    builder.append(':');
                    builder.append(to_string(r.get_page(), position_start.column));
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
IoModelError::IoModelError(struct FileError _File) : _tag(File), _File(_File) {}


}
}