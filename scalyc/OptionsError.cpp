#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long OptionsError::getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::getErrorInfo() {
    return errorInfo;
}

_OptionsError_invalidOption::_OptionsError_invalidOption(_LetString* option)
: option(option){
}

_OptionsError_invalidOption* OptionsError::get_invalidOption(){
    return (_OptionsError_invalidOption*)errorInfo;
}

_OptionsError_unknownOption::_OptionsError_unknownOption(_LetString* option)
: option(option){
}

_OptionsError_unknownOption* OptionsError::get_unknownOption(){
    return (_OptionsError_unknownOption*)errorInfo;
}

}