#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long ParserError::getErrorCode() {
    return (long)errorCode;
}

void* ParserError::getErrorInfo() {
    return errorInfo;
}

_ParserError_identifierExpected::_ParserError_identifierExpected(Position* position) 
: position(position) { }

_ParserError_identifierExpected* ParserError::get_identifierExpected() {
    return (_ParserError_identifierExpected*)errorInfo;
}

_ParserError_literalExpected::_ParserError_literalExpected(Position* position) 
: position(position) { }

_ParserError_literalExpected* ParserError::get_literalExpected() {
    return (_ParserError_literalExpected*)errorInfo;
}

_ParserError_keywordExpected::_ParserError_keywordExpected(Position* position, string* keyword) 
: position(position), keyword(keyword) { }

_ParserError_keywordExpected* ParserError::get_keywordExpected() {
    return (_ParserError_keywordExpected*)errorInfo;
}

_ParserError_punctuationExpected::_ParserError_punctuationExpected(Position* position, string* punctuation) 
: position(position), punctuation(punctuation) { }

_ParserError_punctuationExpected* ParserError::get_punctuationExpected() {
    return (_ParserError_punctuationExpected*)errorInfo;
}

_ParserError_operatorExpected::_ParserError_operatorExpected(Position* position) 
: position(position) { }

_ParserError_operatorExpected* ParserError::get_operatorExpected() {
    return (_ParserError_operatorExpected*)errorInfo;
}

_ParserError_unableToParse::_ParserError_unableToParse(Position* position, _Vector<ParserError>* errors) 
: position(position), errors(errors) { }

_ParserError_unableToParse* ParserError::get_unableToParse() {
    return (_ParserError_unableToParse*)errorInfo;
}

_ParserError_notAtEnd::_ParserError_notAtEnd(Position* position) 
: position(position) { }

_ParserError_notAtEnd* ParserError::get_notAtEnd() {
    return (_ParserError_notAtEnd*)errorInfo;
}


}
