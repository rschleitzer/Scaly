#ifndef _Directory_h
#define _Directory_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Directory : Object {

    static Result<bool, FileError> exists(Page* ep, String path);
    static Result<Void, FileError> create(Page* ep, String path);
    static Result<Void, FileError> remove(Page* ep, String path);
};

#endif