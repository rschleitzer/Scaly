#include "scalyc.h"
using namespace scaly;
namespace scalyc {

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
