#include "Lexer.h"

namespace scaly {

Lexer::Lexer(llvm::StringRef Source)
    : Source_(Source),
      Current_(Source.empty() ? nullptr : Source.data()),
      End_(Source.data() + Source.size()),
      Token_(EmptyToken{}) {
    if (Current_) {
        skipWhitespace(true);
    }
}

void Lexer::readCharacter() {
    if (Current_ == nullptr) return;
    ++Current_;
    ++Position_;
    if (Current_ >= End_) {
        Current_ = nullptr;
    }
}

void Lexer::empty() {
    Token_ = EmptyToken{};
}

void Lexer::advance() {
    PreviousPosition_ = Position_;

    if (Current_ == nullptr) {
        return;
    }

    char C = *Current_;

    // Identifier: starts with letter
    if ((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z')) {
        Token_ = scanIdentifier();
        skipWhitespace(false);
        return;
    }

    // Number: starts with 1-9
    if (C >= '1' && C <= '9') {
        Token_ = scanIntegerLiteral(Current_, 0);
        skipWhitespace(false);
        return;
    }

    // Operators
    switch (C) {
    case '+': case '-': case '*': case '/': case '=':
    case '%': case '&': case '|': case '~': case '<': case '>':
        Token_ = scanOperator();
        skipWhitespace(false);
        return;

    // Punctuation (followed by no whitespace skip for newlines)
    case '}': case ')': case ']': case '.':
    case '?': case '!': case '$': case '#': case '^':
        Token_ = PunctuationToken{*Current_};
        readCharacter();
        skipWhitespace(false);
        return;

    // Punctuation (followed by whitespace skip including newlines)
    case '{': case '(': case '[': case ',':
        Token_ = PunctuationToken{*Current_};
        readCharacter();
        skipWhitespace(true);
        return;

    case '\n':
        Token_ = scanLineFeed();
        break;

    case ':':
        readCharacter();
        Token_ = ColonToken{};
        break;

    case '0':
        Token_ = scanNumericLiteral();
        break;

    case '@':
        readCharacter();
        Token_ = scanAttribute();
        break;

    case '"':
        Token_ = scanStringLiteral();
        break;

    case '\'':
        Token_ = scanStringIdentifier();
        break;

    case '`':
        Token_ = scanFragmentLiteral();
        break;

    default:
        Token_ = InvalidToken{};
        break;
    }

    skipWhitespace(false);
}

Token Lexer::scanLineFeed() {
    while (true) {
        readCharacter();
        skipWhitespace(false);
        if (Current_ == nullptr) {
            return ColonToken{};
        }
        if (*Current_ == '\n') {
            continue;
        }
        return ColonToken{};
    }
}

Token Lexer::scanIdentifier() {
    const char* Start = Current_;
    size_t Length = 0;

    while (true) {
        if (Current_ == nullptr) {
            if (Length == 0) {
                return InvalidToken{};
            }
            return IdentifierToken{llvm::StringRef(Start, Length)};
        }

        char C = *Current_;
        if ((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') ||
            (C >= '0' && C <= '9') || C == '_') {
            readCharacter();
            ++Length;
        } else {
            return IdentifierToken{llvm::StringRef(Start, Length)};
        }
    }
}

Token Lexer::scanAttribute() {
    const char* Start = Current_;
    size_t Length = 0;

    while (true) {
        if (Current_ == nullptr) {
            if (Length == 0) {
                return InvalidToken{};
            }
            return AttributeToken{llvm::StringRef(Start, Length)};
        }

        char C = *Current_;
        if ((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') ||
            (C >= '0' && C <= '9') || C == '_') {
            readCharacter();
            ++Length;
        } else {
            return AttributeToken{llvm::StringRef(Start, Length)};
        }
    }
}

Token Lexer::scanOperator() {
    const char* Start = Current_;
    size_t Length = 0;

    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            if (Length == 1) {
                return InvalidToken{};
            }
            return IdentifierToken{llvm::StringRef(Start, Length)};
        }

        switch (*Current_) {
        case '+': case '-': case '*': case '/': case '=':
        case '%': case '&': case '|': case '^': case '~':
        case '<': case '>':
            continue;
        default:
            return IdentifierToken{llvm::StringRef(Start, Length)};
        }
    }
}

Token Lexer::scanStringLiteral() {
    const char* Start = Current_ + 1;
    size_t Length = 0;

    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return InvalidToken{};
        }

        char C = *Current_;
        switch (C) {
        case '"':
            readCharacter();
            return LiteralToken{StringLiteral{llvm::StringRef(Start, Length - 1)}};

        case '\\':
            readCharacter();
            ++Length;
            if (Current_ == nullptr) {
                return InvalidToken{};
            }
            switch (*Current_) {
            case '"': case '\\': case '\'': case 'n': case 'r': case 't': case '0':
                break;
            case 'u':
                // Unicode escape: \uXXXX (4 hex digits)
                for (int i = 0; i < 4; ++i) {
                    readCharacter();
                    ++Length;
                    if (Current_ == nullptr) {
                        return InvalidToken{};
                    }
                    char H = *Current_;
                    if (!((H >= '0' && H <= '9') || (H >= 'a' && H <= 'f') || (H >= 'A' && H <= 'F'))) {
                        return InvalidToken{};
                    }
                }
                break;
            default:
                return InvalidToken{};
            }
            break;
        }
    }
}

Token Lexer::scanStringIdentifier() {
    const char* Start = Current_ + 1;
    size_t Length = 0;

    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return InvalidToken{};
        }

        char C = *Current_;
        switch (C) {
        case '\'':
            readCharacter();
            return LiteralToken{CharacterLiteral{llvm::StringRef(Start, Length - 1)}};

        case '\\':
            readCharacter();
            ++Length;
            if (Current_ == nullptr) {
                return InvalidToken{};
            }
            switch (*Current_) {
            case '"': case '\\': case '\'': case 'n': case 'r': case 't': case '0':
                break;
            case 'u':
                // Unicode escape: \uXXXX (4 hex digits)
                for (int i = 0; i < 4; ++i) {
                    readCharacter();
                    ++Length;
                    if (Current_ == nullptr) {
                        return InvalidToken{};
                    }
                    char H = *Current_;
                    if (!((H >= '0' && H <= '9') || (H >= 'a' && H <= 'f') || (H >= 'A' && H <= 'F'))) {
                        return InvalidToken{};
                    }
                }
                break;
            default:
                return InvalidToken{};
            }
            break;
        }
    }
}

Token Lexer::scanFragmentLiteral() {
    const char* Start = Current_ + 1;
    size_t Length = 0;

    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return InvalidToken{};
        }

        switch (*Current_) {
        case '`':
            readCharacter();
            return LiteralToken{FragmentLiteral{llvm::StringRef(Start, Length - 1)}};

        case '\\':
            readCharacter();
            ++Length;
            if (Current_ == nullptr) {
                return InvalidToken{};
            }
            switch (*Current_) {
            case '`': case '\\': case '\'': case 'n': case 'r': case 't': case '0':
                break;
            default:
                return InvalidToken{};
            }
            break;
        }
    }
}

Token Lexer::scanNumericLiteral() {
    const char* Start = Current_;
    size_t Length = 0;

    readCharacter();
    ++Length;

    if (Current_ == nullptr) {
        return LiteralToken{IntegerLiteral{llvm::StringRef(Start, Length)}};
    }

    char C = *Current_;

    if (C >= '0' && C <= '9') {
        return scanIntegerLiteral(Start, Length);
    }

    switch (C) {
    case '.':
        return scanFraction(Start, Length);
    case 'E': case 'e':
        return scanExponent(Start, Length);
    case 'x':
        return scanHexLiteral(Start, Length);
    case 'B':
        return scanBooleanLiteral();
    default:
        return LiteralToken{IntegerLiteral{llvm::StringRef(Start, Length)}};
    }
}

Token Lexer::scanIntegerLiteral(const char* Start, size_t Length) {
    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return LiteralToken{IntegerLiteral{llvm::StringRef(Start, Length)}};
        }

        char C = *Current_;

        if (C >= '0' && C <= '9') {
            continue;
        }

        switch (C) {
        case '.':
            return scanFraction(Start, Length);
        case 'E': case 'e':
            return scanExponent(Start, Length);
        default:
            return LiteralToken{IntegerLiteral{llvm::StringRef(Start, Length)}};
        }
    }
}

Token Lexer::scanFraction(const char* Start, size_t Length) {
    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return LiteralToken{FloatingPointLiteral{llvm::StringRef(Start, Length)}};
        }

        char C = *Current_;

        if (C >= '0' && C <= '9') {
            continue;
        }

        switch (C) {
        case 'E': case 'e':
            return scanExponent(Start, Length);
        default:
            return LiteralToken{FloatingPointLiteral{llvm::StringRef(Start, Length)}};
        }
    }
}

Token Lexer::scanExponent(const char* Start, size_t Length) {
    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return LiteralToken{FloatingPointLiteral{llvm::StringRef(Start, Length)}};
        }

        char C = *Current_;

        if (C >= '0' && C <= '9') {
            continue;
        }

        return LiteralToken{FloatingPointLiteral{llvm::StringRef(Start, Length)}};
    }
}

Token Lexer::scanHexLiteral(const char* Start, size_t Length) {
    while (true) {
        readCharacter();
        ++Length;

        if (Current_ == nullptr) {
            return LiteralToken{HexLiteral{llvm::StringRef(Start, Length)}};
        }

        char C = *Current_;

        if ((C >= '0' && C <= '9') || (C >= 'A' && C <= 'F') || (C >= 'a' && C <= 'f')) {
            continue;
        }

        return LiteralToken{HexLiteral{llvm::StringRef(Start, Length)}};
    }
}

Token Lexer::scanBooleanLiteral() {
    readCharacter();

    if (Current_ == nullptr) {
        return InvalidToken{};
    }

    char C = *Current_;

    if (C != '0' && C != '1') {
        return InvalidToken{};
    }

    readCharacter();
    return LiteralToken{BooleanLiteral{C == '1'}};
}

void Lexer::skipWhitespace(bool SkipLineFeed) {
    while (true) {
        if (Current_ == nullptr) {
            return;
        }

        switch (*Current_) {
        case ' ':
        case '\t':
        case '\r':
            readCharacter();
            continue;

        case '\n':
            if (SkipLineFeed) {
                readCharacter();
                continue;
            }
            return;

        case '\\':
            readCharacter();
            if (Current_ == nullptr) {
                return;
            }
            switch (*Current_) {
            case '\r':
                readCharacter();
                readCharacter();
                continue;
            case '\n':
                readCharacter();
                continue;
            }
            return;

        case ';':
            readCharacter();
            if (Current_ == nullptr) {
                return;
            }
            if (*Current_ != '*') {
                readCharacter();
                handleSingleLineComment();
            } else {
                readCharacter();
                handleMultiLineComment();
            }
            continue;

        default:
            return;
        }
    }
}

void Lexer::handleSingleLineComment() {
    while (true) {
        if (Current_ == nullptr) {
            return;
        }
        if (*Current_ == '\n') {
            readCharacter();
            return;
        }
        readCharacter();
    }
}

void Lexer::handleMultiLineComment() {
    while (true) {
        if (Current_ == nullptr) {
            return;
        }

        switch (*Current_) {
        case ';':
            readCharacter();
            if (Current_ == nullptr) {
                return;
            }
            if (*Current_ == '*') {
                readCharacter();
                handleMultiLineComment(); // Nested comment
            }
            continue;

        case '*':
            readCharacter();
            if (Current_ == nullptr) {
                return;
            }
            if (*Current_ == ';') {
                readCharacter();
                return; // End of comment
            }
            continue;

        default:
            readCharacter();
            continue;
        }
    }
}

// Parser helpers

bool Lexer::parseKeyword(llvm::StringRef Keyword) {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    if (auto* Ident = get_if<IdentifierToken>(Token_)) {
        if (Ident->Name == Keyword) {
            empty();
            return true;
        }
    }
    return false;
}

llvm::StringRef Lexer::parseIdentifier() {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    if (auto* Ident = get_if<IdentifierToken>(Token_)) {
        llvm::StringRef Name = Ident->Name;
        empty();
        return Name;
    }
    return llvm::StringRef();
}

llvm::StringRef Lexer::peekIdentifier() {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    if (auto* Ident = get_if<IdentifierToken>(Token_)) {
        return Ident->Name;  // Don't consume - just return the name
    }
    return llvm::StringRef();
}

llvm::StringRef Lexer::parseAttribute() {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    if (auto* Attr = get_if<AttributeToken>(Token_)) {
        llvm::StringRef Name = Attr->Name;
        empty();
        return Name;
    }
    return llvm::StringRef();
}

bool Lexer::parsePunctuation(char C) {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    if (auto* Punct = get_if<PunctuationToken>(Token_)) {
        if (Punct->Sign == C) {
            empty();
            return true;
        }
    }
    return false;
}

bool Lexer::parseColon() {
    if (is<EmptyToken>(Token_)) {
        advance();
    }

    // Accept Colon, Empty (linefeeds become ColonToken via scanLineFeed)
    if (is<ColonToken>(Token_) || is<EmptyToken>(Token_)) {
        empty();
        return true;
    }
    return false;
}

} // namespace scaly
