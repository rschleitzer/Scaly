#include "scalyc.h"
using namespace scaly;
namespace scalyc {

Position::Position(size_t line, size_t column) {
    this->line = line;
    this->column = column;
}

Position::Position(Position* position) {
    this->line = position->line;
    this->column = position->column;
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
        if (token != nullptr)
            token->getPage()->clear();
        token = new(getPage()->allocateExclusivePage()) EofToken();
        return;
    }
    char c = text->charAt(position);
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) {
        if (token != nullptr)
            token->getPage()->clear();
        token = scanIdentifier(token->getPage());
        return;
    }
    if ((c >= '0') && (c <= '9')) {
        if (token != nullptr)
            token->getPage()->clear();
        token = scanNumericLiteral(token->getPage());
        return;
    }
    switch (c) {
        case '\"': {
            if (token != nullptr)
                token->getPage()->clear();
            token = scanStringLiteral(token->getPage());
            break;
        }

        case '\'': {
            if (token != nullptr)
                token->getPage()->clear();
            token = scanCharacterLiteral(token->getPage());
            break;
        }

        case '_': case '(': case ')': case '{': case '}': case '[': case ']': case ',': case ':': case ';': case '@': case '#': case '`': {
            {
                if (token != nullptr)
                    token->getPage()->clear();
                token = new(getPage()->allocateExclusivePage()) Punctuation(String(text->charAt(position)));
                position++;
                column++;
            }
            break;
        }

        case '/': case '+': case '*': case '%': case '&': case '|': case '^': case '~': {
            if (token != nullptr)
                token->getPage()->clear();
            token = scanOperator(token->getPage(), false);
            break;
        }

        case '<': case '>': {
            {
                position++;
                column++;
                if (position == end) {
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) InvalidToken();
                }
                else {
                    switch (text->charAt(position)) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': case '.': case ' ': case '\t': case '\r': case '\n': {
                            {
                                position--;
                                column--;
                                if (token != nullptr)
                                    token->getPage()->clear();
                                token = scanOperator(token->getPage(), true);
                            }
                            break;
                        }

                        default: {
                            if (token != nullptr)
                                token->getPage()->clear();
                            token = new(getPage()->allocateExclusivePage()) Punctuation(String(c));
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
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) InvalidToken();
                }
                else {
                    if (text->charAt(position) == '.') {
                        position--;
                        column--;
                        if (token != nullptr)
                            token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                    }
                    else {
                        if (token != nullptr)
                            token->getPage()->clear();
                        token = new(getPage()->allocateExclusivePage()) Punctuation(String('.'));
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
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) InvalidToken();
                }
                else {
                    if (text->charAt(position) != '>') {
                        position--;
                        column--;
                        if (token != nullptr)
                            token->getPage()->clear();
                        token = scanOperator(token->getPage(), true);
                    }
                    else {
                        if (token != nullptr)
                            token->getPage()->clear();
                        token = new(getPage()->allocateExclusivePage()) Punctuation(String("->"));
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
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) PostfixOperator(String("!"));
                }
                else {
                    switch (text->charAt(position)) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': case '.': case ' ': case '\t': case '\r': case '\n': {
                            {
                                position--;
                                column--;
                                if (token != nullptr)
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
                                    if (token != nullptr)
                                        token->getPage()->clear();
                                    token = scanOperator(token->getPage(), true);
                                }
                                else {
                                    if (token != nullptr)
                                        token->getPage()->clear();
                                    token = new(getPage()->allocateExclusivePage()) Punctuation(String("!"));
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
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) PostfixOperator(String("?"));
                }
                else {
                    switch (text->charAt(position)) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                            {
                                position--;
                                column--;
                                if (token != nullptr)
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
                                    if (token != nullptr)
                                        token->getPage()->clear();
                                    token = scanOperator(token->getPage(), true);
                                }
                                else {
                                    if (token != nullptr)
                                        token->getPage()->clear();
                                    token = new(getPage()->allocateExclusivePage()) Punctuation(String("?"));
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
                    if (token != nullptr)
                        token->getPage()->clear();
                    token = new(getPage()->allocateExclusivePage()) InvalidToken();
                }
                else {
                    switch (text->charAt(position)) {
                        case '/': case '=': case '+': case '!': case '*': case '%': case '&': case '|': case '^': case '~': {
                            {
                                position--;
                                column--;
                                if (token != nullptr)
                                    token->getPage()->clear();
                                token = scanOperator(token->getPage(), true);
                            }
                            break;
                        }

                        default: {
                            if (token != nullptr)
                                token->getPage()->clear();
                            token = new(getPage()->allocateExclusivePage()) Punctuation(String("="));
                        }
                    }
                }
            }
            break;
        }

        default: {
            if (token != nullptr)
                token->getPage()->clear();
            token = new(getPage()->allocateExclusivePage()) InvalidToken();
        }
    }
}

Identifier* Lexer::scanIdentifier(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* name = ) VarString(text->charAt(position));
    do {
        position++;
        column++;
        if (position == end)
            return Identifier(String(name));
        char c = text->charAt(position);
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_'))
            name->append(text->charAt(position));
        else
            return Identifier(String(name));
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
    VarString* operation = ) VarString(text->charAt(position));
    do {
        position++;
        column++;
        if (position == end) {
            if (whitespaceSkippedBefore)
                return BinaryOperator(String(operation));
            else
                return PostfixOperator(String(operation));
        }
        if (includeDots && (text->charAt(position) == '.')) {
            operation->append(text->charAt(position));
            continue;
        }
        switch (text->charAt(position)) {
            case '/': case '=': case '-': case '+': case '!': case '*': case '%': case '<': case '&': case '|': case '^': case '~': {
                operation->append(text->charAt(position));
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
                        switch (text->charAt(position)) {
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
                        return BinaryOperator(String(operation));
                    if ((!whitespaceSkippedBefore && whitespaceSkippedAfter))
                        return PostfixOperator(String(operation));
                    if ((whitespaceSkippedBefore && !whitespaceSkippedAfter))
                        return PrefixOperator(String(operation));
                }
            }
        }
    }
    while (true);
}

Token* Lexer::scanStringLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = ) VarString("");
    do {
        position++;
        column++;
        if (position == end)
            return InvalidToken();
        switch (text->charAt(position)) {
            case '\"': {
                {
                    position++;
                    column++;
                    return StringLiteral(String(value));
                }
                break;
            }

            case '\\': {
                {
                    position++;
                    column++;
                    switch (text->charAt(position)) {
                        case '\"': case '\\': case '\'': {
                            {
                                value->append('\\');
                                value->append(text->charAt(position));
                            }
                            break;
                        }

                        case 'n': {
                            value->append("\\n");
                            break;
                        }

                        case 'r': {
                            value->append("\\r");
                            break;
                        }

                        case 't': {
                            value->append("\\t");
                            break;
                        }

                        case '0': {
                            value->append("\\0");
                            break;
                        }

                        default: {
                            return InvalidToken();
                        }
                    }
                }
                break;
            }

            default: {
                value->append(text->charAt(position));
            }
        }
    }
    while (true);
}

Token* Lexer::scanCharacterLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = ) VarString("");
    do {
        position++;
        column++;
        if (position == end)
            return InvalidToken();
        switch (text->charAt(position)) {
            case '\'': {
                {
                    position++;
                    column++;
                    return CharacterLiteral(String(value));
                }
                break;
            }

            case '\\': {
                {
                    position++;
                    column++;
                    switch (text->charAt(position)) {
                        case '\"': case '\\': case '\'': {
                            value->append(text->charAt(position));
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
                            return InvalidToken();
                        }
                    }
                }
                break;
            }

            default: {
                {
                    value->append(text->charAt(position));
                }
            }
        }
    }
    while (true);
}

NumericLiteral* Lexer::scanNumericLiteral(_Page* _rp) {
    _Region _region; _Page* _p = _region.get();
    VarString* value = ) VarString(text->charAt(position));
    do {
        position++;
        column++;
        if (position == end)
            return NumericLiteral(String(value));
        char c = text->charAt(position);
        if ((c >= '0') && (c <= '9'))
            value->append(text->charAt(position));
        else
            return NumericLiteral(String(value));
    }
    while (true);
}

bool Lexer::skipWhitespace() {
    whitespaceSkipped = false;
    do {
        if (position == end)
            return true;
        switch (text->charAt(position)) {
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
                    if (text->charAt(position) == '/') {
                        whitespaceSkipped = true;
                        handleSingleLineComment();
                    }
                    else {
                        if (text->charAt(position) == '*') {
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
        switch (text->charAt(position)) {
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
                {
                    position++;
                    column++;
                    continue;
                }
            }
        }
    }
    while (true);
}

void Lexer::handleMultiLineComment() {
    do {
        if (position == end)
            return;
        switch (text->charAt(position)) {
            case '/': {
                {
                    position++;
                    column++;
                    if (position == end)
                        return;
                    else {
                        if (text->charAt(position) == '*')
                            handleMultiLineComment();
                        else
                            return;
                    }
                }
                break;
            }

            case '*': {
                {
                    position++;
                    column++;
                    if (position == end)
                        return;
                    else {
                        if (text->charAt(position) == '/') {
                            position++;
                            column++;
                            return;
                        }
                    }
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

            default: {
                {
                    position++;
                    column++;
                    continue;
                }
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
        return nullptr;
    Identifier* identifier = (Identifier*)token;
    return String(identifier->name);
}

bool Lexer::parsePunctuation(String* fixedString) {
    if (!(token->_isPunctuation()))
        return false;
    Punctuation* punctuation = (Punctuation*)token;
    return punctuation->sign->equals(fixedString);
}

String* Lexer::parseOperator(_Page* _rp) {
    if (!(token->_isOperator()))
        return nullptr;
    Operator* op = (Operator*)token;
    return String(op->operation);
}

Literal* Lexer::parseLiteral(_Page* _rp) {
    if (!(token->_isLiteral()))
        return nullptr;
    if (token->_isStringLiteral()) {
        StringLiteral* stringLiteral = (StringLiteral*)token;
        return StringLiteral(String(stringLiteral->string));
    }
    if (token->_isCharacterLiteral()) {
        CharacterLiteral* characterLiteral = (CharacterLiteral*)token;
        return CharacterLiteral(String(characterLiteral->value));
    }
    if (token->_isNumericLiteral()) {
        NumericLiteral* numericLiteral = (NumericLiteral*)token;
        return NumericLiteral(String(numericLiteral->value));
    }
    return nullptr;
}

String* Lexer::parsePrefixOperator(_Page* _rp) {
    if (!(token->_isPrefixOperator()))
        return nullptr;
    Operator* op = (Operator*)token;
    return String(op->operation);
}

String* Lexer::parseBinaryOperator(_Page* _rp) {
    if (!(token->_isBinaryOperator())) {
        if ((token->_isPunctuation()) && ((((Punctuation*)token)->sign->equals("<")) || (((Punctuation*)token)->sign->equals(">")))) {
            Operator* op = (Operator*)token;
            return String(op->operation);
        }
        return nullptr;
    }
    Operator* op = (Operator*)token;
    return String(op->operation);
}

String* Lexer::parsePostfixOperator(_Page* _rp) {
    if (!(token->_isPostfixOperator()))
        return nullptr;
    Operator* op = (Operator*)token;
    return String(op->operation);
}

bool Lexer::isAtEnd() {
    return position == end;
}

Position* Lexer::getPosition(_Page* _rp) {
    return Position(line, column);
}

Position* Lexer::getPreviousPosition(_Page* _rp) {
    return Position(previousLine, previousColumn);
}


}
