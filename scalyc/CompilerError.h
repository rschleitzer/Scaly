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



class _unableToCreateOutputDir : public Object {
public:
    _unableToCreateOutputDir(_LetString* directory, DirectoryError* error);

    _LetString* directory;
    DirectoryError* error;
};

enum _CompilerErrorCode {
    _CompilerError_unableToReadFile = 1,
    _CompilerError_syntaxError,
    _CompilerError_unableToCreateOutputDir,
};

class CompilerError : public Object
{
public:
    CompilerError(_CompilerErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    CompilerError(_unableToReadFile* unableToReadFile)
    : errorCode(_CompilerError_unableToReadFile), errorInfo(unableToReadFile) {}

    CompilerError(_syntaxError* syntaxError)
    : errorCode(_CompilerError_syntaxError), errorInfo(syntaxError) {}

    CompilerError(_unableToCreateOutputDir* unableToCreateOutputDir)
    : errorCode(_CompilerError_unableToCreateOutputDir), errorInfo(unableToCreateOutputDir) {}

    long getErrorCode();
    void* getErrorInfo();

    _unableToReadFile* get_unableToReadFile();
    _syntaxError* get_syntaxError();
    _unableToCreateOutputDir* get_unableToCreateOutputDir();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__CompilerError__
