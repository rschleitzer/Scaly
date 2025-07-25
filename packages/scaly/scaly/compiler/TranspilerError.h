#ifndef _TranspilerError_h
#define _TranspilerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct FeatureNotImplemented : Object {
    String expected;

    FeatureNotImplemented(String expected);
};

struct TranspilerError : Object {
    TranspilerError(FeatureNotImplemented);
    TranspilerError(FileError);
    enum {
        NotImplemented,
        FileError,
    } _tag;
    union {
        struct FeatureNotImplemented _NotImplemented;
        struct FileError _FileError;
    };
    String to_string(Page* rp);
};
#endif