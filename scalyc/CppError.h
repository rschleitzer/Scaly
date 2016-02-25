#ifndef __scalyc__CppError__
#define __scalyc__CppError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class UnableToCreateOutputDirectory : public Object {
public:
    UnableToCreateOutputDirectory(_VarString& outputDirectory, FileError& fileError);

    _VarString& outputDirectory;
    FileError& fileError;
};

enum _CppErrorCode {
    _CppError_unableToCreateOutputDirectory = 1,
};

class CppError : public Object
{
public:
    CppError(UnableToCreateOutputDirectory& UnableToCreateOutputDirectory)
    : errorCode(_CppError_unableToCreateOutputDirectory), errorInfo(&UnableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    UnableToCreateOutputDirectory& getUnableToCreateOutputDirectory();

private:
    _CppErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CppError__
