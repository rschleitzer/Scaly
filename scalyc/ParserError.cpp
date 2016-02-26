#include "scalyc.h"
using namespace scaly;
namespace scalyc {

ParserErrorInfo::ParserErrorInfo(Position* position)
: position(position) {
}

long ParserError::getErrorCode() {
    return (long)errorCode;
}

void* ParserError::getErrorInfo() {
    return errorInfo;
}

_identifierExpected::_identifierExpected(Position* position)
: ParserErrorInfo(position) {
}

_identifierExpected* ParserError::get_identifierExpected(){
    return (_identifierExpected*)errorInfo;
}

_literalExpected::_literalExpected(Position* position)
: ParserErrorInfo(position) {
}

_literalExpected* ParserError::get_literalExpected(){
    return (_literalExpected*)errorInfo;
}

_keywordExpected::_keywordExpected(Position* position, _LetString* keyword)
: ParserErrorInfo(position), keyword(keyword) {
}

_keywordExpected* ParserError::get_keywordExpected(){
    return (_keywordExpected*)errorInfo;
}

_punctuationExpected::_punctuationExpected(Position* position, _LetString* punctuation)
: ParserErrorInfo(position), punctuation(punctuation) {
}

_punctuationExpected* ParserError::get_punctuationExpected(){
    return (_punctuationExpected*)errorInfo;
}

_operatorExpected::_operatorExpected(Position* position)
: ParserErrorInfo(position) {
}

_operatorExpected* ParserError::get_operatorExpected(){
    return (_operatorExpected*)errorInfo;
}

_unableToParse::_unableToParse(Position* position, _Array<ParserError>*errors)
: ParserErrorInfo(position), errors(errors) {
}

_unableToParse* ParserError::get_unableToParse(){
    return (_unableToParse*)errorInfo;
}

_notAtEnd::_notAtEnd(Position* position)
: ParserErrorInfo(position) {
}

_notAtEnd* ParserError::get_notAtEnd(){
    return (_notAtEnd*)errorInfo;
}

}