#include "scalyc.h"
using namespace scaly;
namespace scalyc {
    
ParserErrorInfo::ParserErrorInfo(Position& position)
: position(position) {
}

long ParserError::getErrorCode() {
    return (long)errorCode;
}

void* ParserError::getErrorInfo() {
    return errorInfo;
}

IdentifierExpected::IdentifierExpected(Position& position)
: ParserErrorInfo(position) {
}

IdentifierExpected& ParserError::getIdentifierExpected(){
    return *(IdentifierExpected*)errorInfo;
}

LiteralExpected::LiteralExpected(Position& position)
: ParserErrorInfo(position) {
}

LiteralExpected& ParserError::getLiteralExpected(){
    return *(LiteralExpected*)errorInfo;
}

KeywordExpected::KeywordExpected(Position& position, String& keyword)
: ParserErrorInfo(position), keyword(keyword) {
}

KeywordExpected& ParserError::getShouldStartWithKeyword(){
    return *(KeywordExpected*)errorInfo;
}

PunctuationExpected::PunctuationExpected(Position& position, String& punctuation)
: ParserErrorInfo(position), punctuation(punctuation) {
}

PunctuationExpected& ParserError::getPunctuationExpected(){
    return *(PunctuationExpected*)errorInfo;
}

OperatorExpected::OperatorExpected(Position& position)
: ParserErrorInfo(position) {
}

OperatorExpected& ParserError::getOperatorExpected(){
    return *(OperatorExpected*)errorInfo;
}

UnableToParse::UnableToParse(Position& position, Array<ParserError>& errors)
: ParserErrorInfo(position), errors(errors) {
}

UnableToParse& ParserError::getUnableToParse(){
    return *(UnableToParse*)errorInfo;
}

NotAtEnd::NotAtEnd(Position& position)
: ParserErrorInfo(position) {
}

NotAtEnd& ParserError::getNotAtEnd(){
    return *(NotAtEnd*)errorInfo;
}

}