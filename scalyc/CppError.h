#ifndef __scalyc__CppError__
#define __scalyc__CppError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class _unableToCreateOutputDirectory : public Object {
public:
    _unableToCreateOutputDirectory(_LetString* directory, DirectoryError* error);

    _LetString* directory;
    DirectoryError* error;
};

enum _CppErrorCode {
    _CppError_unableToCreateOutputDirectory = 1,
};

class CppError : public Object
{
public:
    CppError(_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CppError_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    _unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CppErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CppError__
