#ifndef _Result_h
#define _Result_h
#include "../scaly.h"
using namespace scaly::memory;

struct Void {};

template<class OK, class ERROR>
struct Result {
    enum {
        Ok,
        Error,
} _tag;
    union {
        OK _Ok;
        ERROR _Error;
    };
};
#endif