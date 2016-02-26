#ifndef __scalyc_OptionsError__
#define __scalyc_OptionsError__
#include "scalyc.h"

namespace scalyc
{

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
    OptionsError(_invalidOption* invalidOption)
    : errorCode(_OptionsError_invalidOption), errorInfo(invalidOption) {}

    _invalidOption* get_invalidOption();

    OptionsError(_unknownOption* unknownOption)
    : errorCode(_OptionsError_unknownOption), errorInfo(unknownOption) {}

    _unknownOption* get_unknownOption();

    OptionsError(_OptionsErrorCode code)
    : errorCode(code), errorInfo(0) {}

    long getErrorCode();
    void* getErrorInfo();


private:
    _OptionsErrorCode errorCode;
    void* errorInfo;
};

}
#endif//__scalyc_OptionsError__
