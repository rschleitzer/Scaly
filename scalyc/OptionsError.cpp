#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long OptionsError::getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::getErrorInfo() {
    return errorInfo;
}

_InvalidOption::_InvalidOption(_LetString* option)
: option(option){
}

_InvalidOption* OptionsError::getInvalidOption(){
    return (_InvalidOption*)errorInfo;
}

_UnknownOption::_UnknownOption(_LetString* option)
: option(option){
}

_UnknownOption* OptionsError::getUnknownOption(){
    return (_UnknownOption*)errorInfo;
}

}