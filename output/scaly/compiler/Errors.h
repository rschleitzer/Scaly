#ifndef _Errors_h
#define _Errors_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct DifferentSyntax : Object {

};

struct InvalidSyntax : Object {
    size_t start;
    size_t end;
    String expected;

    InvalidSyntax(size_t start, size_t end, String expected);
};

struct ParserError {
    ParserError(DifferentSyntax);
    ParserError(InvalidSyntax);
    enum {
        Different,
        Invalid,
    } _tag;
    union {
        DifferentSyntax _Different;
        InvalidSyntax _Invalid;
    };
};
#endif