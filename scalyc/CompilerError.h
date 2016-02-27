#ifndef __scalyc__CompilerError__
#define __scalyc__CompilerError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class CompilerError;

class _unableToReadFile : public Object {
public:
    _unableToReadFile(_LetString* file, FileError* error);

    _LetString* file;
    FileError* error;
};



class _syntaxError : public Object {
public:
    _syntaxError(ParserError* error);

    ParserError* error;
};



class _CompilerError_unableToCreateOutputDirectory : public Object {
public:
    _CompilerError_unableToCreateOutputDirectory(_LetString* directory, DirectoryError* error);

    _LetString* directory;
    DirectoryError* error;
};

enum _CompilerErrorCode {
    _CompilerErrorCode_unableToReadFile = 1,
    _CompilerErrorCode_syntaxError,
    _CompilerErrorCode_unableToCreateOutputDirectory,
};

class CompilerError : public Object
{
public:
    CompilerError(_CompilerErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CompilerError(_unableToReadFile* unableToReadFile)
    : errorCode(_CompilerErrorCode_unableToReadFile), errorInfo(unableToReadFile) {}

    CompilerError(_syntaxError* syntaxError)
    : errorCode(_CompilerErrorCode_syntaxError), errorInfo(syntaxError) {}

    CompilerError(_CompilerError_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CompilerErrorCode_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    _unableToReadFile* get_unableToReadFile();
    _syntaxError* get_syntaxError();
    _unableToCreateOutputDirectory* get_unableToCreateOutputDirectory();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CompilerError__
