#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CompilerError::_getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::_getErrorInfo() {
    return errorInfo;
}

_CompilerError_fileNotFound::_CompilerError_fileNotFound(string* file) 
: file(file) { }

_CompilerError_fileNotFound* CompilerError::get_fileNotFound() {
    return (_CompilerError_fileNotFound*)errorInfo;
}

_CompilerError_unableToReadFile::_CompilerError_unableToReadFile(string* file, FileError* error) 
: file(file), error(error) { }

_CompilerError_unableToReadFile* CompilerError::get_unableToReadFile() {
    return (_CompilerError_unableToReadFile*)errorInfo;
}

_CompilerError_syntaxError::_CompilerError_syntaxError(ParserError* error) 
: error(error) { }

_CompilerError_syntaxError* CompilerError::get_syntaxError() {
    return (_CompilerError_syntaxError*)errorInfo;
}

_CompilerError_unableToCreateOutputDirectory::_CompilerError_unableToCreateOutputDirectory(string* directory, DirectoryError* error) 
: directory(directory), error(error) { }

_CompilerError_unableToCreateOutputDirectory* CompilerError::get_unableToCreateOutputDirectory() {
    return (_CompilerError_unableToCreateOutputDirectory*)errorInfo;
}

long CppError::_getErrorCode() {
    return (long)errorCode;
}

void* CppError::_getErrorInfo() {
    return errorInfo;
}

_CppError_unableToCreateOutputDirectory::_CppError_unableToCreateOutputDirectory(string* directory, DirectoryError* error) 
: directory(directory), error(error) { }

_CppError_unableToCreateOutputDirectory* CppError::get_unableToCreateOutputDirectory() {
    return (_CppError_unableToCreateOutputDirectory*)errorInfo;
}


}
