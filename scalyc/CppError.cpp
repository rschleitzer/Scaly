#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CppError::getErrorCode() {
    return (long)errorCode; }

void* CppError::getErrorInfo() {
    return errorInfo; }

_unableToCreateOutputDirectory::_unableToCreateOutputDirectory(_LetString* directory, DirectoryError* error)
: directory(directory), error(error) { }

_unableToCreateOutputDirectory* CppError::get_unableToCreateOutputDirectory() {
    return (_unableToCreateOutputDirectory*)errorInfo; }

}