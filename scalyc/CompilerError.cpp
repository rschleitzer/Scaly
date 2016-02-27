#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CompilerError::getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::getErrorInfo() {
    return errorInfo;
}

_unableToReadFile::_unableToReadFile(_LetString* file, FileError* error)
: file(file), error(error) {
}

_CompilerError_unableToCreateOutputDirectory::_CompilerError_unableToCreateOutputDirectory(_LetString* directory, DirectoryError* error)
: directory(directory), error(error) {
}

_unableToReadFile* CompilerError::get_unableToReadFile(){
    return (_unableToReadFile*)errorInfo;
}

_syntaxError::_syntaxError(ParserError* error)
: error(error) {
}

_syntaxError* CompilerError::get_syntaxError(){
    return (_syntaxError*)errorInfo;
}

}