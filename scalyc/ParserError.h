#ifndef __scalyc_ParserError__
#define __scalyc_ParserError__
#include "scalyc.h"

namespace scalyc
{

class ParserErrorInfo : public Object {
public:
    ParserErrorInfo(Position* position);
    Position* position;
};

class _identifierExpected : public ParserErrorInfo {
public:
    _identifierExpected(Position* position);
};

class _literalExpected : public ParserErrorInfo {
public:
    _literalExpected(Position* position);
};

class _keywordExpected: public ParserErrorInfo {
public:
    _keywordExpected(Position* position, _LetString* keyword);
    _LetString* keyword;
};

class _punctuationExpected: public ParserErrorInfo {
public:
    _punctuationExpected(Position* position, _LetString* punctuation);
    _LetString* punctuation;
};

class _operatorExpected : public ParserErrorInfo {
public:
    _operatorExpected(Position* position);
};

class ParserError;
class _unableToParse: public ParserErrorInfo {
public:
    _unableToParse(Position* position, _Array<ParserError>* errors);
    _Array<ParserError>* errors;
};

class _notAtEnd : public ParserErrorInfo {
public:
    _notAtEnd(Position* position);
};

enum _ParserErrorCode {
    _ParserError_identifierExpected = 1,
    _ParserError_literalExpected,
    _ParserError_keywordExpected,
    _ParserError_punctuationExpected,
    _ParserError_operatorExpected,
    _ParserError_unableToParse,
    _ParserError_notAtEnd,
};

class ParserError : public Object
{
public:
    ParserError(_identifierExpected* identifierExpected)
    : errorCode(_ParserError_identifierExpected), errorInfo(identifierExpected) {}

    ParserError(_literalExpected* literalExpected)
    : errorCode(_ParserError_literalExpected), errorInfo(literalExpected) {}

    ParserError(_keywordExpected* keywordExpected)
    : errorCode(_ParserError_keywordExpected), errorInfo(keywordExpected) {}

    ParserError(_punctuationExpected* punctuationExpected)
    : errorCode(_ParserError_punctuationExpected), errorInfo(punctuationExpected) {}

    ParserError(_operatorExpected* operatorExpected)
    : errorCode(_ParserError_operatorExpected), errorInfo(operatorExpected) {}

    ParserError(_unableToParse* unableToParse)
    : errorCode(_ParserError_unableToParse), errorInfo(unableToParse) {}

    ParserError(_notAtEnd* notAtEnd)
    : errorCode(_ParserError_notAtEnd), errorInfo(notAtEnd) {}

    long getErrorCode();
    void* getErrorInfo();

    _identifierExpected* get_identifierExpected();
    _literalExpected* get_literalExpected();
    _keywordExpected* get_keywordExpected();
    _punctuationExpected* get_punctuationExpected();
    _operatorExpected* get_operatorExpected();
    _unableToParse* get_unableToParse();
    _notAtEnd* get_notAtEnd();

private:
    _ParserErrorCode errorCode;
    void* errorInfo;
};

}
#endif//__scalyc_ParserError__
