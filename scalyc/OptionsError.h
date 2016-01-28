#ifndef __scalyc_OptionsError__
#define __scalyc_OptionsError__
#include "scalyc.h"

namespace scalyc
{

class _InvalidOption : public Object {
public:
    _InvalidOption(String& option);

    String& option;
};

enum _OptionsErrorCode {
    invalidOption = 1,
    noOutputOption,
    noFilesToCompile,
};

class OptionsError : public Object
{
public:
    OptionsError(_InvalidOption& _InvalidOption)
    : errorCode(invalidOption), errorInfo(&_InvalidOption) {}

    OptionsError(_OptionsErrorCode code)
    : errorCode(code), errorInfo(0) {}

    long getErrorCode();
    void* getErrorInfo();
    
    _InvalidOption& getInvalidOption();

private:
    _OptionsErrorCode errorCode;
    void* errorInfo;
};

}
#endif//__scalyc_OptionsError__
