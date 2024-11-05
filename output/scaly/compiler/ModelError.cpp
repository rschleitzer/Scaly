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

}
}