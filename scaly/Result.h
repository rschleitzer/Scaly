#ifndef __Scaly_Result__
#define __Scaly_Result__
#include "Scaly.h"
namespace scaly {

template<class R, class E> class _Result : public Object {
public:
    // The operation succeeded
    _Result(R* result)
    : errorCode(0), payload(result) {}
        
    // The operation failed
    _Result(E* error)
    : errorCode((long)(error->getErrorCode())), payload(error->getErrorInfo()) {}
    
    bool succeeded() {
        return errorCode == 0;
    }
    
    R* getResult() {
        return (R*)payload;
    }
    
    long getErrorCode() {
        return errorCode;
    }
    
    E* getError() {
        return (E*)this;
    }

private:
    long errorCode;
    void* payload;
};

}
#endif//__Scaly_Result__
