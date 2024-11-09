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

Position calculate_position_from_string(String text, size_t offset);
String build_hint_lines_from_string(Page* rp, String text, size_t start_offset, size_t end_offset, Position start_position, Position end_position);
Result<String, FileError> build_hint_lines(Page* rp, Page* ep, String file, size_t start_offset, size_t end_offset, Position start_position, Position end_position);
Result<Position, FileError> calculate_position(Page* rp, Page* ep, String file, size_t offset);
String to_string(Page* rp, size_t number);
void append_error_message_header(StringBuilder& builder, String file, size_t offset);
void append_hint_lines(StringBuilder& builder, String file, size_t start, size_t end);
struct IoModelError : Object {
    IoModelError(FileError);
    enum {
        File,
    } _tag;
    union {
        struct FileError _File;
    };
    String to_string(Page* rp);
};
#endif