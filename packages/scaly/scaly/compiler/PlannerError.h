#ifndef _PlannerError_h
#define _PlannerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct PlannerError : Object {
    PlannerError(FeatureNotImplemented);
    PlannerError(FileError);
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