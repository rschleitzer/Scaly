#ifndef __scalyc__Lexer__
#define __scalyc__Lexer__
#include "scalycpp.h"
using namespace scaly;
namespace scalyc {

class Position;

class Token;

class EofToken;

class InvalidToken;

class Identifier;

class Literal;

class StringLiteral;

class CharacterLiteral;

class NumericLiteral;

class Punctuation;

class Operator;

class PrefixOperator;

class BinaryOperator;

class PostfixOperator;

class Lexer : public Object {
public:
    Token* token; bool whitespaceSkipped;
    string* text;
    size_t position;
    size_t end;
    size_t previousLine; size_t previousColumn;
    size_t line;
    size_t column;
    Lexer(string* theText);
    virtual void advance();
    virtual Identifier* scanIdentifier(_Page* _rp);
    virtual Operator* scanOperator(_Page* _rp, bool includeDots);
    virtual Token* scanStringLiteral(_Page* _rp);
    virtual Token* scanCharacterLiteral(_Page* _rp);
    virtual NumericLiteral* scanNumericLiteral(_Page* _rp);
    virtual bool skipWhitespace();
    virtual void handleSingleLineComment();
    virtual void handleMultiLineComment();
    virtual bool parseKeyword(string* fixedString);
    virtual string* parseIdentifier(_Page* _rp);
    virtual bool parsePunctuation(string* fixedString);
    virtual string* parseOperator(_Page* _rp);
    virtual Literal* parseLiteral(_Page* _rp);
    virtual string* parsePrefixOperator(_Page* _rp);
    virtual string* parseBinaryOperator(_Page* _rp);
    virtual string* parsePostfixOperator(_Page* _rp);
    virtual bool isAtEnd();
    virtual Position* getPosition(_Page* _rp);
    virtual Position* getPreviousPosition(_Page* _rp);

};

class Position : public Object {
public:
    Position(size_t line, size_t column);
    Position(Position* position);
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
    Identifier(string* name);
    string* name;

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
    StringLiteral(string* theString);
    string* value;

    virtual bool _isStringLiteral();
};

class CharacterLiteral : public Literal {
public:
    CharacterLiteral(string* theString);
    string* value;

    virtual bool _isCharacterLiteral();
};

class NumericLiteral : public Literal {
public:
    NumericLiteral(string* theValue);
    string* value;

    virtual bool _isNumericLiteral();
};

class Punctuation : public Token {
public:
    Punctuation(string* theSign);
    string* sign;

    virtual bool _isPunctuation();
};

class Operator : public Token {
public:
    string* operation;

    virtual bool _isOperator();
    virtual bool _isPrefixOperator();
    virtual bool _isBinaryOperator();
    virtual bool _isPostfixOperator();
};

class PrefixOperator : public Operator {
public:
    PrefixOperator(string* theOperation);

    virtual bool _isPrefixOperator();
};

class BinaryOperator : public Operator {
public:
    BinaryOperator(string* theOperation);

    virtual bool _isBinaryOperator();
};

class PostfixOperator : public Operator {
public:
    PostfixOperator(string* theOperation);

    virtual bool _isPostfixOperator();
};

}
#endif // __scalyc__Lexer__
