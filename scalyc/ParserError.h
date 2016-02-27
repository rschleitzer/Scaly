#ifndef __scalyc__ParserError__
#define __scalyc__ParserError__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class ParserError;

class _identifierExpected : public Object {
public:
    _identifierExpected(Position* position);

    Position* position;
};



class _literalExpected : public Object {
public:
    _literalExpected(Position* position);

    Position* position;
};



class _keywordExpected : public Object {
public:
    _keywordExpected(Position* position, _LetString* keyword);

    Position* position;
    _LetString* keyword;
};



class _punctuationExpected : public Object {
public:
    _punctuationExpected(Position* position, _LetString* punctuation);

    Position* position;
    _LetString* punctuation;
};



class _operatorExpected : public Object {
public:
    _operatorExpected(Position* position);

    Position* position;
};



class _unableToParse : public Object {
public:
    _unableToParse(Position* position, _Vector<ParserError>* errors);

    Position* position;
    _Vector<ParserError>* errors;
};



class _notAtEnd : public Object {
public:
    _notAtEnd(Position* position);

    Position* position;
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
#endif // __scalyc__ParserError__
