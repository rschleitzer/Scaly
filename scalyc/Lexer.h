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
    Identifier(_LetString* name);
    _LetString* name;
    virtual bool _isIdentifier();
};

class Literal : public Token {
public:
    virtual bool _isLiteral();
};

class StringLiteral : public Literal {
public:
    StringLiteral(_LetString* theString);
    _LetString* string;
    virtual bool _isStringLiteral();
};

class NumericLiteral : public Literal {
public:
    NumericLiteral(_LetString* theValue);
    _LetString* value;
    virtual bool _isNumericLiteral();
};

class Punctuation : public Token {
public:
    Punctuation(_LetString* theSign);
    _LetString* sign;
    virtual bool _isPunctuation();
};

class Operator : public Token {
public:
    Operator(_LetString* theOperation);
    _LetString* operation;
    virtual bool _isOperator();
};

class PrefixOperator : public Operator {
public:
    PrefixOperator(_LetString* theOperation);
    virtual bool _isPrefixOperator();
};

class BinaryOperator : public Operator {
public:
    BinaryOperator(_LetString* theOperation);
    virtual bool _isBinaryOperator();
};

class PostfixOperator : public Operator {
public:
    PostfixOperator(_LetString* theOperation);
    virtual bool _isPostfixOperator();
};

class Lexer : public Object {
public:
    Token* token;
    bool whitespaceSkipped;
    _LetString* text;
    size_t length;
    size_t position;
    size_t end;
    size_t previousLine;
    size_t previousColumn;
    size_t line;
    size_t column;
    Lexer(_LetString* text);
    void advance();
    Identifier* scanIdentifier(_Page* _rp);
    Operator* scanOperator(_Page* _rp, bool includeDots);
    NumericLiteral* scanNumericLiteral(_Page* _rp);
    Token* scanStringLiteral(_Page* _rp);
    bool parseKeyword(const char* fixedString);
    _LetString* parseIdentifier(_Page* _rp);
    Literal* parseLiteral(_Page* _rp);
    _LetString* parseOperator(_Page* _rp);
    _LetString* parsePrefixOperator(_Page* _rp);
    _LetString* parseBinaryOperator(_Page* _rp);
    _LetString* parsePostfixOperator(_Page* _rp);
    bool skipWhitespace();
    bool parsePunctuation(const char* fixedString);
    bool isAtEnd();
    Position getPosition();
    Position getPreviousPosition();

    void handleSingleLineComment();
    void handleMultiLineComment();
};

}

#endif /* defined(__scalyc__Lexer__) */
