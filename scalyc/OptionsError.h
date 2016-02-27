#ifndef __scalyc__OptionsError__
#define __scalyc__OptionsError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class OptionsError;

class _invalidOption : public Object {
public:
    _invalidOption(_LetString* option);

    _LetString* option;
};



class _unknownOption : public Object {
public:
    _unknownOption(_LetString* option);

    _LetString* option;
};

enum _OptionsErrorCode {
    _OptionsError_invalidOption = 1,
    _OptionsError_unknownOption,
    _OptionsError_noOutputOption,
    _OptionsError_noFilesToCompile,
};

class OptionsError : public Object
{
public:
    OptionsError(_OptionsErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    OptionsError(_invalidOption* invalidOption)
    : errorCode(_OptionsError_invalidOption), errorInfo(invalidOption) {}

    OptionsError(_unknownOption* unknownOption)
    : errorCode(_OptionsError_unknownOption), errorInfo(unknownOption) {}

    long getErrorCode();
    void* getErrorInfo();

    _invalidOption* get_invalidOption();
    _unknownOption* get_unknownOption();

private:
    _OptionsErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__OptionsError__
