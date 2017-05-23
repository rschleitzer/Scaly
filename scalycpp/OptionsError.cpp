#include "scalycpp.h"
using namespace scaly;
namespace scalyc {

long OptionsError::_getErrorCode() {
    return (long)errorCode;
}

void* OptionsError::_getErrorInfo() {
    return errorInfo;
}

_OptionsError_invalidOption::_OptionsError_invalidOption(string* option) 
: option(option) { }

_OptionsError_invalidOption* OptionsError::get_invalidOption() {
    return (_OptionsError_invalidOption*)errorInfo;
}

_OptionsError_unknownOption::_OptionsError_unknownOption(string* option) 
: option(option) { }

_OptionsError_unknownOption* OptionsError::get_unknownOption() {
    return (_OptionsError_unknownOption*)errorInfo;
}

long ParserError::_getErrorCode() {
    return (long)errorCode;
}

void* ParserError::_getErrorInfo() {
    return errorInfo;
}

_ParserError_syntax::_ParserError_syntax(size_t line, size_t column) 
: line(line), column(column) { }

_ParserError_syntax* ParserError::get_syntax() {
    return (_ParserError_syntax*)errorInfo;
}

long CompilerError::_getErrorCode() {
    return (long)errorCode;
}

void* CompilerError::_getErrorInfo() {
    return errorInfo;
}

_CompilerError_parser::_CompilerError_parser(size_t line, size_t column) 
: line(line), column(column) { }

_CompilerError_parser* CompilerError::get_parser() {
    return (_CompilerError_parser*)errorInfo;
}


}
