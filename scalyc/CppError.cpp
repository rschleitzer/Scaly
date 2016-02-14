#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long CppError::getErrorCode() {
    return (long)errorCode;
}

void* CppError::getErrorInfo() {
    return errorInfo;
}

UnableToCreateOutputDirectory::UnableToCreateOutputDirectory(_VarString& outputDirectory, FileError& fileError)
: outputDirectory(outputDirectory), fileError(fileError) {
}

UnableToCreateOutputDirectory& CppError::getUnableToCreateOutputDirectory(){
    return *(UnableToCreateOutputDirectory*)errorInfo;
}

}