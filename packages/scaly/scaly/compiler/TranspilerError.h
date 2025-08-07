#ifndef _TranspilerError_h
#define _TranspilerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct FeatureNotImplemented : Object {
    String file;
    Span span;
    String feature;

    FeatureNotImplemented(String file, Span span, String feature);
    String to_string(Page* rp);
};

struct CantThrowInNonThrowingFunction : Object {
    String file;
    Span span;

    CantThrowInNonThrowingFunction(String file, Span span);
    String to_string(Page* rp);
};

struct TranspilerError : Object {
    TranspilerError(FeatureNotImplemented);
    TranspilerError(FileError);
    TranspilerError(CantThrowInNonThrowingFunction);
    enum {
        NotImplemented,
        FileError,
        CantThrowInNonThrowingFunction,
    } _tag;
    union {
        struct FeatureNotImplemented _NotImplemented;
        struct FileError _FileError;
        struct CantThrowInNonThrowingFunction _CantThrowInNonThrowingFunction;
    };
    String to_string(Page* rp);
};
#endif