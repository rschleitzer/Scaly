#include "../../scaly.h"
namespace scaly {
namespace compiler {
using namespace scaly::io;


Position::Position(size_t line, size_t column) : line(line), column(column) {}

Position calculate_position_from_string(Page* rp, String text, size_t offset) {
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

}
}