#include "scalyc.h"
using namespace scaly;
namespace scalyc {

long ParserError::getErrorCode() {
    return (long)errorCode;
}

void* ParserError::getErrorInfo() {
    return errorInfo;
}

_identifierExpected::_identifierExpected(Position* position)
: position(position) {
}

_identifierExpected* ParserError::get_identifierExpected(){
    return (_identifierExpected*)errorInfo;
}

_literalExpected::_literalExpected(Position* position)
: position(position) {
}

_literalExpected* ParserError::get_literalExpected(){
    return (_literalExpected*)errorInfo;
}

_keywordExpected::_keywordExpected(Position* position, _LetString* keyword)
: position(position), keyword(keyword) {
}

_keywordExpected* ParserError::get_keywordExpected(){
    return (_keywordExpected*)errorInfo;
}

_punctuationExpected::_punctuationExpected(Position* position, _LetString* punctuation)
: position(position), punctuation(punctuation) {
}

_punctuationExpected* ParserError::get_punctuationExpected(){
    return (_punctuationExpected*)errorInfo;
}

_operatorExpected::_operatorExpected(Position* position)
: position(position) {
}

_operatorExpected* ParserError::get_operatorExpected(){
    return (_operatorExpected*)errorInfo;
}

_unableToParse::_unableToParse(Position* position, _Vector<ParserError>* errors)
: position(position), errors(errors) {
}

_unableToParse* ParserError::get_unableToParse(){
    return (_unableToParse*)errorInfo;
}

_notAtEnd::_notAtEnd(Position* position)
: position(position) {
}

_notAtEnd* ParserError::get_notAtEnd(){
    return (_notAtEnd*)errorInfo;
}

}