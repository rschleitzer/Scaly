#ifndef __scalyc_OptionsError__
#define __scalyc_OptionsError__
#include "scalyc.h"

namespace scalyc
{

class _InvalidOption : public Object {
public:
    _InvalidOption(_LetString* option);

    _LetString* option;
};

class _UnknownOption : public Object {
public:
    _UnknownOption(_LetString* option);

    _LetString* option;
};

enum _OptionsErrorCode {
    invalidOption = 1,
    unknownOption,
    noOutputOption,
    noFilesToCompile,
};

class OptionsError : public Object
{
public:
    OptionsError(_InvalidOption* _InvalidOption)
    : errorCode(invalidOption), errorInfo(_InvalidOption) {}

    _InvalidOption* getInvalidOption();

    OptionsError(_UnknownOption* _UnknownOption)
    : errorCode(unknownOption), errorInfo(_UnknownOption) {}

    _UnknownOption* getUnknownOption();

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
