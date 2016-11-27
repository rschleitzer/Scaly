#ifndef __scalyc__ParserError__
#define __scalyc__ParserError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class ParserError;

class _ParserError_identifierExpected : public Object {
public:
    _ParserError_identifierExpected(Position* position);

    Position* position;
};

class _ParserError_literalExpected : public Object {
public:
    _ParserError_literalExpected(Position* position);

    Position* position;
};

class _ParserError_keywordExpected : public Object {
public:
    _ParserError_keywordExpected(Position* position, string* keyword);

    Position* position;
    string* keyword;
};

class _ParserError_punctuationExpected : public Object {
public:
    _ParserError_punctuationExpected(Position* position, string* punctuation);

    Position* position;
    string* punctuation;
};

class _ParserError_operatorExpected : public Object {
public:
    _ParserError_operatorExpected(Position* position);

    Position* position;
};

class _ParserError_unableToParse : public Object {
public:
    _ParserError_unableToParse(Position* position, _Array<ParserError>* errors);

    Position* position;
    _Array<ParserError>* errors;
};

class _ParserError_notAtEnd : public Object {
public:
    _ParserError_notAtEnd(Position* position);

    Position* position;
};
enum _ParserErrorCode {
    _ParserErrorCode_identifierExpected = 1,
    _ParserErrorCode_literalExpected,
    _ParserErrorCode_keywordExpected,
    _ParserErrorCode_punctuationExpected,
    _ParserErrorCode_operatorExpected,
    _ParserErrorCode_unableToParse,
    _ParserErrorCode_notAtEnd,
};

class ParserError : public Object {
public:
    ParserError(_ParserErrorCode errorCode)
    : errorCode(errorCode), errorInfo(0) {}

    ParserError(_ParserError_identifierExpected* identifierExpected)
    : errorCode(_ParserErrorCode_identifierExpected), errorInfo(identifierExpected) {}

    ParserError(_ParserError_literalExpected* literalExpected)
    : errorCode(_ParserErrorCode_literalExpected), errorInfo(literalExpected) {}

    ParserError(_ParserError_keywordExpected* keywordExpected)
    : errorCode(_ParserErrorCode_keywordExpected), errorInfo(keywordExpected) {}

    ParserError(_ParserError_punctuationExpected* punctuationExpected)
    : errorCode(_ParserErrorCode_punctuationExpected), errorInfo(punctuationExpected) {}

    ParserError(_ParserError_operatorExpected* operatorExpected)
    : errorCode(_ParserErrorCode_operatorExpected), errorInfo(operatorExpected) {}

    ParserError(_ParserError_unableToParse* unableToParse)
    : errorCode(_ParserErrorCode_unableToParse), errorInfo(unableToParse) {}

    ParserError(_ParserError_notAtEnd* notAtEnd)
    : errorCode(_ParserErrorCode_notAtEnd), errorInfo(notAtEnd) {}

    long _getErrorCode();
    void* _getErrorInfo();

    _ParserError_identifierExpected* get_identifierExpected();
    _ParserError_literalExpected* get_literalExpected();
    _ParserError_keywordExpected* get_keywordExpected();
    _ParserError_punctuationExpected* get_punctuationExpected();
    _ParserError_operatorExpected* get_operatorExpected();
    _ParserError_unableToParse* get_unableToParse();
    _ParserError_notAtEnd* get_notAtEnd();

private:
    _ParserErrorCode errorCode;
    void* errorInfo;
};

}
#endif // __scalyc__ParserError__
