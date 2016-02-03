#ifndef __scalyc_ParserError__
#define __scalyc_ParserError__
#include "scalyc.h"

namespace scalyc
{

class ParserErrorInfo : public Object {
public:
    ParserErrorInfo(Position& position);
    Position& position;
};

class IdentifierExpected : public ParserErrorInfo {
public:
    IdentifierExpected(Position& position);
};

class LiteralExpected : public ParserErrorInfo {
public:
    LiteralExpected(Position& position);
};

class KeywordExpected: public ParserErrorInfo {
public:
    KeywordExpected(Position& position, String& keyword);
    String& keyword; 
};

class PunctuationExpected: public ParserErrorInfo {
public:
    PunctuationExpected(Position& position, String& punctuation);
    String& punctuation; 
};

class OperatorExpected : public ParserErrorInfo {
public:
    OperatorExpected(Position& position);
};

class ParserError;
class UnableToParse: public ParserErrorInfo {
public:
    UnableToParse(Position& position, Array<ParserError>& errors);
    Array<ParserError>& errors; 
};

class NotAtEnd : public ParserErrorInfo {
public:
    NotAtEnd(Position& position);
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
    ParserError(IdentifierExpected& IdentifierExpected)
    : errorCode(_ParserError_identifierExpected), errorInfo(&IdentifierExpected) {}

    ParserError(LiteralExpected& LiteralExpected)
    : errorCode(_ParserError_literalExpected), errorInfo(&LiteralExpected) {}

    ParserError(KeywordExpected& KeywordExpected)
    : errorCode(_ParserError_keywordExpected), errorInfo(&KeywordExpected) {}

    ParserError(PunctuationExpected& PunctuationExpected)
    : errorCode(_ParserError_punctuationExpected), errorInfo(&PunctuationExpected) {}

    ParserError(OperatorExpected& OperatorExpected)
    : errorCode(_ParserError_operatorExpected), errorInfo(&OperatorExpected) {}

    ParserError(UnableToParse& UnableToParse)
    : errorCode(_ParserError_unableToParse), errorInfo(&UnableToParse) {}

    ParserError(NotAtEnd& NotAtEnd)
    : errorCode(_ParserError_notAtEnd), errorInfo(&NotAtEnd) {}

    long getErrorCode();
    void* getErrorInfo();
    
    IdentifierExpected& getIdentifierExpected();
    LiteralExpected& getLiteralExpected();
    KeywordExpected& getKeywordExpected();
    PunctuationExpected& getPunctuationExpected();
    OperatorExpected& getOperatorExpected();
    UnableToParse& getUnableToParse();
    NotAtEnd& getNotAtEnd();

private:
    _ParserErrorCode errorCode;
    void* errorInfo;
};

}
#endif//__scalyc_ParserError__
