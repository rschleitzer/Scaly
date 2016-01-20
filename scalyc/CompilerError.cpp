#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CompilerError::getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::getErrorInfo() {
    return errorInfo;
}

UnableToReadFile::UnableToReadFile(String& file, FileError& fileError)
: file(file), fileError(fileError) {
}

UnableToReadFile& CompilerError::getFileNotFound(){
    return *(UnableToReadFile*)errorInfo;
}

SyntaxError::SyntaxError(ParserError& parserError)
: parserError(parserError) {
}

}