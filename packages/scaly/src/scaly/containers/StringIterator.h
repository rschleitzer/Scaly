#ifndef _StringIterator_h
#define _StringIterator_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct StringIterator : Object {
    char* current;
    char* last;

    StringIterator(char* current, char* last);
    StringIterator(String string);
    char* next();
};

#endif