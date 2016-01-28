#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long OptionsError::getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::getErrorInfo() {
    return errorInfo;
}

_InvalidOption::_InvalidOption(String& option)
: option(option){
}

_InvalidOption& OptionsError::getInvalidOption(){
    return *(_InvalidOption*)errorInfo;
}

}