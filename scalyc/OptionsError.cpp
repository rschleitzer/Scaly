#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long OptionsError::getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::getErrorInfo() {
    return errorInfo;
}

_invalidOption::_invalidOption(_LetString* option)
: option(option){
}

_invalidOption* OptionsError::get_invalidOption(){
    return (_invalidOption*)errorInfo;
}

_unknownOption::_unknownOption(_LetString* option)
: option(option){
}

_unknownOption* OptionsError::get_unknownOption(){
    return (_unknownOption*)errorInfo;
}

}