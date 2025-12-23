#pragma once

#include "llvm/ADT/StringRef.h"
#include <cstddef>
#include <variant>

namespace scaly {

// Literal token variants
struct StringLiteral { llvm::StringRef Value; };
struct CharacterLiteral { llvm::StringRef Value; };
struct FragmentLiteral { llvm::StringRef Value; };
struct IntegerLiteral { llvm::StringRef Value; };
struct BooleanLiteral { bool Value; };
struct FloatingPointLiteral { llvm::StringRef Value; };
struct HexLiteral { llvm::StringRef Value; };

using Literal = std::variant<
    StringLiteral,
    CharacterLiteral,
    FragmentLiteral,
    IntegerLiteral,
    BooleanLiteral,
    FloatingPointLiteral,
    HexLiteral
>;

// Token variants
struct EmptyToken {};
struct InvalidToken {};
struct ColonToken {};
struct IdentifierToken { llvm::StringRef Name; };
struct AttributeToken { llvm::StringRef Name; };
struct PunctuationToken { char Sign; };
struct LiteralToken { Literal Value; };

using Token = std::variant<
    EmptyToken,
    InvalidToken,
    ColonToken,
    IdentifierToken,
    AttributeToken,
    PunctuationToken,
    LiteralToken
>;

// Helper to check token type
template<typename T>
bool is(const Token& tok) {
    return std::holds_alternative<T>(tok);
}

template<typename T>
const T* get_if(const Token& tok) {
    return std::get_if<T>(&tok);
}

class Lexer {
public:
    explicit Lexer(llvm::StringRef Source);

    void advance();
    void empty();

    const Token& token() const { return Token_; }
    size_t position() const { return Position_; }
    size_t previousPosition() const { return PreviousPosition_; }
    bool isAtEnd() const { return Current_ == nullptr; }

    // Parser helpers
    bool parseKeyword(llvm::StringRef Keyword);
    llvm::StringRef parseIdentifier();
    llvm::StringRef peekIdentifier();  // Returns identifier name without consuming
    llvm::StringRef parseAttribute();
    bool parsePunctuation(char C);
    bool parseColon();

private:
    llvm::StringRef Source_;
    const char* Current_;
    const char* End_;
    Token Token_;
    size_t Position_ = 0;
    size_t PreviousPosition_ = 0;

    void readCharacter();
    void skipWhitespace(bool SkipLineFeed);
    void handleSingleLineComment();
    void handleMultiLineComment();

    Token scanIdentifier();
    Token scanAttribute();
    Token scanOperator();
    Token scanLineFeed();
    Token scanStringLiteral();
    Token scanStringIdentifier();
    Token scanFragmentLiteral();
    Token scanNumericLiteral();
    Token scanIntegerLiteral(const char* Start, size_t Length);
    Token scanFraction(const char* Start, size_t Length);
    Token scanExponent(const char* Start, size_t Length);
    Token scanHexLiteral(const char* Start, size_t Length);
    Token scanBooleanLiteral();
};

} // namespace scaly
