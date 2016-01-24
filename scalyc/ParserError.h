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
    identifierExpected = 1,
    literalExpected,
    keywordExpected,
    punctuationExpected,
    operatorExpected,
    unableToParse,
    notAtEnd,
};

class ParserError : public Object
{
public:
    ParserError(IdentifierExpected& IdentifierExpected)
    : errorCode(identifierExpected), errorInfo(&IdentifierExpected) {}

    ParserError(LiteralExpected& LiteralExpected)
    : errorCode(literalExpected), errorInfo(&LiteralExpected) {}

    ParserError(KeywordExpected& KeywordExpected)
    : errorCode(keywordExpected), errorInfo(&KeywordExpected) {}

    ParserError(PunctuationExpected& PunctuationExpected)
    : errorCode(punctuationExpected), errorInfo(&PunctuationExpected) {}

    ParserError(OperatorExpected& OperatorExpected)
    : errorCode(operatorExpected), errorInfo(&OperatorExpected) {}

    ParserError(UnableToParse& UnableToParse)
    : errorCode(unableToParse), errorInfo(&UnableToParse) {}

    ParserError(NotAtEnd& NotAtEnd)
    : errorCode(notAtEnd), errorInfo(&NotAtEnd) {}

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
