#ifndef __scalyc_CompilerError__
#define __scalyc_CompilerError__
#include "scalyc.h"

namespace scalyc
{

class UnableToReadFile : public Object {
public:
    UnableToReadFile(_LetString* file, FileError* fileError);

    _LetString* file;
    FileError* fileError;
};

class SyntaxError : public Object {
public:
    SyntaxError(ParserError* parserError);
    ParserError* parserError;
};

enum _CompilerErrorCode {
    _CompilerError_fileNotFound = 1,
    _CompilerError_syntaxError,
    _CompilerError_unableToCreateOutputDirectory
};

class CompilerError : public Object
{
public:
    CompilerError(UnableToReadFile* UnableToReadFile)
    : errorCode(_CompilerError_fileNotFound), errorInfo(UnableToReadFile) {}

    CompilerError(SyntaxError* SyntaxError)
    : errorCode(_CompilerError_syntaxError), errorInfo(SyntaxError) {}

    CompilerError(UnableToCreateOutputDirectory* UnableToCreateOutputDirectory)
    : errorCode(_CompilerError_unableToCreateOutputDirectory), errorInfo(UnableToCreateOutputDirectory) {}

    long getErrorCode();
    void* getErrorInfo();

    UnableToReadFile* getUnableToReadFile();
    SyntaxError* getSyntaxError();
    UnableToCreateOutputDirectory* getUnableToCreateOutputDirectory();

private:
    _CompilerErrorCode errorCode;
    void* errorInfo;
};

}
#endif//__scalyc_CompilerError__
