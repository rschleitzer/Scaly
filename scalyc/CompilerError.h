#ifndef __scalyc_CompilerError__
#define __scalyc_CompilerError__
#include "scalyc.h"

namespace scalyc
{

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

enum _CompilerErrorCode {
    _CompilerError_fileNotFound = 1,
    _CompilerError_syntaxError,
    _CompilerError_unableToCreateOutputDirectory
};

class CompilerError : public Object
{
public:
    CompilerError(_unableToReadFile* unableToReadFile)
    : errorCode(_CompilerError_fileNotFound), errorInfo(unableToReadFile) {}

    CompilerError(_syntaxError* syntaxError)
    : errorCode(_CompilerError_syntaxError), errorInfo(syntaxError) {}

    CompilerError(_unableToCreateOutputDirectory* unableToCreateOutputDirectory)
    : errorCode(_CompilerError_unableToCreateOutputDirectory), errorInfo(unableToCreateOutputDirectory) {}

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
#endif//__scalyc_CompilerError__
