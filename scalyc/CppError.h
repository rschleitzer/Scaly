#ifndef __scalyc__CppError__
#define __scalyc__CppError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CppError;

class _CppError_unableToCreateOutputDirectory : public Object {
public:
    _CppError_unableToCreateOutputDirectory(_LetString* directory, DirectoryError* error);

    _LetString* directory;
    DirectoryError* error;
};
enum _CppErrorCode {
    _CppErrorCode_unableToCreateOutputDirectory = 1,
};

class CppError : public Object
{
public:
    CppError(_CppErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CppError(_CppError_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CppErrorCode_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    _CppError_unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CppErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CppError__
