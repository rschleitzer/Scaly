#ifndef _Result_h
#define _Result_h
#include "../scaly.h"
using namespace scaly;
using namespace scaly::memory;

struct Void {

};

enum Success {
    Ok,
    Error
};

template<class OK, class ERROR>
struct Result {
    Result(OK _OK) : _tag(Ok) { _Ok = _OK; }
    Result(ERROR _ERROR) : _tag(Error) { _Error = _ERROR; }
    Success _tag;
    union {
        OK _Ok;
        ERROR _Error;
    };
};
#endif