#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CompilerError::getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::getErrorInfo() {
    return errorInfo;
}

UnableToReadFile::UnableToReadFile(_LetString* file, FileError* error)
: file(file), error(error) {
}

UnableToReadFile* CompilerError::getUnableToReadFile(){
    return (UnableToReadFile*)errorInfo;
}

SyntaxError::SyntaxError(ParserError* error)
: error(error) {
}

SyntaxError* CompilerError::getSyntaxError(){
    return (SyntaxError*)errorInfo;
}

}