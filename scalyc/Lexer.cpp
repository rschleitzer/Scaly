#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Position::Position(size_t line, size_t column)
: line(line), column(column) {
}

bool Token::_isEofToken() { return false; }
bool Token::_isInvalidToken() { return false; }
bool Token::_isIdentifier() { return false; }
bool Token::_isLiteral() { return false; }
bool Token::_isStringLiteral() { return false; }
bool Token::_isNumericLiteral() { return false; }
bool Token::_isPunctuation() { return false; }
bool Token::_isOperator() { return false; }
bool Token::_isPrefixOperator() { return false; }
bool Token::_isBinaryOperator() { return false; }
bool Token::_isPostfixOperator() { return false; }

bool EofToken::_isEofToken() { return true; }

bool InvalidToken::_isInvalidToken() { return true; }
    
Identifier::Identifier(String& name) {
    this->name = new(getPage()) String(name);
}

bool Identifier::_isIdentifier() { return true; }

bool Literal::_isLiteral() { return true; }
bool Literal::_isStringLiteral() { return false; }
bool Literal::_isNumericLiteral() { return false; }

StringLiteral::StringLiteral(String& literal)
: string(literal) {
}

bool StringLiteral::_isStringLiteral() { return true; }

NumericLiteral::NumericLiteral(String& value)
: value(value) {
}

bool NumericLiteral::_isNumericLiteral() { return true; }

Punctuation::Punctuation(String& theString)
: sign(theString) {
}

bool Punctuation::_isPunctuation() { return true; }

Operator::Operator(String& theOperation)
: operation(theOperation) {
}

bool Operator::_isOperator() { return true; }

PrefixOperator::PrefixOperator(String& theOperation)
: Operator(theOperation) {
}

bool PrefixOperator::_isPrefixOperator() { return true; }

BinaryOperator::BinaryOperator(String& theOperation)
: Operator(theOperation) {
}

bool BinaryOperator::_isBinaryOperator() { return true; }

PostfixOperator::PostfixOperator(String& theOperation)
: Operator(theOperation) {
}

bool PostfixOperator::_isPostfixOperator() { return true; }

Lexer::Lexer(String& text)
: token(0), whitespaceSkipped(true), text(text), length(text.getLength()),
position(0), end(length), previousLine(1), previousColumn(0), line(1), column(0) {
    advance();
}

void Lexer::advance() {        
    skipWhitespace();

    previousLine = line;
    previousColumn = column;

    if (position == end) {
        token = new (getPage()) EofToken();
        return;
    }
    
    char c = text[position];
    
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        token = &scanIdentifier(getPage());
        return;
    }
    
    if ((c >= '0') && (c <= '9')) {
        token = scanNumericLiteral(getPage());
        return;
    }

    switch (c) {
        case '\"': token = scanStringLiteral(getPage()); {
            break;
        }
            
        case '_': case '(': case ')': case '{': case '}': case '[': case ']': case '<': case '>': case ',': case ':': case ';':  case '@': case '#': case '`': {            
            token = new (getPage()) Punctuation(*new(getPage()) String(text[position]));
            position++; column++;
            break;
        }

        case '/': case '+': case '*':  case '%': case '&': case '|': case '^': case '~': {
            token = &scanOperator(getPage(), false);
            break;
        }
        
        case '.': {
            position++; column++;
            if (position == end) {
                token = new (getPage()) InvalidToken();
            }
            else {
                if (text[position] == '.') {
                    position--; column--;
                    token = &scanOperator(getPage(), true);
                }
                else {
                    token = new (getPage()) Punctuation(*new(getPage()) String('.'));
                }
            }
            break;
        }
        
        case '-': {
            position++; column++;
            if (position == end) {
                token = new (getPage()) InvalidToken();
            }
            else {
                if (text[position] != '>') {
                    position--; column--;
                    token = &scanOperator(getPage(), true);
                }
                else {
                    token = new(getPage()) Punctuation(*new(getPage()) String("->"));
                    position++; column++;
                }
            }
            break;
        }
        
        case '!': {
            position++; column++;
            if (position == end) {
                token = new(getPage()) PostfixOperator(*new(getPage()) String("!"));
            }
            else {
                switch (text[position]) {
                    case '/': case '=': case '+': case '!': case'*': case '%': case '&': case '|': case '^': case '~': case '.':
                    case ' ': case '\t': case '\r': case '\n': {
                        position--; column--;
                        token = &scanOperator(getPage(), true);
                        break;
                    }

                    default:
                        if ((whitespaceSkipped) || (token && (token->_isPunctuation()))) {
                            position--; column--;
                            token = &scanOperator(getPage(), true);
                        }
                        else {
                            token = new(getPage()) Punctuation(*new(getPage()) String("!"));
                        }
                } 
            }
            break;
        }
        
        case '?': {
            position++; column++;
            if (position == end) {
                token = new(getPage()) PostfixOperator(*new(getPage()) String("?"));
            }
            else {
                switch (text[position]) {
                    case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                        position--; column--;
                        token = &scanOperator(getPage(), true);
                        break;
                    }
                    default: {
                        if (whitespaceSkipped) {
                            position--; column--;
                            token = &scanOperator(getPage(), true);
                        }
                        else {
                            token = new(getPage()) Punctuation(*new(getPage()) String("?"));
                        }
                    }
                } 
            }
            break;
        }

        case '=': {
            position++; column++;
            if (position == end) {
                token = new(getPage()) InvalidToken();
            }
            else {
                switch (text[position]) {
                    case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                        position--; column--;
                        token = &scanOperator(getPage(), true);
                        break;
                    }
                    default: {
                        token = new(getPage()) Punctuation(*new(getPage()) String("="));
                    }
                }
            }
            break;
        }
        default: {
            token = new(getPage()) InvalidToken();
        }
    }
}

Identifier& Lexer::scanIdentifier(_Page* _rp) {    
    // Make a String taking the character at the current position
    {
        _Region _region; _Page* _p = _region.get();
        String& name = *new(_p) String(text[position]);

        do {
            position++; column++;
            
            if (position == end) {
                return *new(_rp) Identifier(name);
            }
            
            char c = text[position];
            if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_'))
                name += text[position];
            else
                return *new(_rp) Identifier(name);
        }            
        while (true);
    }
}

Operator& Lexer::scanOperator(_Page* _rp, bool includeDots) {
    bool whitespaceSkippedBefore = whitespaceSkipped;
    if (!whitespaceSkippedBefore) {
        if (token->_isPunctuation()) {
            String& sign = ((Punctuation*)token)->sign;
            if ((sign == "(") || (sign == "[") || (sign == "{") || (sign == ",") || (sign == ";") || (sign == ":"))
                whitespaceSkippedBefore = true;
            else
                whitespaceSkippedBefore = false;
        }
    }
    
    // Make a String taking the character at the current position
    String& operation = *new(_rp) String(text[position]);

    do {
        position++; column++;
        if (position == end) {
            if (whitespaceSkippedBefore)
                return *new(_rp) BinaryOperator(operation);
            else
                return *new(_rp) PostfixOperator(operation);
        }
        switch (text[position]) {
            case '/': case '=': case '-': case '+': case '!': case '*': case '%': case '<': case '&': case '|': case '^': case '~': {
                operation += text[position];
                break;
            }
            case '.': {
                if (includeDots) {
                    operation += text[position];
                    break;
                }
                // else fallthrough
            }
            default: {
                size_t lastPosition = position;
                size_t lastLine = line;
                size_t lastColumn = column;
                skipWhitespace();
                position = lastPosition;
                line = lastLine;
                column = lastColumn;
                bool whitespaceSkippedAfter = whitespaceSkipped;
                if (!whitespaceSkippedAfter) {
                    switch (text[position]) {
                        case ')': case ']': case '}': case ',': case ';': case ':': case '.': {
                            whitespaceSkippedAfter = true;
                            break;
                        }
                        default: {
                            whitespaceSkippedAfter = false;
                        }
                    }
                }

                if ((whitespaceSkippedBefore && whitespaceSkippedAfter) || (!whitespaceSkippedBefore && !whitespaceSkippedAfter))
                    return *new(_rp) BinaryOperator(operation);
                    
                if ((!whitespaceSkippedBefore && whitespaceSkippedAfter))
                    return *new(_rp) PostfixOperator(operation);
                    
                if ((whitespaceSkippedBefore && !whitespaceSkippedAfter))
                    return *new(_rp) PrefixOperator(operation);
            }
        }
    }
    while (true);
}

Token* Lexer::scanStringLiteral(_Page* _rp) {
    // Make a String taking the character at the current position
    String& value = *new(_rp) String("");

    do {
        position++; column++;
        if (position == end)
            return new(_rp) InvalidToken();
        switch (text[position]) {
            case '\"': {
                position++; column++;
                return new(_rp) StringLiteral(value);
            }
            case '\\': {
                position++; column++;
                switch (text[position]) {
                    case '\"': case '\\': case '\'':
                        value += text[position];
                        break;
                    case 'n': value += '\n'; break;
                    case 'r': value += '\r'; break;
                    case 't': value += '\t'; break;
                    case '0': value += '\0'; break;
                    default:
                        return new(_rp) InvalidToken();
                }
                break;
            }
            default: {
                value += text[position];
            }
        }
    } 
    while (true);
}

NumericLiteral* Lexer::scanNumericLiteral(_Page* _rp) {    
    // Make a String taking the character at the current position
    String& value = *new(_rp) String(text[position]);

    do {
        position++; column++;
        
        if (position == end)
            return new(_rp) NumericLiteral(value);
        
        char c = text[position];
        if ((c >= '0') && (c <= '9'))
            value += text[position];
        else
            return new(_rp) NumericLiteral(value);
    }            
    while (true);
}
        
bool Lexer::skipWhitespace() {
    whitespaceSkipped = false;
    do {
        if (position == end)
            return true;

        switch (text[position]) {
            case ' ': {
                whitespaceSkipped = true;
                position++; column++;
                continue;
            }
            case '\t': {
                whitespaceSkipped = true;
                position++; column+=4;
                continue;
            }
            case '\r': {
                whitespaceSkipped = true;
                position++;
                continue;
            }
            case '\n': {
                whitespaceSkipped = true;
                position++; column = 1; line++;
                continue;
            }
            case '/': {
                position++; column++;
                if (position == end)
                    return whitespaceSkipped;
                if (text[position] == '/') { 
                    whitespaceSkipped = true;
                    handleSingleLineComment();
                }
                else if (text[position] == '*') { 
                    whitespaceSkipped = true;
                    handleMultiLineComment();
                }
                else {
                    return whitespaceSkipped;
                }
                break;
            }
            default: {
                return whitespaceSkipped;
            }
        }
    } 
    while (true);
}

void Lexer::handleSingleLineComment() {
    do {
        if (position == end)
            return;
            
        switch (text[position]) {
            case '\t': {
                whitespaceSkipped = true;
                position++; column+=4;
                continue;
            }
            case '\r': {
                whitespaceSkipped = true;
                position++;
                continue;
            }
            case '\n': {
                whitespaceSkipped = true;
                position++; column = 1; line++;
                return;
            }
            default: {
                position++; column++;
                continue;
            }
        }
    }
    while (true);
}

void Lexer::handleMultiLineComment() {
    do {
        if (position == end)
            return;
            
        char character = text[position];
        switch (character) {
            case '/': {
                position++; column++;
                if (position == end)
                    return;
                else if (text[position] == '*')
                    handleMultiLineComment();
                else
                    return;
                break;
            }
            case '*': {
                position++; column++;
                if (position == end)
                    return;
                else if (text[position] == '/') { 
                    position++; column++;
                    return;
                }
                // else fallthrough
            }
            case '\t': {
                whitespaceSkipped = true;
                position++; column+=4;
                continue;
            }
            case '\r': {
                whitespaceSkipped = true;
                position++;
                continue;
            }
            case '\n': {
                whitespaceSkipped = true;
                position++; column = 1; line++;
                continue;
            }
            default: {
                position++; column++;
                continue;
            }
        }
    }
    while (true);
}

bool Lexer::parseKeyword(const char* fixedString) {
    if (!(token->_isIdentifier()))
        return false;

    Identifier* identifier = (Identifier*)token;
    return *identifier->name == fixedString;
}

String* Lexer::parseIdentifier(_Page* _rp) {
    if (!(token->_isIdentifier()))
        return 0;
    
    Identifier* identifier = (Identifier*)token;
    return new(_rp) String(*identifier->name);
}

bool Lexer::parsePunctuation(const char* fixedString) {
    if (!(token->_isPunctuation()))
        return false;

    Punctuation* punctuation = (Punctuation*)token;
    return punctuation->sign == fixedString;
}

String* Lexer::parseOperator(_Page* _rp) {
    if (!(token->_isOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return new(_rp) String(op->operation);
}

Literal* Lexer::parseLiteral(_Page* _rp) {
    if (!(token->_isLiteral()))
        return 0;
    
    return (Literal*)token;
}

String* Lexer::parsePrefixOperator(_Page* _rp) {
    if (!(token->_isPrefixOperator()))
        return 0;

    Operator* op = (Operator*)token;

    return new(_rp) String(op->operation);
}

String* Lexer::parseBinaryOperator(_Page* _rp) {
    if (!(token->_isBinaryOperator())) {
        if ((token->_isPunctuation()) && (((((Punctuation*)token)->sign == "<")) || (((Punctuation*)token)->sign == ">"))) {
            Operator* op = (Operator*)token;
            return new(_rp) String(op->operation);
        }
        
        return 0;
    }

    Operator* op = (Operator*)token;
    return new(_rp) String(op->operation);
}


String* Lexer::parsePostfixOperator(_Page* _rp){
    if (!(token->_isPostfixOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return new(_rp) String(op->operation);
}

bool Lexer::isAtEnd() {
    return position == end;
}

Position Lexer::getPosition() {
    return Position(line, column);
}

Position Lexer::getPreviousPosition() {
    return Position(previousLine, previousColumn);
}

}