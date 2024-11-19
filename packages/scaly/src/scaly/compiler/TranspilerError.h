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

struct OnlyFile : Object {

};

struct TranspilerError : Object {
    TranspilerError(FeatureNotImplemented);
    TranspilerError(OnlyFile);
    TranspilerError(FileError);
    enum {
        NotImplemented,
        OnlyFile,
        FileError,
    } _tag;
    union {
        struct FeatureNotImplemented _NotImplemented;
        struct OnlyFile _OnlyFile;
        struct FileError _FileError;
    };
    String to_string(Page* rp);
};
#endif