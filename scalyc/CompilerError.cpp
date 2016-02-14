#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CompilerError::getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::getErrorInfo() {
    return errorInfo;
}

UnableToReadFile::UnableToReadFile(_VarString& file, FileError& fileError)
: file(file), fileError(fileError) {
}

UnableToReadFile& CompilerError::getUnableToReadFile(){
    return *(UnableToReadFile*)errorInfo;
}

SyntaxError::SyntaxError(ParserError& parserError)
: parserError(parserError) {
}

SyntaxError& CompilerError::getSyntaxError(){
    return *(SyntaxError*)errorInfo;
}

}