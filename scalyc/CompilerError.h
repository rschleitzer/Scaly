#ifndef __scalyc__CompilerError__
#define __scalyc__CompilerError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CompilerError;

class _CompilerError_unableToReadFile : public Object {
public:
    _CompilerError_unableToReadFile(String* file, FileError* error);

    String* file;
    FileError* error;
};

class _CompilerError_syntaxError : public Object {
public:
    _CompilerError_syntaxError(ParserError* error);

    ParserError* error;
};

class _CompilerError_unableToCreateOutputDirectory : public Object {
public:
    _CompilerError_unableToCreateOutputDirectory(String* directory, DirectoryError* error);

    String* directory;
    DirectoryError* error;
};
enum _CompilerErrorCode {
    _CompilerErrorCode_unableToReadFile = 1,
    _CompilerErrorCode_syntaxError,
    _CompilerErrorCode_unableToCreateOutputDirectory,
};

class CompilerError : public Object {
public:
    CompilerError(_CompilerErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CompilerError(_CompilerError_unableToReadFile* unableToReadFile)
    : errorCode(_CompilerErrorCode_unableToReadFile), errorInfo(unableToReadFile) {}

    CompilerError(_CompilerError_syntaxError* syntaxError)
    : errorCode(_CompilerErrorCode_syntaxError), errorInfo(syntaxError) {}

    CompilerError(_CompilerError_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CompilerErrorCode_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    _CompilerError_unableToReadFile* get_unableToReadFile();
    _CompilerError_syntaxError* get_syntaxError();
    _CompilerError_unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CompilerError__
