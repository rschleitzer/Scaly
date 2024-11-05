#ifndef _ModelError_h
#define _ModelError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct Position : Object {
    size_t line;
    size_t column;

    Position(size_t line, size_t column);
};

Position calculate_position_from_string(Page* rp, String text, size_t offset);
#endif