#ifndef _JsonErrors_h
#define _JsonErrors_h
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

struct ParserError : Object {
    ParserError(DifferentSyntax);
    ParserError(InvalidSyntax);
    enum {
        Different,
        Invalid,
    } _tag;
    union {
        struct DifferentSyntax _Different;
        struct InvalidSyntax _Invalid;
    };
};
#endif