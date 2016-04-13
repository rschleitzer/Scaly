#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CppError::getErrorCode() {
    return (long)errorCode; }

void* CppError::getErrorInfo() {
    return errorInfo; }

_CppError_unableToCreateOutputDirectory::_CppError_unableToCreateOutputDirectory(String* directory, DirectoryError* error)
: directory(directory), error(error) { }

_CppError_unableToCreateOutputDirectory* CppError::get_unableToCreateOutputDirectory() {
    return (_CppError_unableToCreateOutputDirectory*)errorInfo; }

}
