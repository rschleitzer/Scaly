#ifndef _PlannerError_h
#define _PlannerError_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::io;

struct DuplicateIntrinsic : Object {
    String name;

    DuplicateIntrinsic(String name);
};

struct PlannerError : Object {
    PlannerError(FeatureNotImplemented);
    PlannerError(FileError);
    PlannerError(DuplicateIntrinsic);
    enum {
        NotImplemented,
        FileError,
        DuplicateIntrinsic,
    } _tag;
    union {
        struct FeatureNotImplemented _NotImplemented;
        struct FileError _FileError;
        struct DuplicateIntrinsic _DuplicateIntrinsic;
    };
    String to_string(Page* rp);
};
#endif