#ifndef _File_h
#define _File_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct File : Object {

    static Result<String, FileError> read_to_string(Page* rp, Page* ep, String path);
    static Result<Void, FileError> write_from_string(Page* ep, String path, String contents);
};

#endif