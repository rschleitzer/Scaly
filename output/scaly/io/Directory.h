#ifndef _Directory_h
#define _Directory_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Directory : Object {
    static Result<Void, FileError> remove(Page* ep, String path);
};

#endif