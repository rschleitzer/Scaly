#ifndef _Lexer_h
#define _Lexer_h
#include "../../scaly.h"
using namespace scaly;
using namespace scaly::memory;

using namespace scaly::containers;

struct EmptyToken : Object {

};

struct InvalidToken : Object {

};

struct ColonToken : Object {

};

struct IdentifierToken : Object {
    Vector<char> name;

    IdentifierToken(Vector<char> name);
};

struct AttributeToken : Object {
    Vector<char> name;

    AttributeToken(Vector<char> name);
};

struct PunctuationToken : Object {
    char sign;

    PunctuationToken(char sign);
};

struct StringToken : Object {
    Vector<char> value;

    StringToken(Vector<char> value);
};

struct CharacterToken : Object {
    Vector<char> value;

    CharacterToken(Vector<char> value);
};

struct FragmentToken : Object {
    Vector<char> value;

    FragmentToken(Vector<char> value);
};

struct IntegerToken : Object {
    Vector<char> value;

    IntegerToken(Vector<char> value);
};

struct BooleanToken : Object {
    bool value;

    BooleanToken(bool value);
};

struct FloatingPointToken : Object {
    Vector<char> value;

    FloatingPointToken(Vector<char> value);
};

struct HexToken : Object {
    Vector<char> value;

    HexToken(Vector<char> value);
};

struct LiteralToken {
    LiteralToken(StringToken _StringToken);
    LiteralToken(CharacterToken _CharacterToken);
    LiteralToken(FragmentToken _FragmentToken);
    LiteralToken(IntegerToken _IntegerToken);
    LiteralToken(BooleanToken _BooleanToken);
    LiteralToken(FloatingPointToken _FloatingPointToken);
    LiteralToken(HexToken _HexToken);
    enum {
        String,
        Character,
        Fragment,
        Integer,
        Boolean,
        FloatingPoint,
        Hex,
} _tag;
    union {
        StringToken _String;
        CharacterToken _Character;
        FragmentToken _Fragment;
        IntegerToken _Integer;
        BooleanToken _Boolean;
        FloatingPointToken _FloatingPoint;
        HexToken _Hex;
    };
};
struct Token {
    Token(EmptyToken _EmptyToken);
    Token(InvalidToken _InvalidToken);
    Token(IdentifierToken _IdentifierToken);
    Token(AttributeToken _AttributeToken);
    Token(PunctuationToken _PunctuationToken);
    Token(LiteralToken _LiteralToken);
    Token(ColonToken _ColonToken);
    enum {
        Empty,
        Invalid,
        Identifier,
        Attribute,
        Punctuation,
        Literal,
        Colon,
} _tag;
    union {
        EmptyToken _Empty;
        InvalidToken _Invalid;
        IdentifierToken _Identifier;
        AttributeToken _Attribute;
        PunctuationToken _Punctuation;
        LiteralToken _Literal;
        ColonToken _Colon;
    };
};
struct Lexer : Object {
    Token token;
    char* character;
    StringIterator chars;
    size_t previous_position;
    size_t position;

    Lexer(Token token, char* character, StringIterator chars, size_t previous_position, size_t position);
    Lexer ();
    Lexer(String deck);
    void read_character();
    static void advance();
    static void empty();
};

#endif