#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CppError::getErrorCode() {
    return (long)errorCode; }

void* CppError::getErrorInfo() {
    return errorInfo; }

UnableToCreateOutputDirectory::UnableToCreateOutputDirectory(_LetString* directory, DirectoryError* error)
: directory(directory), error(error) { }

UnableToCreateOutputDirectory* CppError::getUnableToCreateOutputDirectory() {
    return (UnableToCreateOutputDirectory*)errorInfo; }

}