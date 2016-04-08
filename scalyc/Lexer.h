#ifndef __scalyc__Lexer__
#define __scalyc__Lexer__
#include "scalyc.h"
using namespace scaly;
namespace scalyc {

class Position : public Object {
public:
    Position(size_t line, size_t column);
    size_t line;
    size_t column;

};

class Token : public Object {
public:

    virtual bool _isEofToken();
    virtual bool _isInvalidToken();
    virtual bool _isIdentifier();
    virtual bool _isLiteral();
    virtual bool _isStringLiteral();
    virtual bool _isCharacterLiteral();
    virtual bool _isNumericLiteral();
    virtual bool _isPunctuation();
    virtual bool _isOperator();
    virtual bool _isPrefixOperator();
    virtual bool _isBinaryOperator();
    virtual bool _isPostfixOperator();
};

class EofToken : public Token {
public:

    virtual bool _isEofToken();
};

class InvalidToken : public Token {
public:

    virtual bool _isInvalidToken();
};

class Identifier : public Token {
public:
    Identifier(String* name);
    String* name;

    virtual bool _isIdentifier();
};

class Literal : public Token {
public:

    virtual bool _isLiteral();
    virtual bool _isStringLiteral();
    virtual bool _isCharacterLiteral();
    virtual bool _isNumericLiteral();
};

class StringLiteral : public Literal {
public:
    StringLiteral(String* theString);
    String* string;

    virtual bool _isStringLiteral();
};

class CharacterLiteral : public Literal {
public:
    CharacterLiteral(String* theString);
    String* value;

    virtual bool _isCharacterLiteral();
};

class NumericLiteral : public Literal {
public:
    NumericLiteral(String* theValue);
    String* value;

    virtual bool _isNumericLiteral();
};

class Punctuation : public Token {
public:
    Punctuation(String* theSign);
    String* sign;

    virtual bool _isPunctuation();
};

class Operator : public Token {
public:
    String* operation;

    virtual bool _isOperator();
    virtual bool _isPrefixOperator();
    virtual bool _isBinaryOperator();
    virtual bool _isPostfixOperator();
};

class PrefixOperator : public Operator {
public:
    PrefixOperator(String* theOperation);

    virtual bool _isPrefixOperator();
};

class BinaryOperator : public Operator {
public:
    BinaryOperator(String* theOperation);

    virtual bool _isBinaryOperator();
};

class PostfixOperator : public Operator {
public:
    PostfixOperator(String* theOperation);

    virtual bool _isPostfixOperator();
};

class Lexer : public Object {
public:
    Token* token;
    bool whitespaceSkipped;
    String* text;
    size_t position;
    size_t end;
    size_t previousLine;
    size_t previousColumn;
    size_t line;
    size_t column;
    Lexer(String* theText);
    virtual void advance();
    virtual Identifier* scanIdentifier(_Page* _rp);
    virtual Operator* scanOperator(_Page* _rp, bool includeDots);
    virtual NumericLiteral* scanNumericLiteral(_Page* _rp);
    virtual Token* scanStringLiteral(_Page* _rp);
    virtual Token* scanCharacterLiteral(_Page* _rp);
    virtual bool parseKeyword(String* fixedString);
    virtual String* parseIdentifier(_Page* _rp);
    virtual Literal* parseLiteral(_Page* _rp);
    virtual bool parsePunctuation(String* fixedString);
    virtual String* parseOperator(_Page* _rp);
    virtual String* parsePrefixOperator(_Page* _rp);
    virtual String* parseBinaryOperator(_Page* _rp);
    virtual String* parsePostfixOperator(_Page* _rp);
    virtual bool skipWhitespace();
    virtual bool isAtEnd();
    virtual Position* getPosition(_Page* _rp);
    virtual Position* getPreviousPosition(_Page* _rp);
    virtual void handleSingleLineComment();
    virtual void handleMultiLineComment();

};

}
#endif // __scalyc__Lexer__
