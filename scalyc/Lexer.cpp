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

Identifier::Identifier(String* name) {
    this->name = name;
}

bool Identifier::_isIdentifier() { return true; }

bool Literal::_isLiteral() { return true; }

bool Literal::_isStringLiteral() { return false; }
bool Literal::_isCharacterLiteral() { return false; }
bool Literal::_isNumericLiteral() { return false; }

StringLiteral::StringLiteral(String* theString) {
    string = theString;
}

bool StringLiteral::_isStringLiteral() { return true; }

CharacterLiteral::CharacterLiteral(String* theString) {
    value = theString;
}

bool CharacterLiteral::_isCharacterLiteral() { return true; }

NumericLiteral::NumericLiteral(String* theValue) {
    value = theValue;
}

bool NumericLiteral::_isNumericLiteral() { return true; }

Punctuation::Punctuation(String* theSign) {
    sign = theSign;
}

bool Punctuation::_isPunctuation() { return true; }

bool Operator::_isOperator() { return true; }

bool Operator::_isPrefixOperator() { return false; }
bool Operator::_isBinaryOperator() { return false; }
bool Operator::_isPostfixOperator() { return false; }

PrefixOperator::PrefixOperator(String* theOperation) {
    operation = theOperation;
}

bool PrefixOperator::_isPrefixOperator() { return true; }

BinaryOperator::BinaryOperator(String* theOperation) {
    operation = theOperation;
}

bool BinaryOperator::_isBinaryOperator() { return true; }

PostfixOperator::PostfixOperator(String* theOperation) {
    operation = theOperation;
}

bool PostfixOperator::_isPostfixOperator() { return true; }

Lexer::Lexer(String* theText) {
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
                token = new(token->getPage()) Punctuation(&String::create(token->getPage(), (*text)[position]));
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

                        default: {
                            token->getPage()->clear();
                            token = new(token->getPage()) Punctuation(&String::create(token->getPage(), c));
                        }
                    }
                }
            }
            break;
        }

        case '.': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) InvalidToken();
                }
                else {
                    if ((*text)[position] == '.') {
                        position--;
                        column--;
                        token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                    }
                    else {
                        token->getPage()->clear();
                        token = new(token->getPage()) Punctuation(&String::create(token->getPage(), '.'));
                    }
                }
            }
            break;
        }

        case '-': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) InvalidToken();
                }
                else {
                    if ((*text)[position] != '>') {
                        position--;
                        column--;
                        token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                    }
                    else {
                        token->getPage()->clear();
                        token = new(token->getPage()) Punctuation(&String::create(token->getPage(), "->"));
                        position++;
                        column++;
                    }
                }
            }
            break;
        }

        case '!': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) PostfixOperator(&String::create(token->getPage(), "!"));
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

                        default: {
                            {
                                if (whitespaceSkipped || ((token != nullptr) && (token->_isPunctuation()))) {
                                    position--;
                                    column--;
                                    token->getPage()->clear();
                                    token = scanOperator(token->getPage(), true);
                                }
                                else {
                                    token->getPage()->clear();
                                    token = new(token->getPage()) Punctuation(&String::create(token->getPage(), "!"));
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case '?': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) PostfixOperator(&String::create(token->getPage(), "?"));
                }
                else {
                    switch ((*text)[position]) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                            {
                                position--;
                                column--;
                                token->getPage()->clear();
                                token = scanOperator(token->getPage(), true);
                            }
                            break;
                        }

                        default: {
                            {
                                if (whitespaceSkipped) {
                                    position--;
                                    column--;
                                    token->getPage()->clear();
                                    token = scanOperator(token->getPage(), true);
                                }
                                else {
                                    token->getPage()->clear();
                                    token = new(token->getPage()) Punctuation(&String::create(token->getPage(), "?"));
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        case '=': {
            {
                position++;
                column++;
                if (position == end) {
                    token->getPage()->clear();
                    token = new(token->getPage()) InvalidToken();
                }
                else {
                    switch ((*text)[position]) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                            {
                                position--;
                                column--;
                                token->getPage()->clear();
                                token = scanOperator(token->getPage(), true);
                            }
                            break;
                        }

                        default: {
                            token->getPage()->clear();
                            token = new(token->getPage()) Punctuation(&String::create(token->getPage(), "="));
                        }
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
    _Region _region; _Page* _p = _region.get();
    VarString* name = new(_p) VarString((*text)[position]);
    do {
        position++;
        column++;
        if (position == end)
            return new(_rp) Identifier(&String::create(_rp, name));
        char c = (*text)[position];
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
            name->append((*text)[position]);
        else
            return new(_rp) Identifier(&String::create(_rp, name));
    }
    while (true);
}

Operator* Lexer::scanOperator(_Page* _rp, bool includeDots) {
    _Region _region; _Page* _p = _region.get();
    bool whitespaceSkippedBefore = whitespaceSkipped;
    if (!whitespaceSkippedBefore) {
        if (token->_isPunctuation()) {
            String* sign = ((Punctuation*)token)->sign;
            if (sign->equals("(") || sign->equals("[") || sign->equals("{") || sign->equals(",") || sign->equals(";") || sign->equals(":"))
                whitespaceSkippedBefore = true;
            else
                whitespaceSkippedBefore = false;
        }
    }
    VarString* operation = new(_p) VarString((*text)[position]);
    do {
        position++;
        column++;
        if (position == end) {
            if (whitespaceSkippedBefore)
                return new(_rp) BinaryOperator(&String::create(_rp, operation));
            else
                return new(_rp) PostfixOperator(&String::create(_rp, operation));
        }
        if (includeDots && ((*text)[position] == '.')) {
            operation->append((*text)[position]);
            continue;
        }
        switch ((*text)[position]) {
            case '/': case '=': case '-': case '+': case '!': case '*': case '%': case '<': case '&': case '|': case '^': case '~': {
                operation->append((*text)[position]);
                break;
            }

            default: {
                {
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
                        return new(_rp) BinaryOperator(&String::create(_rp, operation));
                    if ((!whitespaceSkippedBefore && whitespaceSkippedAfter))
                        return new(_rp) PostfixOperator(&String::create(_rp, operation));
                    if ((whitespaceSkippedBefore && !whitespaceSkippedAfter))
                        return new(_rp) PrefixOperator(&String::create(_rp, operation));
                }
            }
        }
    }
    while (true);
}

Token* Lexer::scanStringLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = new(_p) VarString("");
    do {
        position++;
        column++;
        if (position == end)
            return new(_rp) InvalidToken();
        switch ((*text)[position]) {
            case '\"': {
                {
                    position++;
                    column++;
                    return new(_rp) StringLiteral(&String::create(_rp, value));
                }
                break;
            }

            case '\\': {
                {
                    position++;
                    column++;
                    switch ((*text)[position]) {
                        case '\"': case '\\': case '\'': {
                            value->append((*text)[position]);
                            break;
                        }

                        case 'n': {
                            value->append('\n');
                            break;
                        }

                        case 'r': {
                            value->append('\r');
                            break;
                        }

                        case 't': {
                            value->append('\t');
                            break;
                        }

                        case '0': {
                            value->append('\0');
                            break;
                        }

                        default: {
                            return new(_rp) InvalidToken();
                        }
                    }
                }
                break;
            }

            default: {
                value->append((*text)[position]);
            }
        }
    }
    while (true);
}

Token* Lexer::scanCharacterLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = new(_p) VarString("");
    do {
        position++;
        column++;
        if (position == end)
            return new(_rp) InvalidToken();
        switch ((*text)[position]) {
            case '\'': {
                {
                    position++;
                    column++;
                    return new(_rp) CharacterLiteral(&String::create(_rp, value));
                }
                break;
            }

            case '\\': {
                {
                    position++;
                    column++;
                    switch ((*text)[position]) {
                        case '\"': case '\\': case '\'': {
                            value->append((*text)[position]);
                            break;
                        }

                        case 'n': {
                            value->append('\n');
                            break;
                        }

                        case 'r': {
                            value->append('\r');
                            break;
                        }

                        case 't': {
                            value->append('\t');
                            break;
                        }

                        case '0': {
                            value->append('\0');
                            break;
                        }

                        default: {
                            return new(_rp) InvalidToken();
                        }
                    }
                }
                break;
            }

            default: {
                {
                    value->append((*text)[position]);
                }
            }
        }
    }
    while (true);
}

NumericLiteral* Lexer::scanNumericLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = new(_p) VarString((*text)[position]);
    do {
        position++;
        column++;
        if (position == end)
            return new(_rp) NumericLiteral(&String::create(_rp, value));
        char c = (*text)[position];
        if ((c >= '0') && (c <= '9'))
            value->append((*text)[position]);
        else
            return new(_rp) NumericLiteral(&String::create(_rp, value));
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
                {
                    whitespaceSkipped = true;
                    position++;
                    column++;
                    continue;
                }
                break;
            }

            case '\t': {
                {
                    whitespaceSkipped = true;
                    position++;
                    column += 4;
                    continue;
                }
                break;
            }

            case '\r': {
                {
                    whitespaceSkipped = true;
                    position++;
                    continue;
                }
                break;
            }

            case '\n': {
                {
                    whitespaceSkipped = true;
                    position++;
                    column = 1;
                    line++;
                    continue;
                }
                break;
            }

            case '/': {
                {
                    position++;
                    column++;
                    if (position == end)
                        return whitespaceSkipped;
                    if ((*text)[position] == '/') {
                        whitespaceSkipped = true;
                        handleSingleLineComment();
                    }
                    else {
                        if ((*text)[position] == '*') {
                            whitespaceSkipped = true;
                            handleMultiLineComment();
                        }
                        else
                            return whitespaceSkipped;
                    }
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
                {
                    whitespaceSkipped = true;
                    position++;
                    column += 4;
                    continue;
                }
                break;
            }
            
            case '\r': {
                {
                    whitespaceSkipped = true;
                    position++;
                    continue;
                }
                break;
            }
            
            case '\n': {
                {
                    whitespaceSkipped = true;
                    position++;
                    column = 1;
                    line++;
                    return;
                }
                break;
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
                {
                    position++; column++;
                    if (position == end)
                        return;
                    else if ((*text)[position] == '*')
                        handleMultiLineComment();
                    else
                        return;
                }
                break;
            }
            case '*': {
                {
                    position++; column++;
                    if (position == end)
                        return;
                    else if ((*text)[position] == '/') {
                        position++; column++;
                        return;
                    }
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

bool Lexer::parseKeyword(String* fixedString) {
    if (!(token->_isIdentifier()))
        return false;

    Identifier* identifier = (Identifier*)token;
    return identifier->name->equals(fixedString);
}

String* Lexer::parseIdentifier(_Page* _rp) {
    if (!(token->_isIdentifier()))
        return 0;

    Identifier* identifier = (Identifier*)token;
    return &String::create(_rp, identifier->name);
}

bool Lexer::parsePunctuation(String* fixedString) {
    if (!(token->_isPunctuation()))
        return false;

    Punctuation* punctuation = (Punctuation*)token;
    return punctuation->sign->equals(fixedString);
}

String* Lexer::parseOperator(_Page* _rp) {
    if (!(token->_isOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return &String::create(_rp, op->operation);
}

Literal* Lexer::parseLiteral(_Page* _rp) {
    if (!(token->_isLiteral()))
        return 0;

    if (token->_isStringLiteral()) {
        StringLiteral* stringLiteral = (StringLiteral*)token;
        return new(_rp) StringLiteral(&String::create(_rp, stringLiteral->string));
    }

    if (token->_isCharacterLiteral()) {
        CharacterLiteral* characterLiteral = (CharacterLiteral*)token;
        return new(_rp) CharacterLiteral(&String::create(_rp, characterLiteral->value));
    }
    if (token->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)token;
        return new(_rp) NumericLiteral(&String::create(_rp, numericLiteral->value));
    }
    
    return 0;
}

String* Lexer::parsePrefixOperator(_Page* _rp) {
    if (!(token->_isPrefixOperator()))
        return 0;

    Operator* op = (Operator*)token;

    return &String::create(_rp, op->operation);
}

String* Lexer::parseBinaryOperator(_Page* _rp) {
    if (!(token->_isBinaryOperator())) {
        if ((token->_isPunctuation()) && ((((Punctuation*)token)->sign->equals("<")) || (((Punctuation*)token)->sign->equals(">")))) {
            Operator* op = (Operator*)token;
            return &String::create(_rp, op->operation);
        }

        return 0;
    }

    Operator* op = (Operator*)token;
    return &String::create(_rp, op->operation);
}


String* Lexer::parsePostfixOperator(_Page* _rp){
    if (!(token->_isPostfixOperator()))
        return 0;

    Operator* op = (Operator*)token;
    return &String::create(_rp, op->operation);
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