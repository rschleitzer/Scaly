#ifndef __scalyc__OptionsError__
#define __scalyc__OptionsError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class OptionsError;

class _OptionsError_invalidOption : public Object {
public:
    _OptionsError_invalidOption(String* option);

    String* option;
};

class _OptionsError_unknownOption : public Object {
public:
    _OptionsError_unknownOption(String* option);

    String* option;
};
enum _OptionsErrorCode {
    _OptionsErrorCode_invalidOption = 1,
    _OptionsErrorCode_unknownOption,
    _OptionsErrorCode_noOutputOption,
    _OptionsErrorCode_noFilesToCompile,
};

class OptionsError : public Object
{
public:
    OptionsError(_OptionsErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    OptionsError(_OptionsError_invalidOption* invalidOption)
    : errorCode(_OptionsErrorCode_invalidOption), errorInfo(invalidOption) {}

    OptionsError(_OptionsError_unknownOption* unknownOption)
    : errorCode(_OptionsErrorCode_unknownOption), errorInfo(unknownOption) {}

    long getErrorCode();
    void* getErrorInfo();

    _OptionsError_invalidOption* get_invalidOption();
    _OptionsError_unknownOption* get_unknownOption();

private:
    _OptionsErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__OptionsError__
