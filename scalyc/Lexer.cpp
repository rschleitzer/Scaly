#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Position::Position(size_t line, size_t column) {
    this->line = line;
    this->column = column;
}

bool Token::_isEofToken() { return false; }
bool Token::_isInvalidToken() { return false; }
bool Token::_isIdentifier() { return false; }
bool Token::_isLiteral() { return false; }
bool Token::_isStringLiteral() { return false; }
bool Token::_isCharacterLiteral() { return false; }
bool Token::_isNumericLiteral() { return false; }
bool Token::_isPunctuation() { return false; }
bool Token::_isOperator() { return false; }
bool Token::_isPrefixOperator() { return false; }
bool Token::_isBinaryOperator() { return false; }
bool Token::_isPostfixOperator() { return false; }

bool EofToken::_isEofToken() { return true; }

bool InvalidToken::_isInvalidToken() { return true; }

Identifier::Identifier(_LetString* name) {
    this->name = name;
}

bool Identifier::_isIdentifier() { return true; }

bool Literal::_isLiteral() { return true; }

bool Literal::_isStringLiteral() { return false; }
bool Literal::_isCharacterLiteral() { return false; }
bool Literal::_isNumericLiteral() { return false; }

StringLiteral::StringLiteral(_LetString* theString) {
    string = theString;
}

bool StringLiteral::_isStringLiteral() { return true; }

CharacterLiteral::CharacterLiteral(_LetString* theString) {
    value = theString;
}

bool CharacterLiteral::_isCharacterLiteral() { return true; }

NumericLiteral::NumericLiteral(_LetString* theValue) {
    value = theValue;
}

bool NumericLiteral::_isNumericLiteral() { return true; }

Punctuation::Punctuation(_LetString* theSign) {
    sign = theSign;
}

bool Punctuation::_isPunctuation() { return true; }

bool Operator::_isOperator() { return true; }

bool Operator::_isPrefixOperator() { return false; }
bool Operator::_isBinaryOperator() { return false; }
bool Operator::_isPostfixOperator() { return false; }

PrefixOperator::PrefixOperator(_LetString* theOperation) {
    operation = theOperation;
}

bool PrefixOperator::_isPrefixOperator() { return true; }

BinaryOperator::BinaryOperator(_LetString* theOperation) {
    operation = theOperation;
}

bool BinaryOperator::_isBinaryOperator() { return true; }

PostfixOperator::PostfixOperator(_LetString* theOperation) {
    operation = theOperation;
}

bool PostfixOperator::_isPostfixOperator() { return true; }

Lexer::Lexer(_LetString* theText) {
    token = new(getPage()->allocateExclusivePage()) InvalidToken();
    whitespaceSkipped = true;
    text = theText;
    end = text->getLength();
    position = 0;
    previousLine = 1;
    previousColumn = 0;
    line = 1;
    column = 0;
    advance();
}

void Lexer::advance() {
    skipWhitespace();
    previousLine = line;
    previousColumn = column;
    if (position == end) {
        token->getPage()->clear();
        token = new(token->getPage()) EofToken();
        return;
    }
    char c = (*text)[position];
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        token->getPage()->clear();
        token = scanIdentifier(token->getPage());
        return;
    }
    if ((c >= '0') && (c <= '9')) {
        token->getPage()->clear();
        token = scanNumericLiteral(token->getPage());
        return;
    }
    switch (c) {
        case '\"': {
            token->getPage()->clear();
            token = scanStringLiteral(token->getPage());
            break;
        }

        case '\'': {
            token->getPage()->clear();
            token = scanCharacterLiteral(token->getPage());
            break;
        }

        case '_': case '(': case ')': case '{': case '}': case '[': case ']': case ',': case ':': case ';': case '@': case '#': case '`': {
            {
                token->getPage()->clear();
                token = new(token->getPage()) Punctuation(&_LetString::create(token->getPage(), (*text)[position]));
                position++;
                column++;
            }
            break;
        }

        case '/': case '+': case '*': case '%': case '&': case '|': case '^': case '~': {
            token->getPage()->clear();
            token = scanOperator(token->getPage(), false);
            break;
        }

        case '<': case '>': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) InvalidToken();
                }
                else {
                    switch ((*text)[position]) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': case '.': case ' ': case '\t': case '\r': case '\n': {
                            {
                                position--;
                                column--;
                                token->getPage()->clear();
                                token = scanOperator(token->getPage(), true);
                            }
                            break;
                        }
                        default:
                            token->getPage()->clear();
                            token = new (token->getPage()) Punctuation(&_LetString::create(token->getPage(), c));
                    }
                }
            }
            break;
        }

        case '.': {
            position++; column++;
            if (position == end) {
                token->getPage()->clear();
                token = new (token->getPage()) InvalidToken();
            }
            else {
                if ((*text)[position] == '.') {
                    position--; column--;
                    token->getPage()->clear();
                    token = scanOperator(token->getPage(), true);
                }
                else {
                    token->getPage()->clear();
                    token = new (token->getPage()) Punctuation(&_LetString::create(token->getPage(), '.'));
                }
            }
            break;
        }

        case '-': {
            position++; column++;
            if (position == end) {
                token->getPage()->clear();
                token = new (token->getPage()) InvalidToken();
            }
            else {
                if ((*text)[position] != '>') {
                    position--; column--;
                    token->getPage()->clear();
                    token = scanOperator(token->getPage(), true);
                }
                else {
                    token->getPage()->clear();
                    token = new(token->getPage()) Punctuation(&_LetString::create(token->getPage(), "->"));
                    position++; column++;
                }
            }
            break;
        }

        case '!': {
            position++; column++;
            if (position == end) {
                token->getPage()->clear();
                token = new(token->getPage()) PostfixOperator(&_LetString::create(token->getPage(), "!"));
            }
            else {
                switch ((*text)[position]) {
                    case '/': case '=': case '+': case '!': case'*': case '%': case '&': case '|': case '^': case '~': case '.':
                    case ' ': case '\t': case '\r': case '\n': {
                        position--; column--;
                        token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                        break;
                    }

                    default:
                        if ((whitespaceSkipped) || (token && (token->_isPunctuation()))) {
                            position--; column--;
                            token->getPage()->clear();
                            token = scanOperator(token->getPage(), true);
                        }
                        else {
                            token->getPage()->clear();
                            token = new(getPage()) Punctuation(&_LetString::create(token->getPage(), "!"));
                        }
                }
            }
            break;
        }

        case '?': {
            position++; column++;
            if (position == end) {
                token->getPage()->clear();
                token = new(token->getPage()) PostfixOperator(&_LetString::create(token->getPage(), "?"));
            }
            else {
                switch ((*text)[position]) {
                    case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                        position--; column--;
                        token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                        break;
                    }
                    default: {
                        if (whitespaceSkipped) {
                            position--; column--;
                            token->getPage()->clear();
                            token = scanOperator(token->getPage(), true);
                        }
                        else {
                            token->getPage()->clear();
                            token = new(token->getPage()) Punctuation(&_LetString::create(token->getPage(),"?"));
                        }
                    }
                }
            }
            break;
        }

        case '=': {
            position++; column++;
            if (position == end) {
                token->getPage()->clear();
                token = new(token->getPage()) InvalidToken();
            }
            else {
                switch ((*text)[position]) {
                    case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                        position--; column--;
                        token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                        break;
                    }
                    default: {
                        token->getPage()->clear();
                        token = new(token->getPage()) Punctuation(&_LetString::create(token->getPage(), "="));
                    }
                }
            }
            break;
        }
        default: {
            token->getPage()->clear();
            token = new(token->getPage()) InvalidToken();
        }
    }
}

Identifier* Lexer::scanIdentifier(_Page* _rp) {
    // Make a String taking the character at the current position
    {
        _Region _region; _Page* _p = _region.get();
        _VarString& name = *new(_p) _VarString((*text)[position]);

        do {
            position++; column++;

            if (position == end) {
                return new(_rp) Identifier(&_LetString::create(_rp, name));
            }

            char c = (*text)[position];
            if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '_'))
                name += (*text)[position];
            else
                return new(_rp) Identifier(&_LetString::create(_rp, name));
        }
        while (true);
    }
}

Operator* Lexer::scanOperator(_Page* _rp, bool includeDots) {
    bool whitespaceSkippedBefore = whitespaceSkipped;
    if (!whitespaceSkippedBefore) {
        if (token->_isPunctuation()) {
            _LetString& sign = *((Punctuation*)token)->sign;
            if ((sign == "(") || (sign == "[") || (sign == "{") || (sign == ",") || (sign == ";") || (sign == ":"))
                whitespaceSkippedBefore = true;
            else
                whitespaceSkippedBefore = false;
        }
    }

    // Make a String taking the character at the current position
    _VarString& operation = *new(_rp) _VarString((*text)[position]);

    do {
        position++; column++;
        if (position == end) {
            if (whitespaceSkippedBefore)
                return new(_rp) BinaryOperator(&_LetString::create(_rp, operation));
            else
                return new(_rp) PostfixOperator(&_LetString::create(_rp, operation));
        }
        switch ((*text)[position]) {
            case '/': case '=': case '-': case '+': case '!': case '*': case '%': case '<': case '&': case '|': case '^': case '~': {
                operation += (*text)[position];
                break;
            }
            case '.': {
                if (includeDots) {
                    operation += (*text)[position];
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
                    switch ((*text)[position]) {
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
                    return new(_rp) BinaryOperator(&_LetString::create(_rp, operation));

                if ((!whitespaceSkippedBefore && whitespaceSkippedAfter))
                    return new(_rp) PostfixOperator(&_LetString::create(_rp, operation));

                if ((whitespaceSkippedBefore && !whitespaceSkippedAfter))
                    return new(_rp) PrefixOperator(&_LetString::create(_rp, operation));
            }
        }
    }
    while (true);
}

Token* Lexer::scanStringLiteral(_Page* _rp) {
    // Make a String taking the character at the current position
    _VarString& value = *new(_rp) _VarString("");

    do {
        position++; column++;
        if (position == end)
            return new(_rp) InvalidToken();
        switch ((*text)[position]) {
            case '\"': {
                position++; column++;
                return new(_rp) StringLiteral(&_LetString::create(_rp, value));
            }
            case '\\': {
                position++; column++;
                switch ((*text)[position]) {
                    case '\"': case '\\': case '\'':
                        value += (*text)[position];
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
                value += (*text)[position];
            }
        }
    }
    while (true);
}

Token* Lexer::scanCharacterLiteral(_Page* _rp) {
    // Make a String taking the character at the current position
    _VarString& value = *new(_rp) _VarString("");

    do {
        position++; column++;
        if (position == end)
            return new(_rp) InvalidToken();
        switch ((*text)[position]) {
            case '\'': {
                position++; column++;
                return new(_rp) CharacterLiteral(&_LetString::create(_rp, value));
            }
            case '\\': {
                position++; column++;
                switch ((*text)[position]) {
                    case '\"': case '\\': case '\'':
                        value += (*text)[position];
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
                value += (*text)[position];
            }
        }
    }
    while (true);
}

NumericLiteral* Lexer::scanNumericLiteral(_Page* _rp) {
    // Make a String taking the character at the current position
    _VarString& value = *new(_rp) _VarString((*text)[position]);

    do {
        position++; column++;

        if (position == end)
            return new(_rp) NumericLiteral(&_LetString::create(_rp, value));

        char c = (*text)[position];
        if ((c >= '0') && (c <= '9'))
            value += (*text)[position];
        else
            return new(_rp) NumericLiteral(&_LetString::create(_rp, value));
    }
    while (true);
}

bool Lexer::skipWhitespace() {
    whitespaceSkipped = false;
    do {
        if (position == end)
            return true;

        switch ((*text)[position]) {
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
                if ((*text)[position] == '/') {
                    whitespaceSkipped = true;
                    handleSingleLineComment();
                }
                else if ((*text)[position] == '*') {
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

        switch ((*text)[position]) {
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

        char character = (*text)[position];
        switch (character) {
            case '/': {
                position++; column++;
                if (position == end)
                    return;
                else if ((*text)[position] == '*')
                    handleMultiLineComment();
                else
                    return;
                break;
            }
            case '*': {
                position++; column++;
                if (position == end)
                    return;
                else if ((*text)[position] == '/') {
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

bool Lexer::parseKeyword(_LetString* fixedString) {
    if (!(token->_isIdentifier()))
        return false;

    Identifier* identifier = (Identifier*)token;
    return *identifier->name == *fixedString;
}

_LetString* Lexer::parseIdentifier(_Page* _rp) {
    if (!(token->_isIdentifier()))
        return 0;

    Identifier* identifier = (Identifier*)token;
    return &_LetString::create(_rp, *identifier->name);
}

bool Lexer::parsePunctuation(_LetString* fixedString) {
    if (!(token->_isPunctuation()))
        return false;

    Punctuation* punctuation = (Punctuation*)token;
    return *punctuation->sign == *fixedString;
}

_LetString* Lexer::parseOperator(_Page* _rp) {
    if (!(token->_isOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return &_LetString::create(_rp, *(op->operation));
}

Literal* Lexer::parseLiteral(_Page* _rp) {
    if (!(token->_isLiteral()))
        return 0;

    if (token->_isStringLiteral()) {
        StringLiteral* stringLiteral = (StringLiteral*)token;
        return new(_rp) StringLiteral(&_LetString::create(_rp, *stringLiteral->string));
    }

    if (token->_isCharacterLiteral()) {
        CharacterLiteral* characterLiteral = (CharacterLiteral*)token;
        return new(_rp) CharacterLiteral(&_LetString::create(_rp, *characterLiteral->value));
    }
    if (token->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)token;
        return new(_rp) NumericLiteral(&_LetString::create(_rp, *numericLiteral->value));
    }
    
    return 0;
}

_LetString* Lexer::parsePrefixOperator(_Page* _rp) {
    if (!(token->_isPrefixOperator()))
        return 0;

    Operator* op = (Operator*)token;

    return &_LetString::create(_rp, *(op->operation));
}

_LetString* Lexer::parseBinaryOperator(_Page* _rp) {
    if (!(token->_isBinaryOperator())) {
        if ((token->_isPunctuation()) && (((*((Punctuation*)token)->sign == "<")) || (*((Punctuation*)token)->sign == ">"))) {
            Operator* op = (Operator*)token;
            return &_LetString::create(_rp, *(op->operation));
        }

        return 0;
    }

    Operator* op = (Operator*)token;
    return &_LetString::create(_rp, *(op->operation));
}


_LetString* Lexer::parsePostfixOperator(_Page* _rp){
    if (!(token->_isPostfixOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return &_LetString::create(_rp, *(op->operation));
}

bool Lexer::isAtEnd() {
    return position == end;
}

Position* Lexer::getPosition(_Page* _rp) {
    return new(_rp) Position(line, column);
}

Position* Lexer::getPreviousPosition(_Page* _rp) {
    return new(_rp) Position(previousLine, previousColumn);
}

}